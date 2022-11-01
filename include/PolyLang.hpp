#ifndef POLYLANG_HPP
#define POLYLANG_HPP

#include "Compiler.hpp"
#include <string>

class PolyLang {
private:
  bool m_hadError;
  Compiler m_compiler;
  const int m_argc;
  const char** m_argv;

public:
  PolyLang(const int argc, const char** argv) 
    : m_hadError(false)
    , m_argc(argc)
    , m_argv(argv) { };

  void run();

private:
  void runPrompt();
  void execute(std::string source);
  void runFile(std::string path);
};




#endif // DEBUG
