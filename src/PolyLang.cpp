#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <string>

#include "Compiler.hpp"
#include "PolyLang.hpp"
#include "Parser.hpp"

void PolyLang::run() {
  if (m_argc == 1) {
    runPrompt();
    return;
  };
  runFile(m_argv[1]);
};

void PolyLang::runFile(std::string path) {
  std::cout << path << std::endl;
}

void PolyLang::runPrompt() {

  std::string inputLine;

  while(true) {
    std::cout << ">>> ";
    std::getline(std::cin, inputLine);
    if (inputLine.empty())
      break;
    execute(inputLine);
  }

}

void PolyLang::execute(std::string source) {
  Parser parser = Parser(std::move(source));
  Compiler compiler = Compiler();
  auto statements = parser.parse();

  for (int i=0; i<statements.size(); i++) {
    auto* IR = statements[i].get()->accept(compiler);
    IR->print(llvm::errs());
    IR->eraseFromParent();
  }
}