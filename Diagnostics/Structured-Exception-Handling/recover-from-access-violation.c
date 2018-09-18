#include <windows.h>

HANDLE stdOut;

void out(char const* text) {
   DWORD charsWritten;
   WriteConsole(stdOut, text, lstrlen(text), &charsWritten, NULL);
}

LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS exPtr) {
    out("Exception handler\r\n");

    char buf[123];

    if (exPtr->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
      out("Expected EXCEPTION_ACCESS_VIOLATION!\n");
      return EXCEPTION_CONTINUE_SEARCH;
    }

    wsprintf(buf, "    ExceptionAddress: %x  EIP %x\n", exPtr->ExceptionRecord->ExceptionAddress, exPtr->ContextRecord->Eip);
    out(buf);


 //
 // The instruction that caused the exception is 6 byte long (at least on
 // 32 Bit Windows, compare with recover-from-access-violation.objdump).
 // We increase the instruction pointer (EIP) by these 6 bytes so as
 // to recover from the violation...
 //
   (exPtr->ContextRecord->Eip) += 6;

 //
 // ... and set the changed contect record into the current record
 // which causes the thread to continue after the bad instruction:
 //
    SetThreadContext(GetCurrentThread(), exPtr->ContextRecord);
    out("Never reached\n");

    return EXCEPTION_CONTINUE_SEARCH;
}


int main() {
  stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  AddVectoredExceptionHandler(1, ExceptionHandler  );

  int *ptr;
  ptr = (int*) 0;

  out("Going to cause access violation\n");
 *ptr = 42;
  out("After access violation\n");

}
