#include <windows.h>

//
// TODO: Should FlushInstructionCache() be used?
//

HANDLE stdOut;

#define NOF_BREAKPOINTS 2

char  byte_orig;
char *func_addrs[NOF_BREAKPOINTS];
char  old_instr[NOF_BREAKPOINTS];

void out(char const* text) {
   DWORD charsWritten;   
   WriteConsole(stdOut, text, lstrlen(text), &charsWritten, NULL);
}


LONG WINAPI ExHandler(PEXCEPTION_POINTERS exPtr) {

    if (exPtr->ExceptionRecord->ExceptionCode != EXCEPTION_BREAKPOINT) {
      out("Expected EXCEPTION_BREAKPOINT!\n");
      return EXCEPTION_CONTINUE_SEARCH;
    }

    for (int i=0; i<NOF_BREAKPOINTS; i++) {
     if (exPtr->ContextRecord->Eip == (int) func_addrs[i]) {
        char buf[123];
        wsprintf(buf, "Breakpoint %d was hit\n", i);
        out(buf);
       *func_addrs[i] = old_instr[i];
        SetThreadContext(GetCurrentThread(), exPtr->ContextRecord);
     }
    }


    out("Should not be reached\n");

    return EXCEPTION_CONTINUE_SEARCH;
}


int func_0(int i) {
    return i*3;
}

int func_1(int i) {
    return func_0(i*2);
}

int main() {
  stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  AddVectoredExceptionHandler(1, ExHandler  );

  func_addrs[0] = (char*) func_0;
  func_addrs[1] = (char*) func_1;

  for (int i=0; i<NOF_BREAKPOINTS; i++) {

     DWORD oldProtection;
     VirtualProtect(func_addrs[0], 0x4000, PAGE_EXECUTE_READWRITE, &oldProtection);

     old_instr[i]  = *func_addrs[i];
    *func_addrs[i] = 0xcc;

  }

  int res = func_1(7);

  char buf[123];
  wsprintf(buf, "res = %d\n", res);

  out(buf);
}

