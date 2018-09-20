#include <windows.h>
#include <psapi.h>

HANDLE stdOut;

void out(char const* text) {
   DWORD charsWritten;
   WriteConsole(stdOut, text, lstrlen(text), &charsWritten, NULL);
}

int main() {

  stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  HMODULE process = GetCurrentProcess();

#define nofModules 256
  HMODULE modules[nofModules];
  DWORD   bytesNeeded;


  if (! EnumProcessModules(
      process,
      modules, // HMODULE *lphModule,
      sizeof(modules),
     &bytesNeeded
  )) {
      out("EnumProcessModulesEx returned FALSE!\n");
      return 1;
  }

  for (int i=0; i<bytesNeeded/sizeof(HANDLE); i++) {

    char baseName[MAX_PATH];

    GetModuleBaseName(process, modules[i], baseName, MAX_PATH);
    out(baseName); out("\n");
  }

}
