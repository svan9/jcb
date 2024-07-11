#ifndef __CUBA_JIT_DESCR
#define __CUBA_JIT_DESCR

#include "nan.h"
#include "lexer.cpp"

namespace Nan {
  

class Descryptor {
private:
  std::string _content;
public:
  Descryptor() {}

  std::string write(Lexer& lexer) {
    std::stringstream builder;
    for (Lexer::Unit unit: lexer) {
      builder << unit.inttype();
      switch (unit.type()) {
        case (Lexer::Instruction::WRITE): {
          builder << unit.text()[0];
        } break;
        case (Lexer::Instruction::TEXT): {
          uint32_t length = (uint32_t)unit.text().size();
          std::string slength((char*)(&length), 4);
          builder << slength;
          builder << unit.text();
        } break;
        case (
          Lexer::Instruction::INT    ||
          Lexer::Instruction::FLOAT  ||
          Lexer::Instruction::FRAC   ||
          Lexer::Instruction::ADD    ||
          Lexer::Instruction::SUB    ||
          Lexer::Instruction::MOV    ||
          Lexer::Instruction::MOD    ||
          Lexer::Instruction::MUL    ||
          Lexer::Instruction::DIV
        ): {
          builder << unit.text();
        } break;

        default: break;
      }
    } 
    this->_content = builder.str(); 
    return this->_content;
  }

  void puts(std::string content) {
    this->_content = content;
  }

  std::string gets() const {
    return this->_content;
  }

  Lexer read() {
    Lexer lexer;
    read(&lexer);
    return lexer;
  }

  void read(Lexer* lexer) {
    std::string::iterator it = this->_content.begin();
    while (it != this->_content.end()) {
      Lexer::Instruction type = 
        integer_as_enum<Lexer::Instruction>(*(it++));

      switch (type) {
        case (Lexer::Instruction::WRITE): {
          char sign = *(it++);
          std::string str(1, sign);
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
        } break;

        case (Lexer::Instruction::TEXT): {
          char* ptr;

          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          uint32_t size;
          memcpy(&size, ptr, sizeof(size));
          it+=sizeof(size);
          
          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          std::string str(ptr, 0U, size);
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
          it+=size;
        } break;

        case (Lexer::Instruction::INT): {
          char* ptr;
          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          std::string str(ptr, 0U, sizeof(int));
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
          it+=sizeof(int);
        } break;

        case (Lexer::Instruction::FLOAT): {
          char* ptr;
          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          std::string str(ptr, 0U, sizeof(float));
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
          it+=sizeof(float);
        } break;
        
        case (Lexer::Instruction::FRAC): {
          char* ptr;
          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          std::string str(ptr, 0U, sizeof(IFrac));
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
          it+=sizeof(IFrac);
        } break;

        case (
          Lexer::Instruction::ADD ||
          Lexer::Instruction::SUB ||
          Lexer::Instruction::MOV ||
          Lexer::Instruction::MOD ||
          Lexer::Instruction::MUL ||
          Lexer::Instruction::DIV
        ): {
          char fsign = *(it++);
          char ssign = *(it++);
          std::string str;
          str += fsign;
          str += ssign;
          Lexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
        } break;

        default: break;
      }
    }
  }

};

}

#endif