#include "Compiler.hpp"
#include "AST.hpp"
#include "Logger.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <vector>

using namespace llvm;

Value* Compiler::codegenExpr(const ExprAST* const expr) {
  return expr->accept(*this);
};

Function* Compiler::codegenStmt(const StmtAST* const stmt) {
  return stmt->accept(*this);
};

Value* Compiler::visitNumberExpr(const NumberExprAST& expr) {
  return ConstantFP::get(*m_context, llvm::APFloat(expr.value()));
}

Value* Compiler::visitVariableExpr(const VariableExprAST& expr) {

  Value* v = m_namedValues[expr.name()];

  if (!v)
    LogError("Unknown variable name");

  return v;
}

Value* Compiler::visitBinaryExpr(const BinaryExprAST& expr) {
  Value *L = codegenExpr(expr.left());
  Value *R = codegenExpr(expr.right());

  if (!L || !R)
    return nullptr;

  switch (expr.binOp()) {
    case TokenType::Plus:
      return m_builder->CreateFAdd(L, R, "addtmp");
    case TokenType::Minus:
      return m_builder->CreateFSub(L, R, "subtmp");
    case TokenType::Star:
      return m_builder->CreateFMul(L, R, "multmp");
    case TokenType::Slash:
      return m_builder->CreateFDiv(L, R, "divtmp");

    case TokenType::Lesser:
      L = m_builder->CreateFCmpULT(L, R, "cmptmp");
      return m_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*m_context), "booltmp");
    case TokenType::LesserEqual:
      L = m_builder->CreateFCmpULE(L, R, "cmptmp");
      return m_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*m_context), "booltmp");
    case TokenType::Greater:
      L = m_builder->CreateFCmpUGT(L, R, "cmptmp");
      return m_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*m_context), "booltmp");
    case TokenType::GreaterEqual:
      L = m_builder->CreateFCmpUGE(L, R, "cmptmp");
      return m_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*m_context), "booltmp");

    default:
      LogError("Invalid binary operator");
      return nullptr;
  }

}

Value* Compiler::visitCallExpr(const CallExprAST& expr) {

  Function* CalleeF = m_module->getFunction(expr.callee());

  std::size_t args_size = expr.args().size();

  if (!CalleeF) {
    LogError("Unknown function referenced");
    return nullptr;
  }

  if (CalleeF->arg_size() != args_size) {
    LogError("Unknown function referenced");
    return nullptr;
  }

  std::vector<llvm::Value *> argsV;

  for (unsigned i=0; i != args_size; ++i) {
    // compile
    argsV.push_back( codegenExpr( expr.args()[i].get() ) );

    // if the last arg is a nullptr
    if (!argsV.back())
      return nullptr;

  }

  return m_builder->CreateCall(CalleeF, argsV, "calltmp");

}

Function* Compiler::visitPrototypeStmt(const PrototypeAST& stmt) {

  auto Args = stmt.args();

  std::vector<llvm::Type *> Doubles(Args.size(), Type::getDoubleTy(*m_context));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(*m_context), Doubles, false);
  Function* F = Function::Create(FT, Function::ExternalLinkage, stmt.name(), m_module.get());

  unsigned Idx = 0;
  for (auto& Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}


Function* Compiler::visitFunctionStmt(const FunctionAST& stmt) {

  auto Proto = stmt.proto();
  auto Body = stmt.body();

  Function* TheFunction = m_module->getFunction(Proto->name());

  if (!TheFunction)
    TheFunction = codegenStmt(Proto);

  if (!TheFunction)
    return nullptr;

  BasicBlock* BB = BasicBlock::Create(*m_context, "entry", TheFunction);
  m_builder->SetInsertPoint(BB);

  m_namedValues.clear();
  for (auto &Arg : TheFunction->args())
    m_namedValues[std::string(Arg.getName())];
  
  Value* RetVal = codegenExpr(Body);

  if (RetVal) {
    // Finish off the function.
    m_builder->CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    return TheFunction;
  }

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}

Function* Compiler::visitTopLevelExpr(const TopLevelExprAST& stmt) {

  auto Proto = stmt.proto();
  auto Body = stmt.body();

  Function* TheFunction = m_module->getFunction(Proto->name());

  if (!TheFunction)
    TheFunction = codegenStmt(Proto);

  if (!TheFunction)
    return nullptr;

  BasicBlock* BB = BasicBlock::Create(*m_context, "entry", TheFunction);
  m_builder->SetInsertPoint(BB);

  m_namedValues.clear();
  for (auto &Arg : TheFunction->args())
    m_namedValues[std::string(Arg.getName())];
  
  Value* RetVal = codegenExpr(Body);

  if (RetVal) {
    // Finish off the function.
    m_builder->CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    return TheFunction;
  }

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;

}
