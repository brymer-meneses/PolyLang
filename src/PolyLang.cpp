#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <string>

#include "Compiler.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
#include "PolyLang.hpp"

void PolyLang::run() {
  if (m_argc == 1) {
    runPrompt();
    return;
  };
  runFile(m_argv[1]);
};

void PolyLang::runFile(std::string_view path) {
  std::cout << path << std::endl;
}

void PolyLang::runPrompt() {

  std::string inputLine;

  while (true) {
    std::cout << ">>> ";
    std::getline(std::cin, inputLine);
    if (inputLine.empty())
      break;
    execute(inputLine);
  }

  m_compiler.m_module->print(llvm::errs(), nullptr);
}

void PolyLang::execute(std::string_view source) {
  Parser parser = Parser(source);
  auto statements = parser.parse();

  for (int i = 0; i < statements.size(); i++) {
    if (!statements[i]) {
      LogError("Parsing Error.");
      break;
    }

    auto *IR = m_compiler.codegen(statements[i].get());

    if (!IR) {
      LogError("Compilation Error.");
      break;
    }
    IR->print(llvm::errs());

    if (statements[i]->type() == AstType::ExpressionStmt)
      static_cast<llvm::Function *>(IR)->eraseFromParent();
  }
}
