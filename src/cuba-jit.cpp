#include "nan.h"
#include "jcb.h"

static const char* test_file = "./temp.co"; 

using namespace Nan;

void cj_write() {
  Jit jit;
  jit.append(Jit::Token::ActionKind::WRITE, 
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p), 
      Jit::Argument::from<Nan::ViewString>(Jit::Argument::Kind::Text, Nan::ViewString("hellow, word\n"))
    })
  );
  jit.append(Jit::Token::ActionKind::WRITE, 
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p), 
      Jit::Argument::from<Nan::ViewString>(Jit::Argument::Kind::Text, Nan::ViewString("hellow, wordx2\n"))
    })
  );
  jit.append(Jit::Token::ActionKind::WRITE,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p),
      Jit::Argument::from<Nan::ViewString>(Jit::Argument::Kind::Text, Nan::ViewString("hellow, word twice\n")) 
    })
  );
  jit.append(Jit::Token::ActionKind::WRITE,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 4),
      Jit::Argument::from<int>(Jit::Argument::Kind::Int, 12) 
    })
  );
  jit.append(Jit::Token::ActionKind::WRITE,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 5),
      Jit::Argument::from<int>(Jit::Argument::Kind::Int, 16) 
    })
  );
  jit.append(Jit::Token::ActionKind::ADD,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 4),
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 5),
    })
  );
  jit.append(Jit::Token::ActionKind::ITOS,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 6),
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 4),
    })
  );
  jit.append(Jit::Token::ActionKind::WRITE,
    std::vector<Jit::Argument>({
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p),
      Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, 6),
    })
  );
  jit.save((char*)test_file, NanJitResolved);
}

void cj_read() {
  Jit jit;
  jit.load((char*)test_file, NanJitResolved);
  jit.run();
}

void parse_args(int argc, char** argv) {
  Nan::ArgumentsRow args;
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
    Jit jit;
    jit.append(Jit::Token::ActionKind::WRITE, 
      std::vector<Jit::Argument>({
        Jit::Argument::from<Nan::ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p), 
        Jit::Argument::from<Nan::ViewString>(Jit::Argument::Kind::Text, Nan::ViewString(temp))
      })
    );
    jit.save((char*)test_file, NanJitResolved);
    exit(0);
  }

  std::string path = args.find_path();
  if (path.empty()) { ENAN_PANIC_CODE_REL("path faild" , "use --help\n"); }
  Jit jit;
  jit.load((char*)path.c_str(), NanJitResolved);
  jit.run();
}

int main(int argc, char** argv) {
  parse_args(argc, argv);
  return 0;
}