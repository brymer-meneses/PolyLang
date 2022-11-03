
#ifndef  COMPILER_HPP
#define  COMPILER_HPP

#include "Visitor.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
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
    initializeModuleAndPassManager();
  };

  void initializeModuleAndPassManager() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_module = std::make_unique<llvm::Module>("__PolyLangMain", *m_context);
    m_builder = std::make_unique<llvm::IRBuilder<>>(*m_context);


  };

  llvm::Value* codegen(const ExprAST* const expr);
  llvm::Function* codegen(const StmtAST* const stmt);

  llvm::Value* visit(const NumberExprAST& expr) override;
  llvm::Value* visit(const VariableExprAST& expr) override;
  llvm::Value* visit(const BinaryExprAST& expr) override;
  llvm::Value* visit(const CallExprAST& expr) override;

  llvm::Function* visit(const FunctionAST& stmt) override;
  llvm::Function* visit(const PrototypeAST& stmt) override;
  llvm::Function* visit(const TopLevelExprAST& stmt) override;

friend PolyLang;
};


#endif // ! COMPILER_HPP
