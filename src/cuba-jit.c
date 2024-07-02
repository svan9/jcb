#include "nan.h"
static char* test_file = "./temp.co"; 

void cj_write() {
  NanJit jit = NanJitCreate();
  NanJitAppendNS(&jit, NA_WRITE, NanStringFromStr("\x01hellow, word\n"));
  NanJitFSave(&jit, test_file, NanJitResolved);
}

void cj_read() {
  NanJit jit = NanJitCreate();
  NanJitFLoad(&jit, test_file, NanJitResolved);
  NanJitRun(&jit);
}

void parse_args(int argc, char** argv) {
  NanArgumentRow args = NanParseArguments(argc, argv);
  if (NanArgumentIsInRowAbs(&args, "--help")) {
    printf("  JIT-HELP\n");
    printf("  USAGE:\n");
    printf("   %s|>   %scuba-jit %s<path/to/binary>%s   %s<|%s %sdefault usage%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG));
    printf("   %s|>   %scuba-jit %s--help          %s   %s<|%s %sshow this page%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG),NanConsoleRC(NanCharConsoleGrayFG));
    exit(0);
  } 
  else if (NanArgumentIsInRowAbs(&args, "--test")) {
    cj_write();
    cj_read();
    exit(0);
  }

  NanString* path = NanArgumentRowFindPathOne(&args);
  if (NanStringIsNull(path)) { ENAN_PANIC_CODE_REL("using error" , "use --help\n"); }

  NanJit jit = NanJitCreate();
  NanJitFLoad(&jit, NanStringC_Str(path), NanJitResolved);
  NanJitRun(&jit);
}

int main(int argc, char** argv) {
  parse_args(argc, argv);
  return 0;
}