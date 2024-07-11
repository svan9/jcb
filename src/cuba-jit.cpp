#include "nan.h"
#include "jit.cpp"

static const char* test_file = "./temp.co"; 

void cj_write() {
  NanJit jit;
  jit.append(NanJit::Token::ActionKind::WRITE, 
    std::vector<NanJit::Argument>({
      NanJit::Argument::from<ubyte>(NanJit::Argument::Kind::Sign, NanJit::Signs::stdout_p), 
      NanJit::Argument::from<NanViewString>(NanJit::Argument::Kind::Text, NanViewString("hellow, word\n"))
    })
  );
  jit.append(NanJit::Token::ActionKind::WRITE, 
    std::vector<NanJit::Argument>({
      NanJit::Argument::from<ubyte>(NanJit::Argument::Kind::Sign, NanJit::Signs::stdout_p), 
      NanJit::Argument::from<NanViewString>(NanJit::Argument::Kind::Text, NanViewString("hellow, wordx2\n"))
    })
  );
  jit.append(NanJit::Token::ActionKind::WRITE,
    std::vector<NanJit::Argument>({
      NanJit::Argument::from<ubyte>(NanJit::Argument::Kind::Sign, NanJit::Signs::stdout_p),
      NanJit::Argument::from<NanViewString>(NanJit::Argument::Kind::Text, NanViewString("hellow, word twice\n")) 
    })
  );
  jit.save((char*)test_file, NanJitResolved);
}

void cj_read() {
  NanJit jit;
  jit.load((char*)test_file, NanJitResolved);
  jit.run();
}

void parse_args(int argc, char** argv) {
  NanArgumentsRow args;
  args.parse(argc, argv);
  std::string temp;
  if (args.includes("--help")) {
    printf("  JIT-HELP\n");
    printf("  USAGE:\n");
    printf("   %s|>   %sjcb %s<path/to/binary>%s   %s<|%s %sdefault usage%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG));
    printf("   %s|>   %sjcb %s--test          %s   %s<|%s %stest: write and read binary%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG),NanConsoleRC(NanCharConsoleGrayFG));
    printf("   %s|>   %sjcb %s--test-write    %s   %s<|%s %stest: write binary%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG),NanConsoleRC(NanCharConsoleGrayFG));
    printf("   %s|>   %sjcb %s--test-read     %s   %s<|%s %stest: read binary%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG),NanConsoleRC(NanCharConsoleGrayFG));
    printf("   %s|>   %sjcb %s--help          %s   %s<|%s %sshow this page%s\n", 
      NanConsoleRC(NanCharConsoleCyanFG), NanConsoleRC(NanCharConsoleGrayFG),
      NanConsoleRC(NanCharConsoleCyanFG),NanConsoleRC(NanCharConsoleGrayFG));
    exit(0);
  } 
  else if (args.includes("--test")) {
    cj_write();
    cj_read();
    exit(0);
  }
  else if (args.includes("--test-write")) {
    cj_write();
    exit(0);
  }
  else if (args.includes("--test-read")) {
    cj_read();
    exit(0);
  }
  else if (args.includes("--cmd-write", temp)) {
    NanJit jit;
    jit.append(NanJit::Token::ActionKind::WRITE, 
      std::vector<NanJit::Argument>({
        NanJit::Argument::from<ubyte>(NanJit::Argument::Kind::Sign, NanJit::Signs::stdout_p), 
        NanJit::Argument::from<std::string>(NanJit::Argument::Kind::Text, temp)
      })
    );
    jit.save((char*)test_file, NanJitResolved);
    exit(0);
  }

  std::string path = args.find_path();
  if (path.empty()) { ENAN_PANIC_CODE_REL("path faild" , "use --help\n"); }
  NanJit jit;
  jit.load((char*)path.c_str(), NanJitResolved);
  jit.run();
}

int main(int argc, char** argv) {
  parse_args(argc, argv);
  return 0;
}