#ifndef __CUBA_JIT_LEXER
#define __CUBA_JIT_LEXER

#include "nan.h"

namespace Nan {


class Lexer {
public:
  enum Instruction {
    TEXT     = 2,
    WRITE    = 3,
    INPUT    = 4,
    // VAR      = 5,
    // WRITEex  = 6,
    // VARex    = 7,
    INT      = 8,
    FLOAT    = 9,
    FRAC     = 10,
    ADD      = 11,
    SUB      = 12,
    DIV      = 13,
    MUL      = 14,
    MOD      = 15,
    MOV      = 16,
    // MOVL     = 17,
    // LMOV     = 18,
    // LMOVL    = 19,
  };

  class Unit {
  private:
    std::string _content;
    ubyte _type;
  public:
    Unit(std::string content, ubyte type) {
      this->_content = content;
      this->_type = type;
    }
    Unit(std::string content, Instruction type) {
      this->_content = content;
      this->_type = (ubyte)enum_as_integer(type);
    }
    bool isType(Instruction instr) {
      return this->_type == enum_as_integer(instr);
    }

    std::string text() const {
      return this->_content;
    }

    Instruction type() {
      return static_cast<Instruction>(this->_type);
    }

    ubyte inttype() {
      return this->_type;
    }

  };

private:
  std::vector<Unit> tokens;
  size_t cursor;
public:
  typedef typename std::vector<Unit>::iterator iterator;
  typedef typename std::vector<Unit>::const_iterator const_iterator;
  iterator begin() {return tokens.begin();}
  const_iterator begin() const {return tokens.begin();}
  const_iterator cbegin() const {return tokens.cbegin();}
  iterator end() {return tokens.end();}
  const_iterator end() const {return tokens.end();}
  const_iterator cend() const {return tokens.cend();}

  Lexer() {
    this->cursor = 0;
  }

  void append(Unit unit) {
    this->tokens.push_back(unit);
  }

  Unit next() {
    return this->tokens.at(this->cursor++);
  }
};

}

#endif