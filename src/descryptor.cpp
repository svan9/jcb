#ifndef __CUBA_JIT_DESCR
#define __CUBA_JIT_DESCR

#include "nan.h"
#include "lexer.cpp"

class NanDescryptor {
private:
  std::string _content;
public:
  NanDescryptor() {}

  std::string write(NanLexer& lexer) {
    std::stringstream builder;
    for (NanLexer::Unit unit: lexer) {
      builder << unit.inttype();
      switch (unit.type()) {
        case (NanLexer::Instruction::WRITE): {
          builder << unit.text()[0];
        } break;
        case (NanLexer::Instruction::TEXT): {
          uint32_t length = (uint32_t)unit.text().size();
          std::string slength((char*)(&length), 4);
          builder << slength;
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

  NanLexer read() {
    NanLexer lexer;
    read(&lexer);
    return lexer;
  }

  void read(NanLexer* lexer) {
    std::string::iterator it = this->_content.begin();
    while (it != this->_content.end()) {
      NanLexer::Instruction type = 
        integer_as_enum<NanLexer::Instruction>(*(it++));

      switch (type) {
        case (NanLexer::Instruction::WRITE): {
          char sign = *(it++);
          std::string str(1, sign);
          NanLexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
        } break;

        case (NanLexer::Instruction::TEXT): {
          char* ptr;

          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          uint32_t size;
          memcpy(&size, ptr, sizeof(size));
          it+=sizeof(size);
          
          ptr = &this->_content[std::distance(this->_content.begin(), it)];
          std::string str(ptr, 0U, size);
          NanLexer::Unit unit(str, enum_as_integer(type));
          lexer->append(unit);
          it+=size;
        } break;

        default: break;
      }
    }
  }

};



#endif