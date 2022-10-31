#ifndef AST_HPP
#define AST_HPP

#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

enum class ASTType {
  NumberExpr,
  VariableExpr,
  BinaryExpr,
  CallExpr,
  ExpressionStmt,
  FunctionStmt,
  PrototypeStmt,
};

class ASTNode {
  const ASTType m_type;

public:
  ASTNode(ASTType type) : m_type(type) {};
  template<typename T>
  T as() {
    return static_cast<T>(this);
  };

  const ASTType type() const {
    return m_type;
  }
};

class ExprAST : public ASTNode {
public:
  ExprAST(ASTType type) : ASTNode(type) {};
  virtual ~ExprAST() {};
};

class NumberExprAST : public ExprAST {
  double m_value;
public:
  NumberExprAST(double value) : m_value(value), ExprAST(ASTType::NumberExpr) {};
  double value() const {
    return m_value;
  }
};

class VariableExprAST : public ExprAST {
  std::string m_name;
public:
  VariableExprAST(const std::string &name) : m_name(name), ExprAST(ASTType::VariableExpr) {};
};

class BinaryExprAST : public ExprAST {
  TokenType m_operation;
  std::unique_ptr<ExprAST> m_LHS, m_RHS;
public:
  BinaryExprAST(TokenType operation, 
                std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : m_operation(operation)
      , m_LHS(std::move(LHS))
      , m_RHS(std::move(RHS))
      , ExprAST(ASTType::BinaryExpr) { };

  template<typename T>
  T left() const {
    return static_cast<T>(m_LHS.get());
  }

  template<typename T>
  T right() const {
    return static_cast<T>(m_RHS.get());
  }

  TokenType binOp() const {
    return m_operation;
  }

};

class CallExprAST : public ExprAST {
  std::string m_callee;
  std::vector<std::unique_ptr<ExprAST>> m_args;
public:
  CallExprAST(const std::string &callee, 
              std::vector<std::unique_ptr<ExprAST>> args)
      : m_callee(callee)
      , m_args(std::move(args))
      , ExprAST(ASTType::CallExpr) {};
};

class StmtAST : public ASTNode {
public:
  virtual ~StmtAST() {};
  StmtAST(ASTType type) : ASTNode(type) {};
};

class ExpressionStmtAST : public StmtAST {
  std::unique_ptr<ExprAST> m_expr;
public:
  ExpressionStmtAST(std::unique_ptr<ExprAST> expr) 
      : m_expr(std::move(expr))
      , StmtAST(ASTType::ExpressionStmt) {};

  template<typename T>
  T child() const {
    return static_cast<T>(m_expr.get());
  }
};

class PrototypeAST : public StmtAST {
  std::string m_name;
  std::vector<std::string> m_args;
public:
  PrototypeAST(const std::string &name, std::vector<std::string> args)
      : m_name(name)
      , m_args(std::move(args))
      , StmtAST(ASTType::PrototypeStmt) {};
  
};

struct FunctionAST : public StmtAST {
private:
  std::unique_ptr<PrototypeAST> m_proto;
  std::unique_ptr<ExprAST> m_body;
public:
  FunctionAST(std::unique_ptr<PrototypeAST> proto, 
              std::unique_ptr<ExprAST> body)
      : m_proto(std::move(proto))
      , m_body(std::move(body))
      , StmtAST(ASTType::FunctionStmt) {};
};

#endif // !AST_HPP
