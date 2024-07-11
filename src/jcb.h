#ifndef __JCB__
#define __JCB__

#include <iostream>
#include <string>
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

#include "nan.h"
#include "lexer.cpp"
#include "descryptor.cpp"
#include "jit.cpp"

namespace Nan {

  // using ::ubyte;
  // using ::Lexer;
  // using ::Jit;
  // using ::Descryptor;
  // using ::ViewString;
  // using ::ArgumentsRow;
}

#endif