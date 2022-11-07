#include "AST.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(Parser, NumberExpr) {

  Parser parser = Parser("42");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);

  auto stmt = statements[0].get()->as<TopLevelExprAST*>();

  auto expr = stmt->body->as<NumberExprAST*>();

  EXPECT_EQ(expr->value, 42);

}

TEST(Parser, SimpleBinaryExpr) {
  Parser parser = Parser("42 + 100");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);

  auto stmt = statements[0]->as<TopLevelExprAST*>();

  auto expr = stmt->body->as<BinaryExprAST*>();
  auto left  = expr->left<NumberExprAST*>();
  auto right = expr->right<NumberExprAST*>();

  EXPECT_EQ(expr->operation, TokenType::Plus);
  EXPECT_EQ(left->value, 42);
  EXPECT_EQ(right->value, 100);
}

TEST(Parser, GroupingExpr) {
  Parser parser = Parser("2 + (100 * 2)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);
  auto stmt = statements[0].get()->as<TopLevelExprAST*>();

  auto expr = stmt->body->as<BinaryExprAST*>();

  auto left  = expr->left<NumberExprAST*>();
  auto grouping = expr->right<BinaryExprAST*>();

  auto g_left = grouping->left<NumberExprAST*>();
  auto g_op   =  grouping->operation;
  auto g_right = grouping->right<NumberExprAST*>();

  EXPECT_EQ(expr->operation, TokenType::Plus);
  EXPECT_EQ(left->value, 2);

  EXPECT_EQ(g_left->value, 100);
  EXPECT_EQ(g_op, TokenType::Star);
  EXPECT_EQ(g_right->value, 2);
}

TEST(Parser, ComplexBinaryExpr) {
  Parser parser = Parser("42 + 100 * (60 + 2)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);
  auto stmt = statements[0].get()->as<TopLevelExprAST*>();

  auto expr = stmt->body->as<BinaryExprAST*>();

  auto _42  = expr->left<NumberExprAST*>();
  auto right = expr->right<BinaryExprAST*>();

  auto _100 = right->left<NumberExprAST*>();
  auto grouping = right->right<BinaryExprAST*>();
  auto _60 = grouping->left<NumberExprAST*>();
  auto _2 = grouping->right<NumberExprAST*>();

  EXPECT_EQ(expr->operation, TokenType::Plus);
  EXPECT_EQ(_42->value, 42);
  EXPECT_EQ(_100->value, 100);
  EXPECT_EQ(_60->value, 60);
  EXPECT_EQ(_2->value, 2);
  EXPECT_EQ(right->operation, TokenType::Star);
  EXPECT_EQ(grouping->operation, TokenType::Plus);
}

TEST(Parser, Variable) {
  Parser parser = Parser("_variableName123");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);

  auto stmt = statements[0].get()->as<TopLevelExprAST*>();

  auto expr = stmt->body->as<VariableExprAST*>();
  EXPECT_EQ(expr->name, "_variableName123");
}

TEST(Parser, FunctionCall) {
  Parser parser = Parser("dist(x,y)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::TopLevelExprStmt);

  auto stmt = statements[0].get()->as<TopLevelExprAST*>();
  auto expr = stmt->body->as<CallExprAST*>();

  EXPECT_EQ(expr->callee, "dist");

  auto& args = expr->args;

  auto _x = args[0].get()->as<VariableExprAST*>();
  auto _y = args[1].get()->as<VariableExprAST*>();

  EXPECT_EQ(args.size(), 2);
  EXPECT_EQ(_x->name, "x");
  EXPECT_EQ(_y->name, "y");

}

TEST(Parser, FunctionDefinition) {
  Parser parser = Parser("def add(x,y) x + y");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::FunctionStmt);

  auto stmt = statements[0]->as<FunctionAST*>();
  auto expr = stmt->body.get()->as<BinaryExprAST*>();

  EXPECT_EQ(expr->left<VariableExprAST*>()->name, "x");
  EXPECT_EQ(expr->operation, TokenType::Plus);
  EXPECT_EQ(expr->right<VariableExprAST*>()->name, "y");
}
