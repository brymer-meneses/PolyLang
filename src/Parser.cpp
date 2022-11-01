#include "AST.hpp"
#include "Logger.hpp"
#include "Parser.hpp"

#include <memory>
#include <utility>
#include <vector>

std::unique_ptr<StmtAST> Parser::parseStatement() {

  if (match({TokenType::Def}))
    return parseFunctionDefinition();

  return parseTopLevelExpression();
}

std::unique_ptr<StmtAST> Parser::parseTopLevelExpression() {
  if (auto E = parseExpression()) {
    auto proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
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
    return parseVariableExpr();

  if (match({TokenType::LeftParen}))
    return parseGroupingExpr();

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
  if (!check(TokenType::Identifier)) {
    LogError("Expected function name in prototype.");
    return nullptr;
  }

  std::string fnName = std::get<1>(peek().m_value.value());

  if (!check(TokenType::LeftParen)) {
    LogError("Expected '(' in prototype");
    return nullptr;
  }

  std::vector<std::string> argNames;
  while (match({TokenType::Identifier})) {
    std::string arg = std::get<1>(previous().m_value.value());
    argNames.push_back(std::move(arg));
  };

  if (!match({TokenType::RightParen})) {
    LogError("Expected ')' in prototype");
    return nullptr;
  }

  return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<StmtAST> Parser::parseFunctionDefinition() {
  std::unique_ptr<PrototypeAST> proto = parsePrototype();
  if (!proto)
    return nullptr;
  if (auto E = parseExpression()) {
    auto e = std::make_unique<FunctionAST>(std::move(proto), std::move(E));
  }
  return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseVariableExpr() {
  std::string idName = std::get<1>(previous().m_value.value());

  advance();  // eat identifier.

  if (!check(TokenType::LeftParen)) // Simple variable ref.
    return std::make_unique<VariableExprAST>(idName);

  // Call.
  advance();
  std::vector<std::unique_ptr<ExprAST>> Args;
  if (!check(TokenType::RightParen)) {
    while (1) {
      if (auto Arg = parseExpression())
        Args.push_back(std::move(Arg));
      else
        return nullptr;

      if (!check(TokenType::RightParen))
        break;

      if (!check(TokenType::Comma)) {
        LogError("Expected ')' or ',' in argument list");
        return nullptr;
      };
      advance();
    }
  }

  // Eat the ')'.
  advance();
  return std::make_unique<CallExprAST>(idName, std::move(Args));
}

std::vector<std::unique_ptr<StmtAST>> Parser::parse() {
  std::vector<std::unique_ptr<StmtAST>> statements = {};
  while (!isFinished()) {
    auto statement =  parseStatement();
    if (statement)
      statements.push_back(parseStatement());
    else
      break;
  }

  return std::move(statements);
}
