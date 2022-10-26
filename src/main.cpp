#include <iostream>
#include <lexer.hpp>

int main() {
  Lexer lexer(std::string("#lolo lolo dafa fsa \n \"the quick brown fox\""));
  auto tokens = lexer.getTokens();
  std::cout << tokens.size() << std::endl;
  for (auto &token : tokens) {
    printf("%s ", token->lexeme.c_str());
  }
  return 0;
}
