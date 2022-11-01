#ifndef AST_HPP
#define AST_HPP

#include "Token.hpp"
#include "Compiler.hpp"

#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/Value.h>

enum class ASTType {
  NumberExpr,
  VariableExpr,
  BinaryExpr,
  CallExpr,
  TopLevelExprStmt,
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

  virtual const ASTType type() const {
    return m_type;
  };

};

class ExprAST : public ASTNode {
public:
  ExprAST(ASTType type) : ASTNode(type) {};
  virtual ~ExprAST() {};
  virtual llvm::Value* accept(Compiler& visitor) const = 0;
};

class NumberExprAST : public ExprAST {
  double m_value;
public:
  NumberExprAST(double value) : m_value(value), ExprAST(ASTType::NumberExpr) {};
  double value() const {
    return m_value;
  }

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visitNumberExpr(*this);
  }
};

class VariableExprAST : public ExprAST {
  std::string m_name;
public:
  VariableExprAST(const std::string &name) : m_name(name), ExprAST(ASTType::VariableExpr) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visitVariableExpr(*this);
  }

  std::string name() const {
    return m_name;
  }
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

  const ExprAST* left() const {
    return m_LHS.get();
  }

  const ExprAST* right() const {
    return m_RHS.get();
  }

  const TokenType binOp() const {
    return m_operation;
  }

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visitBinaryExpr(*this);
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

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visitCallExpr(*this);
  };

  const std::string& callee() const {
    return m_callee;
  }

  const std::vector<std::unique_ptr<ExprAST>>& args() const {
    return m_args;
  }
};

class StmtAST : public ASTNode {
public:
  virtual ~StmtAST() {};
  StmtAST(ASTType type) : ASTNode(type) {};
  virtual llvm::Function* accept(Compiler& visitor) const = 0;
};

class PrototypeAST : public StmtAST {
  std::string m_name;
  std::vector<std::string> m_args;
public:
  PrototypeAST(const std::string &name, std::vector<std::string> args)
      : m_name(name)
      , m_args(std::move(args))
      , StmtAST(ASTType::PrototypeStmt) {};

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visitPrototypeStmt(*this);
  };

  const std::vector<std::string>& args() const {
    return m_args;
  }

  const std::string& name() const {
    return m_name;
  }
  
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

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visitFunctionStmt(*this);
  };

  template<typename T>
  T body() {
    return static_cast<T>(m_body.get());
  };

  const PrototypeAST* const proto() const {
    return m_proto.get();
  };

  const ExprAST* const body() const {
    return m_body.get();
  };
};

#endif // !AST_HPP
