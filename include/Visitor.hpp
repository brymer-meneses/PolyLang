#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>

class ExprAST;
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class CallExprAST;
class StmtAST;

class FunctionAST;
class TopLevelExprAST;
class PrototypeAST;

template<typename T>
class ExprVisitor {

public:
  virtual ~ExprVisitor() {};
  virtual T visitNumberExpr(const NumberExprAST& expr) = 0;
  virtual T visitVariableExpr(const VariableExprAST& expr) = 0;
  virtual T visitBinaryExpr(const BinaryExprAST& expr) = 0;
  virtual T visitCallExpr(const CallExprAST& expr) = 0;

};

template<typename T>
class StmtVisitor {

public:
  virtual ~StmtVisitor() {};
  virtual T visitFunctionStmt(const FunctionAST& stmt) = 0;
  virtual T visitPrototypeStmt(const PrototypeAST& stmt) = 0;
  virtual T visitTopLevelExpr(const TopLevelExprAST& stmt) = 0;
};

#endif // !EXPR_VISITOR_HPP
