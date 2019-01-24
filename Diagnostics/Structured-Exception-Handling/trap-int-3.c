//
//  Compiling
//    cl trap-int-3.c user32.lib
//
#include <windows.h>

//
// TODO: Should FlushInstructionCache() be used?
//

HANDLE stdOut;

#define NOF_BREAKPOINTS 4

char  byte_orig;
char *func_addrs[NOF_BREAKPOINTS];
char  old_instr [NOF_BREAKPOINTS];
int   nrLastFuncBreakpointHit;

void out(char const* text) {
   DWORD charsWritten;   
   WriteConsole(stdOut, text, lstrlen(text), &charsWritten, NULL);
}


LONG WINAPI ExHandler(PEXCEPTION_POINTERS exPtr) {
    int i;

    if (exPtr->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
//    out("Expected EXCEPTION_BREAKPOINT!\n");
//    return EXCEPTION_CONTINUE_SEARCH;

      for (i=0; i<NOF_BREAKPOINTS; i++) {
       //
       // Trying to determine the function that caused the breakpoint
       // exception.
  
          if (exPtr->ContextRecord->Eip == (int) func_addrs[i]) {
           //
           // The EIP register is equal to the address of one of our
           // functions. Reporting it to the console:
           //
              char buf[123];
              wsprintf(buf, "Breakpoint %d was hit\n", i);
              out(buf);

           //
           // Store the number of the breakpoint so that we can
           // set the breakpoint again after single stepping
           // the »current« instruction:
           //
              nrLastFuncBreakpointHit = i;
  
           //
           // In order to proceed with the execution of the program, we
           // restore the old value of the byte that was replaced by
           // the int-3 instruction:
           //
             *func_addrs[i] = old_instr[i];
  
           //
           // Set TF bit in order to single step the next
           // instruction. This allows to set the break point
           // again after the single step instruction.
           //
  
              exPtr->ContextRecord->EFlags |= 0x100;
  
           //
           // Resume execution:
           //
              SetThreadContext(GetCurrentThread(), exPtr->ContextRecord);
          }
       }
    }
    else if (exPtr->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {

    //
    // The processor is one instruction behind the last breakpoint that was
    // hit. We can now set the breakpoint again.

      *func_addrs[nrLastFuncBreakpointHit] = 0xcc;

    //
    // Apparently, the TF flag is reset after the single execution, it
    // needs not be reset.
    //
    // Thus, we can resume execution again:
    //


       SetThreadContext(GetCurrentThread(), exPtr->ContextRecord);

    }
    else {

    //
    // Should never be reached.
    //
       out("Either EXCEPTION_CONTINUE_SEARCH or EXCEPTION_SINGLE_STEP was expected.\n");
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

int func_0(int);
int func_1(int);
int func_2(int);
int func_3(int);

int func_0(int i) {
  if (i == 0) return func_1(i+1);
  if (i == 2) return func_1(i+1);
  return i;
}

int func_1(int i) {
  if (i == 1) return func_0(i+1);
  if (i == 3) return func_1(i+1);
  if (i == 4) return func_2(i+1);
  return i;
}

int func_2(int i) {
  if (i == 5) return func_3(i+1);
  return i;
}

int func_3(int i) {
  if (i == 6) return func_2(i+1);
  return i;
}


int main() {
  int  i, res;
  char buf[123];

  stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  AddVectoredExceptionHandler(1, ExHandler);

  func_addrs[0] = (char*) func_0;
  func_addrs[1] = (char*) func_1;
  func_addrs[2] = (char*) func_2;
  func_addrs[3] = (char*) func_3;

  for (i=0; i<NOF_BREAKPOINTS; i++) {
  //
  // Setting the breakpoints at the functions addresses.
  //
  // First, we need to make the code segment writable to be able to
  // insert the breakpoint instruction. Otherwise, the modification of
  // the (read-only) code segment would cause an exception.
  //

     DWORD oldProtection;
     VirtualProtect(func_addrs[0], 1, PAGE_EXECUTE_READWRITE, &oldProtection);

  //
  // We also want to store the value of the byte before we set the int-3
  // instruction:
  //
     old_instr[i]  = *func_addrs[i];

  //
  // Finally, we can inject the int-3 instruction (0xcc):
  //
    *func_addrs[i] = 0xcc;

  }

  res = func_0(0);

  wsprintf(buf, "res = %d\n", res);

  out(buf);
}
