
#ifndef  COMPILER_HPP
#define  COMPILER_HPP

#include "Visitor.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

#include <map>
#include <memory>

class PolyLang;

class Compiler : public ExprVisitor<llvm::Value*>, public StmtVisitor<llvm::Function*> {

private:
  std::unique_ptr<llvm::LLVMContext> m_context;
  std::unique_ptr<llvm::IRBuilder<>> m_builder;
  std::unique_ptr<llvm::Module> m_module;
  std::map<std::string_view, llvm::Value*> m_namedValues;
  
public:
  Compiler() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_module = std::make_unique<llvm::Module>("MAIN", *m_context);
    m_builder = std::make_unique<llvm::IRBuilder<>>(*m_context);
  };

  llvm::Value* codegenExpr(const ExprAST* const expr);
  llvm::Function* codegenStmt(const StmtAST* const expr);

  llvm::Value* visitNumberExpr(const NumberExprAST& expr) override;
  llvm::Value* visitVariableExpr(const VariableExprAST& expr) override;
  llvm::Value* visitBinaryExpr(const BinaryExprAST& expr) override;
  llvm::Value* visitCallExpr(const CallExprAST& expr) override;

  llvm::Function* visitFunctionStmt(const FunctionAST& stmt) override;
  llvm::Function* visitPrototypeStmt(const PrototypeAST& stmt) override;
  llvm::Function* visitTopLevelExpr(const TopLevelExprAST& stmt) override;

friend PolyLang;
};


#endif // ! COMPILER_HPP
