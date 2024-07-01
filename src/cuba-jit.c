#include "nan.h"

void cj_write() {
  NanJit jit = NanJitCreate();
  NanJitAppendNS(&jit, NA_PRINT, NanStringFromStr("hellow, word\n"));
  NanJitFSave(&jit, "E:/so2u/GITHUB/cuba/cuba-c/cuba/temp.co");
}
void cj_read() {
  NanJit jit = NanJitCreate();
  NanJitFLoad(&jit, "E:/so2u/GITHUB/cuba/cuba-c/cuba/temp.co");
  NanJitRun(&jit);
}

int main(int argc, char** argv) {
  NanArgumentRow args = NanParseArguments(argc, argv);
  // if (NanArgumentIsInRowAbs(&args, "--help")) {
  //   printf("  JIT-HELP\n");
  //   printf("  USAGE:\n");
  //   printf("   %s|>   %scuba-jit %s<path/to/binary>%s   %s<| %sdefault usage%s\n", NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG), NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG));
  //   printf("   %s|>   %scuba-jit %s--help          %s   %s<| %sshow this page%s\n", NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG), NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG));
  //   exit(0);
  // }

  // NanString* path = NanArgumentRowFindPathOne(&args);
  // if (NanStringIsNull(path)) { NAN_PANIC_CODE("path undefined, use --help"); }

  // NanString finner = NanReadFile(path);
  // NanDescryptor fdescr = NanDescryptorFromString(&finner);
  // NanLexer lexer = NanDescryptorBack(&fdescr);
  // cj_write();
  cj_read();
  return 0;
}