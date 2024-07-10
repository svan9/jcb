#ifndef __NAN_JIT
#define __NAN_JIT

#include <iostream>
#include "nan.h"
#include "descryptor.cpp"
#include "lexer.cpp"

#define NanJitResolved 0b0001
#define NanJitNothing  0b0000

class NanJit {
public:
  class Signs {
  public:
    static const ubyte stdout_p = 1;
    static ubyte stdin_p;
  };

  class Argument {
  public:
    enum Kind {
      Sign, Text
    };
  private:
    Kind _kind;
    void* _content;
  public:
    Argument(Kind kind, void* content) {
      this->_kind = kind;
      this->_content = content;
    }

    template<typename T>
    static Argument from(Kind kind, T content) {
      T* object = (T*)malloc(sizeof(T));
      memcpy(object, &content, sizeof(T));
      return from<T>(kind, object);
    }

    template<typename T>
    static Argument from(Kind kind, T* content) {
      void* __patt = static_cast<void*>(content);
      Argument arg(kind, __patt);
      return arg;
    }

    Kind kind() const {
      return _kind;
    };

    void* content() const {
      return _content;
    };

    template<typename T>
    T unpack() {
      T* p = (T*)this->_content;
      return *p;
    }
  };
  
  class Token {
  public:
    enum ActionKind {
      PRINT,
      INPUT,
      WRITE,
    };
  private:
    ActionKind _kind;
    std::vector<Argument> arguments;
  public:
    Token(ActionKind kind) {
      this->_kind = kind;
    }
    Token(ActionKind kind, std::vector<Argument> arguments) {
      this->_kind = kind;
      this->arguments = arguments;
    }
    
    ActionKind kind() const {
      return this->_kind;
    }

    void add(Argument arg) {
      this->arguments.push_back(arg);
    }

    void add(Argument::Kind kind, void* content) {
      Argument arg(kind, content);
      this->arguments.push_back(arg);
    }

    template<typename T>
    void add(Argument::Kind kind, T content) {
      Argument arg = Argument::from<T>(kind, content);
      this->arguments.push_back(arg);
    }

    template<typename T>
    T at(size_t index) {
      Argument arg = this->arguments.at(index);
      return arg.unpack<T>();
    }
  };
private:
  std::vector<Token> tokens;
  void* signs[225];
public:
  NanJit() { }

  template<typename T>
  T* get_sign(ubyte sign) {
    return (T*)(this->signs[(sign-2)]);
  }

  template<typename T>
  void set_sign(ubyte sign, T& src) {
    signs[sign] = this->get_sign<T>(sign);
  }

  void append(Token& token) {
    tokens.push_back(token);
  }

  void append(Token::ActionKind kind, std::vector<Argument> content) {
    Token token(kind, content);
    tokens.push_back(token);
  }

#define jit_print_const(jit, str)\
  jit.append(NanJit::Token::ActionKind::WRITE, \
    std::vector<NanJit::Argument>({\
      NanJit::Argument::from<ubyte>(NanJit::Argument::Kind::Sign, NanJit::Signs::stdout_p), \
      NanJit::Argument::from<std::string>(NanJit::Argument::Kind::Text, std::string(str))\
    })\
  );
  // void append(Token::ActionKind kind, std::vector<Argument> content) {
  //   Token token(kind, content);
  //   tokens.push_back(token);
  // }

  void from_lex(NanLexer& lexer) {
    NanLexer::iterator it = lexer.begin();
    while (it != lexer.end()) {
      NanLexer::Unit unit = *(it++);
      if (unit.isType(NanLexer::Instruction::WRITE)) {
        ubyte sign = (ubyte)unit.text()[0];
        Token token(Token::ActionKind::WRITE);
        token.add(Argument::Kind::Sign, sign);
        unit = *(it++);
        std::string __text = unit.text().substr(0, unit.text().size());
        NanViewString text(__text);
        token.add(Argument::Kind::Text, text);
        append(token);
      }
    }
  }

  void to_lex(NanLexer& lexer) {
    for (Token token: this->tokens) {
      switch(token.kind()) {
        case (Token::ActionKind::WRITE): {
          ubyte sign = token.at<ubyte>(0);
          std::string content(1, sign);
          NanLexer::Unit unit_write(content, NanLexer::Instruction::WRITE);
          lexer.append(unit_write);

          std::string str = token.at<std::string>(1);
          NanLexer::Unit unit_text(str, NanLexer::Instruction::TEXT);
          lexer.append(unit_text);
        } break;
        default: { NAN_PANIC_CODE("unsupported token"); }
      }
    }
  }


  void run() {
    for (Token token: this->tokens) {
      switch (token.kind()) {
        case (Token::ActionKind::WRITE): {
          ubyte sign = token.at<ubyte>(0);
          NanViewString text = token.at<NanViewString>(1);
          if (sign == 1/*stdout*/) {
            fwrite(text.data(), 1, text.size(), stdout);
          }
          else if (sign == 2/*stdin*/) {
            // fputs(text.c_str(), stdin);
          } else {
            this->set_sign(sign, text);
          }
        } break;

        default: {
          NAN_PANIC_CODE("unsupported token");
        }
      };
    }
  }

  void save(char* path, int flags) {
#ifndef __linux__
    char* _path;
    if ((flags & NanJitResolved) != 0) {
      _path = abs_path(path);
    } else {
      _path = path;
    }
#else
    char* _path = path;
#endif
    NanLexer lex;
    this->to_lex(lex);
    NanDescryptor descr;
    descr.write(lex);
    NanFile file(_path, NAN_WRITE | NAN_CREATE | NAN_BINARY);
    file.open();
    file.write(descr.gets());
    file.close();
  }

  void load(char* path, int flags) {
#ifndef __linux__
    char* _path;
    if ((flags & NanJitResolved) != 0) {
      _path = abs_path(path);
    } else {
      _path = path;
    }
#else
    char* _path = path;
#endif
    NanLexer lex;
    NanDescryptor descr;
    NanFile file(_path, NAN_READ | NAN_CREATE | NAN_BINARY);
    file.open();
    descr.puts(file.read());
    file.close();
    descr.read(&lex);
    this->from_lex(lex);
  }
  
};



#endif