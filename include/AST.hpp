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

  virtual const std::string typeToString() const {
    switch (m_type) {
      case ASTType::NumberExpr:
        return "Number Expression";
      case ASTType::VariableExpr:
        return "Variable Expression";
      case ASTType::BinaryExpr:
        return "Binary Expression";
      case ASTType::CallExpr:
        return "Call Expression";
      case ASTType::TopLevelExprStmt:
        return "Top Level Expression";
      case ASTType::FunctionStmt:
        return "Function Statement";
      case ASTType::PrototypeStmt:
        return "Prototype Statement";
    };
  }

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
    return visitor.visit(*this);
  }
};

class VariableExprAST : public ExprAST {
  std::string_view m_name;
public:
  VariableExprAST(std::string_view name) : m_name(name), ExprAST(ASTType::VariableExpr) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  std::string_view name() const {
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
    return visitor.visit(*this);
  }
};

class CallExprAST : public ExprAST {
  std::string_view m_callee;
  std::vector<std::unique_ptr<ExprAST>> m_args;
public:
  CallExprAST(std::string_view callee, 
              std::vector<std::unique_ptr<ExprAST>> args)
      : m_callee(callee)
      , m_args(std::move(args))
      , ExprAST(ASTType::CallExpr) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  const std::string_view callee() const {
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
  std::string_view m_name;
  std::vector<std::string_view> m_args;
public:
  PrototypeAST(std::string_view name, std::vector<std::string_view> args)
      : m_name(name)
      , m_args(args)
      , StmtAST(ASTType::PrototypeStmt) {};

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  const std::vector<std::string_view>& args() const {
    return m_args;
  }

  std::string_view name() const {
    return m_name;
  }
  
};

class FunctionAST : public StmtAST {
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
    return visitor.visit(*this);
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

class TopLevelExprAST : public StmtAST {
private:
  std::unique_ptr<PrototypeAST> m_proto;
  std::unique_ptr<ExprAST> m_body;

public:
  TopLevelExprAST(std::unique_ptr<ExprAST> body)
      : StmtAST(ASTType::TopLevelExprStmt) { 
        m_proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string_view>());
        m_body = std::move(body);
      };

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
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
