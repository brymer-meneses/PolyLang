#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

#include <list>
#include <memory>
#include <vector>

class Parser {
private:
  std::vector<Token> m_tokens;
  std::size_t m_current = 0;
  bool hadParsingError = false;

public:
  Parser(const std::vector<Token> tokens) : m_tokens(tokens){};
  Parser(const std::string_view source) {
    Lexer lexer = Lexer(source);
    m_tokens = lexer.scanTokens();
  };

  std::vector<std::unique_ptr<Stmt>> parse();

private:
  std::unique_ptr<Expr> parseExpression();
  std::unique_ptr<Expr> parsePrimary();
  std::unique_ptr<Expr> parseNumberExpr();
  std::unique_ptr<Expr> parseGroupingExpr();
  std::unique_ptr<Expr> parseBinaryExpr();
  std::unique_ptr<Expr> parseIdentifierExpr();

  std::unique_ptr<Stmt> parseStatement();
  std::unique_ptr<Stmt> parseExpressionStmt();
  std::unique_ptr<Stmt> parseFunctionDefinition();
  std::unique_ptr<Stmt> parseIfStmt();

  std::unique_ptr<BlockStmt> parseBlock();
  std::unique_ptr<ReturnStmt> parseReturn();
  std::unique_ptr<PrototypeStmt> parsePrototype();

  std::unique_ptr<Expr> parseBinOpRHS(int exprPrec, std::unique_ptr<Expr> LHS);

  int getTokenPrecedence() {
    switch (peek().m_type) {
    case TokenType::Lesser:
    case TokenType::Greater:
    case TokenType::LesserEqual:
    case TokenType::GreaterEqual:
    case TokenType::EqualEqual:
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

  bool match(TokenType tokenType) {
    if (check(tokenType)) {
      advance();
      return true;
    }

    return false;
  }

  bool check(TokenType tok) {
    if (peek().m_type == tok)
      return true;
    return false;
  }

  Token &advance() {
    m_current += 1;
    return m_tokens[m_current - 1];
  }

  Token &previous() { return m_tokens[m_current - 1]; }

  Token &peek() { return m_tokens[m_current]; }

  Token &peekNext() { return m_tokens[m_current + 1]; }

  bool isFinished() { return m_tokens[m_current].m_type == TokenType::Eof; }
};

#endif // !PARSER_HPP
