#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

#include <memory>
#include <vector>
#include <list>


class Parser {
private:
  std::vector<Token> m_tokens;
  std::size_t m_current;

public:
  Parser(const std::vector<Token> tokens) 
      : m_tokens(tokens), 
        m_current(0) { };
  Parser(const std::string source) {
    Lexer lexer = Lexer(std::move(source));
    m_tokens = lexer.scanTokens();
    m_current = 0;
  };
  
  std::vector<std::unique_ptr<StmtAST>> parse();

private:
  std::unique_ptr<ExprAST> parseExpression();
  std::unique_ptr<ExprAST> parsePrimary();
  std::unique_ptr<ExprAST> parseNumberExpr();
  std::unique_ptr<ExprAST> parseGroupingExpr();
  std::unique_ptr<ExprAST> parseBinaryExpr();
  std::unique_ptr<ExprAST> parseVariableExpr();

  std::unique_ptr<StmtAST> parseStatement();
  std::unique_ptr<PrototypeAST> parsePrototype();
  std::unique_ptr<StmtAST> parseTopLevelExpression();
  std::unique_ptr<StmtAST> parseFunctionDefinition();

  std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS);

  int getTokenPrecedence() {
    switch (peek().m_type) {
      case TokenType::Lesser:
      case TokenType::Greater:
      case TokenType::LesserEqual:
      case TokenType::GreaterEqual:
        return 10;
      case TokenType::Plus:
      case TokenType::Minus:
        return 20;
      case TokenType::Star:
        return 40;
      default:
        return -1;
    }
  }

  bool match(const std::list<TokenType> tokenTypes) {
    for (auto token : tokenTypes) {
      if (check(token)) {
        advance();
        return true;
      }
    }
    return false;
  }

  bool check(TokenType tok) {
    if (peek().m_type == tok)
      return true;
    return false;
  }

  Token& advance() {
    m_current += 1;
    return m_tokens[m_current - 1];
  }

  Token& previous() {
    return m_tokens[m_current - 1];
  }

  Token& peek() {
    return m_tokens[m_current];
  }

  Token& peekNext() {
    return m_tokens[m_current + 1];
  }

  bool isFinished() {
    return m_tokens[m_current].m_type == TokenType::Eof;
  }

};

#endif // !PARSER_HPP
