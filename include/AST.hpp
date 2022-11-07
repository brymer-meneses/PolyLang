#ifndef AST_HPP
#define AST_HPP

#include "Token.hpp"
#include "Compiler.hpp"
#include "Visitor.hpp"

#include <llvm/IR/Value.h>

#include <memory>
#include <string>
#include <vector>


enum class AstType {
  TopLevelExprStmt,
  NumberExpr,
  VariableExpr,
  BinaryExpr,
  CallExpr,
  FunctionStmt,
  PrototypeStmt,
};


struct ExprAST {
  ExprAST() {};
  virtual ~ExprAST() {};
  virtual llvm::Value* accept(Compiler& visitor) const = 0;

  template<typename T>
  T as() {
    return static_cast<T>(this);
  };

  virtual AstType type() const = 0;
};

struct NumberExprAST : public ExprAST {
  double value;

  NumberExprAST(double value) : value(value) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::NumberExpr;
  }
};

struct VariableExprAST : public ExprAST {

  std::string_view name;

  VariableExprAST(std::string_view name) : name(name) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::VariableExpr;
  }

};

struct BinaryExprAST : public ExprAST {
private:
  std::unique_ptr<ExprAST> m_left, m_right;
public:

  TokenType operation;
  BinaryExprAST(TokenType operation, 
                std::unique_ptr<ExprAST> left,
                std::unique_ptr<ExprAST> right)
      : operation(operation)
      , m_left(std::move(left))
      , m_right(std::move(right)) { };

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::BinaryExpr;
  }

  template<typename T>
  T left() {
    return static_cast<T>(m_left.get());
  };

  template<typename T>
  T right() {
    return static_cast<T>(m_right.get());
  };

  const ExprAST* left() const {
    return m_left.get();
  }

  const ExprAST* right() const {
    return m_right.get();
  }


};

struct CallExprAST : public ExprAST {
  std::string_view callee;
  std::vector<std::unique_ptr<ExprAST>> args;

  CallExprAST(std::string_view callee, 
              std::vector<std::unique_ptr<ExprAST>> args)
      : callee(callee)
      , args(std::move(args)) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::CallExpr;
  }


};

struct StmtAST  {

  virtual ~StmtAST() {};
  virtual llvm::Function* accept(Compiler& visitor) const = 0;

  template<typename T>
  T as() {
    return static_cast<T>(this);
  };

  virtual AstType type() const = 0;
};

struct PrototypeAST : public StmtAST {
  std::string_view name;
  std::vector<std::string_view> args;

  PrototypeAST(std::string_view name, std::vector<std::string_view> args)
      : name(name)
      , args(args) {};

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::PrototypeStmt;
  }
};

struct FunctionAST : public StmtAST {
  std::unique_ptr<ExprAST> body;
  std::unique_ptr<PrototypeAST>proto;

  FunctionAST(std::unique_ptr<PrototypeAST> proto, 
              std::unique_ptr<ExprAST> body)
      : proto(std::move(proto))
      , body(std::move(body)) {};


  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::FunctionStmt;
  }

};

struct TopLevelExprAST : public StmtAST {

  std::unique_ptr<PrototypeAST> proto;
  std::unique_ptr<ExprAST> body;

  TopLevelExprAST(std::unique_ptr<ExprAST> body) 
    : proto(std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string_view>()))
    , body(std::move(body)) { }

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::TopLevelExprStmt;
  }

};


#endif // !AST_HPP
