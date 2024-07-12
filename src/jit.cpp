#ifndef __NAN_JIT
#define __NAN_JIT

#include <iostream>
#include "nan.h"
#include "descryptor.cpp"
#include "lexer.cpp"

namespace Nan {

#define NanJitResolved 0b0001
#define NanJitNothing  0b0000

class Jit {
public:
  class Signs {
  public:
    static const ubyte stdout_p = 1;
    static ubyte stdin_p;
  };

  class Argument {
  public:
    enum Kind {
      Sign, 
      Text, Int, Float, Frac,
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
      INPUT,
      WRITE,
      ADD, SUB, DIV, MUL, MOD,
      DIVF,
      MOV,
      ITOS,
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

    Nan::Jit::Argument::Kind at_kind(size_t index) {
      Argument arg = this->arguments.at(index);
      return arg.kind();
    }

  };
private:
  std::vector<Token> tokens;
  void* signs[225];
public:
  Jit() {}

  template<typename T>
  T* get_sign(ubyte sign) {
    if ((sign - 2) < 0) {
      throw std::runtime_error("sign must be in range (3-255)");
    }
    return (T*)(this->signs[(sign-2)]);
  }

  template<typename T>
  T get_signn(ubyte sign) {
    if ((sign - 2) < 0) {
      throw std::runtime_error("sign must be in range (3-255)");
    }
    return *(T*)(this->signs[(sign-2)]);
  }

  void swap_sign(ubyte lsign, ubyte rsign) {
    void* l = this->signs[(lsign-2)]; 
    void* r = this->signs[(rsign-2)];
    this->signs[(lsign-2)] = r;
    this->signs[(rsign-2)] = l;
  }

  template<typename T>
  void set_sign(ubyte sign, T& src) {
    if ((sign - 2) < 0) {
      throw std::runtime_error("sign must be in range (3-255)");
    }

    this->signs[(sign-2)] = &src;
    // T* p = this->get_sign<T>(sign);
    // memcpy(p, &src, sizeof(T));
  }

  void append(Token& token) {
    tokens.push_back(token);
  }

  void append(Token::ActionKind kind, std::vector<Argument> content) {
    Token token(kind, content);
    tokens.push_back(token);
  }

#define jit_print_const(jit, str)\
  jit.append(Jit::Token::ActionKind::WRITE, \
    std::vector<Jit::Argument>({\
      Jit::Argument::from<ubyte>(Jit::Argument::Kind::Sign, Jit::Signs::stdout_p), \
      Jit::Argument::from<std::string>(Jit::Argument::Kind::Text, std::string(str))\
    })\
  );

  void from_lex(Lexer& lexer) {
    Lexer::iterator it = lexer.begin();
    while (it != lexer.end()) {
      Lexer::Unit unit = *(it++);
      Lexer::Instruction type = unit.type();
      switch (type) {
        case (Lexer::Instruction::WRITE): {
          ubyte sign = (ubyte)unit.text()[0];
          Token token(Token::ActionKind::WRITE);
          token.add(Argument::Kind::Sign, sign);
          unit = *(it++);
          
          switch (unit.type()) {
            case (Lexer::Instruction::TEXT): {
              std::string __text = unit.text().substr(0, unit.text().size());
              ViewString text(__text);
              token.add(Argument::Kind::Text, text);
            } break;

            case (Lexer::Instruction::SIGN): {
              std::string __text = unit.text().substr(0, unit.text().size());
              ubyte sign = (ubyte)__text.data()[0];
              token.add(Argument::Kind::Sign, sign);
            } break;

            case (Lexer::Instruction::INT): {
              std::string __text = unit.text().substr(0, sizeof(int));
              int number;
              memcpy(&number, __text.data(), sizeof(int));
              token.add(Argument::Kind::Int, number);
            } break;

            case (Lexer::Instruction::FLOAT): {
              std::string __text = unit.text().substr(0, sizeof(float));
              float number;
              memcpy(&number, __text.data(), sizeof(int));
              token.add(Argument::Kind::Float, number);
            } break;

            case (Lexer::Instruction::FRAC): {
              std::string __text = unit.text().substr(0, sizeof(IFrac));
              IFrac number(__text);
              token.add(Argument::Kind::Frac, number);
            } break;
          
            default: break;
          }
          
          append(token);
        } break;
        
        case (Lexer::Instruction::ITOS): {
          Token token(Token::ActionKind::ITOS);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;

        case (Lexer::Instruction::MOV): {
          Token token(Token::ActionKind::MOV);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;
        
        case (Lexer::Instruction::ADD): {
          Token token(Token::ActionKind::ADD);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;
        
        case (Lexer::Instruction::SUB): {
          Token token(Token::ActionKind::SUB);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;

        case (Lexer::Instruction::MUL): {
          Token token(Token::ActionKind::MUL);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;

        case (Lexer::Instruction::DIV): {
          Token token(Token::ActionKind::DIV);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;

        case (Lexer::Instruction::MOD): {
          Token token(Token::ActionKind::MOD);
          ubyte fsign = (ubyte)unit.text()[0];
          token.add(Argument::Kind::Sign, fsign);
          ubyte ssign = (ubyte)unit.text()[1];
          token.add(Argument::Kind::Sign, ssign);
          append(token);
        } break;

        default: break;
      }
    }
  }

  void to_lex(Lexer& lexer) {
    for (Token token: this->tokens) {
      switch(token.kind()) {
        case (Token::ActionKind::WRITE): {
          ubyte sign = token.at<ubyte>(0);
          std::string content(1, sign);
          Lexer::Unit unit_write(content, Lexer::Instruction::WRITE);
          lexer.append(unit_write);
          Argument::Kind type = token.at_kind(1); 
          switch(type) {
            case (Argument::Kind::Text): {
              ViewString str = token.at<ViewString>(1);
              Lexer::Unit unit_text(str.toString(), Lexer::Instruction::TEXT);
              lexer.append(unit_text);
            } break;

            case (Argument::Kind::Int): {
              int number = token.at<int>(1);
              char buffer[sizeof(int)];
              memcpy(buffer, &number, sizeof(int));
              Lexer::Unit unit_text(std::string(buffer, sizeof(int)), Lexer::Instruction::INT);
              lexer.append(unit_text);
            } break;

            case (Argument::Kind::Sign): {
              ubyte number = token.at<ubyte>(1);
              char buffer[sizeof(ubyte)];
              memcpy(buffer, &number, sizeof(ubyte));
              Lexer::Unit unit_text(std::string(buffer, sizeof(ubyte)), Lexer::Instruction::SIGN);
              lexer.append(unit_text);
            } break;

            case (Argument::Kind::Float): {
              float number = token.at<float>(1);
              char buffer[sizeof(float)];
              memcpy(buffer, &number, sizeof(float));
              Lexer::Unit unit_text(std::string(buffer, sizeof(float)), Lexer::Instruction::FLOAT);
              lexer.append(unit_text);
            } break;

            case (Argument::Kind::Frac): {
              IFrac number = token.at<IFrac>(1);
              Lexer::Unit unit_text(number.data(), Lexer::Instruction::FRAC);
              lexer.append(unit_text);
            } break;

            default: break;
          }
        } break;

        case (Token::ActionKind::ITOS): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::ITOS);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::MOV): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::MOV);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::ADD): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::ADD);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::SUB): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::SUB);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::MUL): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::MUL);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::DIV): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::DIV);
          lexer.append(unit);
        } break;

        case (Token::ActionKind::MOD): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          std::string content;
          content += fsign;
          content += ssign;
          Lexer::Unit unit(content, Lexer::Instruction::MOD);
          lexer.append(unit);
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
          Argument::Kind second_kind = token.at_kind(1);
          switch (second_kind) {
            case (Argument::Kind::Text): {
              ViewString text = token.at<ViewString>(1);
              if (sign == 1/*stdout*/) { fwrite(text.data(), 1, text.size(), stdout); }
              else if (sign == 2/*stdin*/) { } 
              else { this->set_sign(sign, text); }
            } break;
            
            case (Argument::Kind::Sign): {
              ubyte ssign = token.at<ubyte>(1);
              ViewString text = get_signn<ViewString>(ssign);
              if (sign == 1/*stdout*/) { fwrite(text.data(), 1, text.size(), stdout); }
              else if (sign == 2/*stdin*/) { } 
              else { this->set_sign(sign, text); }
            } break;
            
            case (Argument::Kind::Int): {
              int number = token.at<int>(1);
              if (sign == 1/*stdout*/) {
                std::string str = std::to_string(number);
                fwrite(str.data(), 1, str.size(), stdout);
              }
              else if (sign == 2/*stdin*/) { } 
              else { this->set_sign(sign, number); }
            } break;

            case (Argument::Kind::Float): {
              float number = token.at<float>(1);
              if (sign == 1/*stdout*/) {
                std::string str = std::to_string(number);
                fwrite(str.data(), 1, str.size(), stdout);
              }
              else if (sign == 2/*stdin*/) { } 
              else { this->set_sign(sign, number); }
            } break;
          
            default: break;
          }
        } break;

        case (Token::ActionKind::ADD): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(fsign) + get_signn<int>(ssign);
          set_sign<int>(fsign, x);
        } break;

        case (Token::ActionKind::SUB): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(fsign) - get_signn<int>(ssign);
          set_sign<int>(fsign, x);
        } break;

        case (Token::ActionKind::DIV): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(fsign) / get_signn<int>(ssign);
          set_sign<int>(fsign, x);
        } break;
        
        case (Token::ActionKind::MUL): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(fsign) * get_signn<int>(ssign);
          set_sign<int>(fsign, x);
        } break;

        case (Token::ActionKind::MOD): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(fsign) % get_signn<int>(ssign);
          set_sign<int>(fsign, x);
        } break;

        case (Token::ActionKind::ITOS): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          int x = get_signn<int>(ssign);
          ViewString str(std::to_string(x));
          set_sign<ViewString>(fsign, str);
        } break;

        case (Token::ActionKind::MOV): {
          ubyte fsign = token.at<ubyte>(0);
          ubyte ssign = token.at<ubyte>(1);
          // if (fsign == 1/*stdout*/) {
          //   ViewString str = get_signn<ViewString>(fsign);
          // } else {
          int x = get_signn<int>(fsign);
          set_sign<int>(ssign, x);
          // }
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
    Lexer lex;
    this->to_lex(lex);
    Descryptor descr;
    descr.write(lex);
    File file(_path, NAN_WRITE | NAN_CREATE | NAN_BINARY);
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
    Lexer lex;
    Descryptor descr;
    File file(_path, NAN_READ | NAN_CREATE | NAN_BINARY);
    file.open();
    descr.puts(file.read());
    file.close();
    descr.read(&lex);
    this->from_lex(lex);
  }
  
};

}



#endif