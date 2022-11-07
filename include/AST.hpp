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
  BlockStmt,
  ReturnStmt,
  PrototypeStmt,
};


struct Expr {
  Expr() {};
  virtual ~Expr() {};
  virtual llvm::Value* accept(Compiler& visitor) const = 0;

  template<typename T>
  T as() {
    return static_cast<T>(this);
  };

  virtual AstType type() const = 0;
};

struct NumberExpr : public Expr {
  double value;

  NumberExpr(double value) : value(value) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::NumberExpr;
  }
};

struct VariableExpr : public Expr {

  std::string_view name;

  VariableExpr(std::string_view name) : name(name) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::VariableExpr;
  }

};

struct BinaryExpr : public Expr {
private:
  std::unique_ptr<Expr> m_left, m_right;
public:

  TokenType operation;
  BinaryExpr(TokenType operation, 
                std::unique_ptr<Expr> left,
                std::unique_ptr<Expr> right)
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

  const Expr* left() const {
    return m_left.get();
  }

  const Expr* right() const {
    return m_right.get();
  }


};

struct CallExpr : public Expr {
  std::string_view callee;
  std::vector<std::unique_ptr<Expr>> args;

  CallExpr(std::string_view callee, 
              std::vector<std::unique_ptr<Expr>> args)
      : callee(callee)
      , args(std::move(args)) {};

  llvm::Value* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::CallExpr;
  }


};

struct Stmt  {

  virtual ~Stmt() {};
  virtual llvm::Function* accept(Compiler& visitor) const = 0;

  template<typename T>
  T as() {
    return static_cast<T>(this);
  };

  virtual AstType type() const = 0;
};

struct PrototypeStmt : public Stmt {
  std::string_view name;
  std::vector<std::string_view> args;

  PrototypeStmt(std::string_view name, std::vector<std::string_view> args)
      : name(name)
      , args(args) {};

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::PrototypeStmt;
  }
};

struct FunctionStmt : public Stmt {
  std::unique_ptr<Expr> body;
  std::unique_ptr<PrototypeStmt>proto;

  FunctionStmt(std::unique_ptr<PrototypeStmt> proto, 
              std::unique_ptr<Expr> body)
      : proto(std::move(proto))
      , body(std::move(body)) {};


  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  };

  AstType type() const override {
    return AstType::FunctionStmt;
  }

};

struct BlockStmt : public Stmt {

  std::vector<std::unique_ptr<Stmt>> statements;

  BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
      : statements(std::move(statements)) {}

  AstType type() const override {
    return AstType::BlockStmt;
  }
};

struct ReturnStmt : public Stmt {

  std::optional<std::unique_ptr<Expr>> expr;

  ReturnStmt(std::optional<std::unique_ptr<Expr>> expr)
      : expr(std::move(expr)){};

  AstType type() const override {
    return AstType::BlockStmt;
  }
};

struct TopLevelExprStmt : public Stmt {

  std::unique_ptr<PrototypeStmt> proto;
  std::unique_ptr<Expr> body;

  TopLevelExprStmt(std::unique_ptr<Expr> body) 
    : proto(std::make_unique<PrototypeStmt>("__anon_expr", std::vector<std::string_view>()))
    , body(std::move(body)) { }

  llvm::Function* accept(Compiler& visitor) const override {
    return visitor.visit(*this);
  }

  AstType type() const override {
    return AstType::TopLevelExprStmt;
  }

};


#endif // !AST_HPP
