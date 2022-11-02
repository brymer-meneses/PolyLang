#include "AST.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
#include "Token.hpp"

#include <memory>
#include <iostream>
#include <utility>
#include <vector>

std::unique_ptr<StmtAST> Parser::parseStatement() {

  if (match({TokenType::Def}))
    return parseFunctionDefinition();

  return parseTopLevelExpr();
}

std::unique_ptr<StmtAST> Parser::parseTopLevelExpr() {
  if (auto E = parseExpression()) {
    return std::make_unique<TopLevelExprAST>(std::move(E));
  }
  return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
  auto LHS = parsePrimary();
  if (!LHS)
    return nullptr;
  return parseBinOpRHS(0, std::move(LHS));
};

std::unique_ptr<ExprAST> Parser::parsePrimary() {

  if (match({TokenType::Number}))
    return parseNumberExpr();
  
  if (match({TokenType::Identifier}))
    return parseIdentifierExpr();

  if (match({TokenType::LeftParen}))
    return parseGroupingExpr();

  std::cout << TokenTypeToString(peek().m_type) << std::endl;
  LogError("Unknown token when expecting an expression");
  return nullptr;
};

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {

  double value = std::get<0>(previous().m_value.value());
  return std::make_unique<NumberExprAST>(value);
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int exprPrec,
                                               std::unique_ptr<ExprAST> LHS) { 

  while (true) {
    int tokPrec = getTokenPrecedence();

    if (tokPrec < exprPrec)
      return LHS;

    TokenType binOp = peek().m_type;
    advance();
    auto RHS = parsePrimary();
    if (!RHS)
      return nullptr;

    int nextPrec = getTokenPrecedence();
    if (tokPrec < nextPrec) {
      RHS = parseBinOpRHS(tokPrec+1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
  }
}

std::unique_ptr<ExprAST> Parser::parseGroupingExpr() {

  auto expr = parseExpression();

  if (!expr)
    return nullptr;

  if (match({TokenType::RightParen})) {
    return std::move(expr);
  };

  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
  if (!match({TokenType::Identifier})) {
    LogError("Expected function name in prototype.");
    return nullptr;
  }

  assert(previous().m_value.has_value());
  std::string_view fnName = std::get<1>(previous().m_value.value());

  if (!match({TokenType::LeftParen})) {
    LogError("Expected '(' in prototype");
    return nullptr;
  }

  std::vector<std::string_view> args;

  if (!match({TokenType::RightParen})) {
    while(true) {
      if (match({TokenType::Identifier})) {
        std::string_view arg = std::get<1>(previous().m_value.value());
        args.push_back(std::move(arg));
      }

      if (match({TokenType::RightParen}))
        break;

      if (!match({TokenType::Comma})) {
        LogError("Expected ',' in prototype");
        return nullptr;
      };
    };
  }

  return std::make_unique<PrototypeAST>(fnName, std::move(args));
}

std::unique_ptr<StmtAST> Parser::parseFunctionDefinition() {
  std::unique_ptr<PrototypeAST> proto = parsePrototype();
  if (!proto)
    return nullptr;
  if (auto E = parseExpression()) {
    return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
  }
  return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
  std::string_view idName = std::get<1>(previous().m_value.value());

  if (!match({TokenType::LeftParen})) // Simple variable ref.
    return std::make_unique<VariableExprAST>(idName);

  std::vector<std::unique_ptr<ExprAST>> Args;
  if (!match({TokenType::RightParen})) {
    while (1) {

      if (auto Arg = parseExpression())
        Args.push_back(std::move(Arg));
      else
        return nullptr;

      if (match({TokenType::RightParen}))
        break;

      if (!match({TokenType::Comma})) {
        std::cout << TokenTypeToString(peek().m_type);
        LogError("Expected ',' in argument list");
        return nullptr;
      };

    }
  }

  return std::make_unique<CallExprAST>(idName, std::move(Args));
}

std::vector<std::unique_ptr<StmtAST>> Parser::parse() {
  std::vector<std::unique_ptr<StmtAST>> statements = {};
  while (!isFinished()) {
    auto statement =  parseStatement();
    if (statement)
      statements.push_back(std::move(statement));
    else
      break;
  }

  return std::move(statements);
}
