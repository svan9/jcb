#ifndef __NAN_H
#define __NAN_H

#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#ifdef _WIN32
  #include <io.h>
#elif defined(__linux__) 
  #include <fcntl.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif
#include <cstdlib>
#include <stdlib.h>
#include <sstream>

#define GETCODELINE() __LINE__
#define GETCODEFILE() __FILE__
#define GETCODEFUNC() __func__
#define stringify(a) #a
#define callpoint() printf("called from: %s() at %s:%i \n", GETCODEFUNC(), GETCODEFILE(), GETCODELINE());
#define callpointf(func) printf("called from: %s() at %s:%i \n", func, GETCODEFILE(), GETCODELINE());
#define ppp callpoint()
#define pppp {callpointf(GETCODEFUNC())}
#define pppf {callpointf(func)}

#undef _MAX_PATH
#ifndef _CRTIMP
#define _CRTIMP
#endif
#ifndef __MINGW_NOTHROW
#define __MINGW_NOTHROW
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifdef _WIN32
_CRTIMP __cdecl __MINGW_NOTHROW  char *_fullpath (char*, const char*, size_t);
#endif
#define _MAX_PATH (260)

#ifndef ltrim
char *ltrim(char *s) {
  while(isspace(*s)) s++;
  return s;
}

char *rtrim(char *s) {
  char* back = s + strlen(s);
  while(isspace(*--back));
  *(back+1) = '\0';
  return s;
}
char *trim(char *s) {
  return rtrim(ltrim(s));
}
#endif

#define NAN_WARN_CODE(message) WENAN_WARN_CODE(GETCODELINE(), GETCODEFILE(), __func__ , message)
#define NAN_PANIC_CODE(message) WEENAN_PANIC_CODE(GETCODELINE(), GETCODEFILE(), __func__ , "error", message)
#define NAN_PANIC_CODE_REL(message) WEENAN_PANIC_CODE_REL(GETCODELINE(), GETCODEFILE(), __func__ , "error", message)
#define ENAN_PANIC_CODE_REL(kind, message) WEENAN_PANIC_CODE_REL(GETCODELINE(), GETCODEFILE(), __func__ , kind, message)
#define ENAN_PANIC_CODE(kind, message) WEENAN_PANIC_CODE(GETCODELINE(), GETCODEFILE(), __func__ , kind, message)
#define NAN_PANIC_CODE_FUNC(message, func) WENAN_PANIC_CODE(GETCODELINE(), GETCODEFILE(), func , message)
#define SNAN_PANIC_CODE(message) AENAN_PANIC_CODE(GETCODELINE(), GETCODEFILE(), __func__ , message)
#define NAN_PANIC_CODEF(...) \
  WENAN_PANIC_CODE(GETCODELINE(), GETCODEFILE(), __func__ , string_format(__VA_ARGS__))


char* READ_LINE_FROM_FILE(int line, const char* file) {
  const int MAX_LINE_LENGTH = 1000;
  FILE * fp;
  fp = fopen(file, "r");
  char* chunk = (char*)malloc(MAX_LINE_LENGTH);
  int nut = 0;
  while(fgets(chunk, MAX_LINE_LENGTH, fp) != NULL) {
    if ((nut++) >= (line-1)) { break; }
  }
  fclose(fp);
  return chunk;
}

void WNAN_PANIC_CODE(int line, const char* path, const char* message) {
  char* _line;
  _line = trim(READ_LINE_FROM_FILE(line, path));
  fprintf(stderr, "error at %s:%i (%s)", path, line, message);
  fprintf(stderr, "\n\t%s", _line);
  fprintf(stderr, "\n\t^");
  exit(1);
}

void WENAN_PANIC_CODE(int line, const char* path, const char* func, const char* message) {
  char* _line;
  _line = trim(READ_LINE_FROM_FILE(line, path));

  fprintf(stderr, "error in function `%s()` at %s:%i (%s)", func, path, line, message);
  fprintf(stderr, "\n\t%s", _line);
  fprintf(stderr, "\n\t^\n");
  exit(1);
}

void WEENAN_PANIC_CODE_REL(int line, const char* path, const char* func, const char* error_kind, const char* message) {
  fprintf(stderr, "%s: %s", error_kind, message);
  exit(1);
}
void WEENAN_PANIC_CODE(int line, const char* path, const char* func, const char* error_kind, const char* message) {
  char* _line;
  _line = trim(READ_LINE_FROM_FILE(line, path));

  fprintf(stderr, "%s in function `%s()` at %s:%i (%s)", error_kind, func, path, line, message);
  fprintf(stderr, "\n\t%s", _line);
  fprintf(stderr, "\n\t^\n");
  exit(1);
}

void WENAN_WARN_CODE(int line, const char* path, const char* func, const char* message) {
  char* _line;
  _line = trim(READ_LINE_FROM_FILE(line, path));

  fprintf(stderr, "warn in function `%s()` at %s:%i (%s)", func, path, line, message);
  fprintf(stderr, "\n\t%s", _line);
  fprintf(stderr, "\n\t^");
}

void AENAN_PANIC_CODE(int line, const char* path, const char* func, const char* message) {
  fprintf(stderr, "error in function `%s()` at %s:%i (%s)\n", func, path, line, message);
  exit(1);
}


typedef const char* view_cstr;

class NanViewString {
private:
  char* _data;
  size_t _size;
public:
  NanViewString(std::string str) {
    // str 
    this->_data = (char*)malloc(str.size());
    memcpy(this->_data, str.data(), str.size());
    // this->_data, str.size()
    this->_size = str.size();
  }

  const char* data() const noexcept {
    return this->_data;
  }

  size_t size() const noexcept {
    return this->_size;
  }
};

typedef unsigned char ubyte;

bool isFilePath(const char* path) {
	return access(path, F_OK) == 0;
}

#define NAN_READ   0b0001
#define NAN_WRITE  0b0010
#define NAN_BINARY 0b0100
#define NAN_CREATE 0b1000

class NanFile {
private:
  FILE* ptr;
  size_t size;
  uintptr_t flags;
  const char* path;
public:
  NanFile(const char* path, uintptr_t flags) {
    if (!isFilePath(path) && !(flags & NAN_CREATE)) { 
      printf("'%s'\n", path);
      ENAN_PANIC_CODE("file error", "undefined file path");
    }
    this->ptr = NULL;
    this->path = path;
    this->flags = flags;
    this->size = (size_t)-1;
  }
  
  size_t seek() {
    long file_size;
    fseek(this->ptr, 0L, SEEK_END);
    file_size = ftell(this->ptr);
    fseek(this->ptr, 0L, SEEK_SET);
    this->size = file_size;
    return this->size;
  }

  static NanFile open(const char* path, uintptr_t flags) {
    NanFile file (path, flags);
    file.open();
    return file;
  }

  void open() {
    char fstr[4] = {0};
    if (this->flags & NAN_READ) {
      fstr[0] = 'r';
    }
    else if (this->flags & NAN_WRITE) {
      fstr[0] = 'w';
    }
    if (this->flags & NAN_BINARY) {
      fstr[1] = 'b';
    }
    if (this->flags & NAN_CREATE) {
      if (fstr[1] != 0) {
        fstr[2] = '+';
      } else {
        fstr[1] = '+';
      }
    }
    if (fstr[0] == 0) { ENAN_PANIC_CODE("file error", "undefined flags"); }
    this->ptr = fopen(path, fstr);
    if (this->ptr == NULL || ferror(this->ptr)) { 
      printf("'%s' ''%s \n", path, fstr);
      throw std::runtime_error("file error");
    }
    this->seek();
  }

  void close() {
    fclose(this->ptr);
  }

  std::string read() {
    if (!(this->flags | NAN_READ)) { NAN_PANIC_CODE("FILE ERROR"); }
    char buffer[this->size];
    fread(buffer, sizeof(char), this->size, this->ptr);
    std::string str(buffer, this->size);
    return str;
  }

  size_t write(std::string text) {
    if (!(this->flags | NAN_WRITE)) { NAN_PANIC_CODE("FILE ERROR"); }
    return fwrite(text.c_str(), sizeof(char), text.size(), this->ptr);
  }
  
};


template <typename E>
auto enum_as_integer(E const value) -> typename std::underlying_type<E>::type {
  return static_cast<typename std::underlying_type<E>::type>(value);
};
template <typename E>
E integer_as_enum(int value) {
  return static_cast<E>(value);
};

char* abs_path(char* path) {
#ifdef __linux__
  char* buffer = (char*)malloc(_MAX_PATH);
  readlink(path, buffer, _MAX_PATH);
  return buffer;
#elif _WIN32
  char* buffer = (char*)malloc(_MAX_PATH);
  if(_fullpath(buffer, path, _MAX_PATH) == NULL) {
    NAN_PANIC_CODE("path is uncorect");
  }
  return buffer;
#endif
}


class NanArgumentsRow {
public:
  class Argument {
  public:
    std::string flag;
    size_t value_i;
  } NanArgument;
private:
  std::vector<std::string> content;
  std::vector<Argument> _with_flags;
public:
  NanArgumentsRow() {}

#define string_starts_with(str, str2) (str.compare(0, strlen(str2), str2) == 0)

  void parse(int argc, char** argv) {
    int cursor = 1;
    while (cursor < argc) {
      std::string arg(argv[cursor++]);
      if (
        string_starts_with(arg, "-") ||
        string_starts_with(arg, "--")
      ) {
        Argument __arg;
        __arg.flag = arg;
        if (cursor+1 < argc) {
          __arg.value_i = this->content.size();
        } else {
          __arg.value_i = (size_t)-1;
        }
        this->_with_flags.push_back(__arg);
      }
      else {
        this->content.push_back(arg);
      }
    }
  }

  bool includes(const char* flag) {
    std::string _flag(flag);
    for (auto it = this->_with_flags.begin(); 
      it != this->_with_flags.end(); it++) {
      Argument arg = *it;
      if (arg.flag == _flag) { return true; }
    }
    return false;
  }

  bool includes(const char* flag, std::string& out_value) {
    std::string _flag(flag);
    for (auto it = this->_with_flags.begin(); 
      it != this->_with_flags.end(); it++) {
      Argument arg = *it;
      if (arg.flag == _flag) { 
        arg = *(++it);
        out_value = this->content.at(arg.value_i);
        return true;
      }
    }
    return false;
  }

  // std::string at(size_t index) {
  //   std::string str = this->content.at()
  // }

  std::string find_path() {
    for (auto it = this->content.begin(); 
      it != this->content.end(); it++) {
      std::string arg = *it;
      if (isFilePath(arg.c_str())) {
        return arg;
      }
    }
    return std::string();
  }
};

static const char* NanCharConsoleClear  = "\033[00m";
static const char* NanCharConsoleCyanFG = "\033[96m";
static const char* NanCharConsoleGrayFG = "\033[02m";

#define NanConsoleRC(a) a, NanCharConsoleClear

#endif