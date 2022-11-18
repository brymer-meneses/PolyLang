#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>

struct Expr;
struct NumberExpr;
struct NumberExpr;
struct VariableExpr;
struct BinaryExpr;
struct CallExpr;

struct BlockStmt;
struct ReturnStmt;
struct Stmt;
struct FunctionStmt;
struct ExpressionStmt;
struct PrototypeStmt;

template <typename T> class ExprVisitor {

public:
  virtual ~ExprVisitor(){};
  virtual T visit(const NumberExpr &expr) = 0;
  virtual T visit(const VariableExpr &expr) = 0;
  virtual T visit(const BinaryExpr &expr) = 0;
  virtual T visit(const CallExpr &expr) = 0;
};

template <typename T, typename U> class StmtVisitor {

public:
  virtual ~StmtVisitor(){};
  virtual T visit(const FunctionStmt &stmt) = 0;
  virtual T visit(const PrototypeStmt &stmt) = 0;
  virtual T visit(const BlockStmt &stmt) = 0;

  virtual U visit(const ExpressionStmt &stmt) = 0;
  virtual U visit(const ReturnStmt &stmt) = 0;
};

#endif // !EXPR_VISITOR_HPP
