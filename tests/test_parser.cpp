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
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmt *>();

  auto expr = stmt->body->as<NumberExpr *>();

  EXPECT_EQ(expr->value, 42);
}

TEST(Parser, SimpleBinaryExpr) {
  Parser parser = Parser("42 + 100");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);

  auto stmt = statements[0]->as<ExpressionStmt *>();

  auto expr = stmt->body->as<BinaryExpr *>();
  auto left = expr->left<NumberExpr *>();
  auto right = expr->right<NumberExpr *>();

  EXPECT_EQ(expr->operation, TokenType::Plus);
  EXPECT_EQ(left->value, 42);
  EXPECT_EQ(right->value, 100);
}

TEST(Parser, GroupingExpr) {
  Parser parser = Parser("2 + (100 * 2)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);
  auto stmt = statements[0].get()->as<ExpressionStmt *>();

  auto expr = stmt->body->as<BinaryExpr *>();

  auto left = expr->left<NumberExpr *>();
  auto grouping = expr->right<BinaryExpr *>();

  auto g_left = grouping->left<NumberExpr *>();
  auto g_op = grouping->operation;
  auto g_right = grouping->right<NumberExpr *>();

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
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);
  auto stmt = statements[0].get()->as<ExpressionStmt *>();

  auto expr = stmt->body->as<BinaryExpr *>();

  auto _42 = expr->left<NumberExpr *>();
  auto right = expr->right<BinaryExpr *>();

  auto _100 = right->left<NumberExpr *>();
  auto grouping = right->right<BinaryExpr *>();
  auto _60 = grouping->left<NumberExpr *>();
  auto _2 = grouping->right<NumberExpr *>();

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
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmt *>();

  auto expr = stmt->body->as<VariableExpr *>();
  EXPECT_EQ(expr->name, "_variableName123");
}

TEST(Parser, FunctionCall) {
  Parser parser = Parser("dist(x,y)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmt *>();
  auto expr = stmt->body->as<CallExpr *>();

  EXPECT_EQ(expr->callee, "dist");

  auto &args = expr->args;

  auto _x = args[0].get()->as<VariableExpr *>();
  auto _y = args[1].get()->as<VariableExpr *>();

  EXPECT_EQ(args.size(), 2);
  EXPECT_EQ(_x->name, "x");
  EXPECT_EQ(_y->name, "y");
}

TEST(Parser, FunctionDefinition) {
  Parser parser = Parser("def add(x,y) return x + y end");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::FunctionStmt);

  auto stmt = statements[0]->as<FunctionStmt *>();
  auto expr = stmt->body.get()->as<BlockStmt *>();

  auto &returnStmt = expr->returnStmt;

  // returnStmt must not be equal to the nullptr
  ASSERT_TRUE(returnStmt);

  auto returnValue = returnStmt->returnValue->as<BinaryExpr *>();

  EXPECT_EQ(returnValue->left<VariableExpr *>()->name, "x");
  EXPECT_EQ(returnValue->operation, TokenType::Plus);
  EXPECT_EQ(returnValue->right<VariableExpr *>()->name, "y");
}

TEST(Parser, SimpleIf) {
  Parser parser = Parser("if 10 == 2*10 then return 0 end");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), AstType::IfStmt);

  auto &condition = statements[0]->as<IfStmt *>()->condition;
  auto &thenBlock = statements[0]->as<IfStmt *>()->thenBlock;

  auto expr = condition->as<BinaryExpr *>();

  ASSERT_EQ(condition->type(), AstType::BinaryExpr);

  EXPECT_EQ(expr->left<NumberExpr *>()->value, 10);
  EXPECT_EQ(expr->operation, TokenType::EqualEqual);

  auto _2_times_10 = expr->right<BinaryExpr *>();

  EXPECT_EQ(_2_times_10->left<NumberExpr *>()->value, 2);
  EXPECT_EQ(_2_times_10->operation, TokenType::Star);
  EXPECT_EQ(_2_times_10->right<NumberExpr *>()->value, 10);

  EXPECT_EQ(thenBlock->statements.size(), 0);
  EXPECT_TRUE(thenBlock->returnStmt);

  auto returnValue = thenBlock->returnStmt->returnValue->as<NumberExpr *>();

  EXPECT_EQ(returnValue->value, 0);
}
