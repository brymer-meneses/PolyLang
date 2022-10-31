#include "AST.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(Parser, NumberExpr) {

  Parser parser = Parser("42");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), ASTType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmtAST*>();
  auto expr = stmt->child<NumberExprAST*>();

  EXPECT_EQ(expr->value(), 42);
}

TEST(Parser, SimpleBinaryExpr) {
  Parser parser = Parser("42 + 100");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), ASTType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmtAST*>();

  auto expr = stmt->child<BinaryExprAST*>();
  auto left  = expr->left<NumberExprAST*>();
  auto right = expr->right<NumberExprAST*>();

  EXPECT_EQ(expr->binOp(), TokenType::Plus);
  EXPECT_EQ(left->value(), 42);
  EXPECT_EQ(right->value(), 100);
}

TEST(Parser, GroupingExpr) {
  Parser parser = Parser("2 + (100 * 2)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), ASTType::ExpressionStmt);
  auto stmt = statements[0].get()->as<ExpressionStmtAST*>();

  auto expr = stmt->child<BinaryExprAST*>();

  auto left  = expr->left<NumberExprAST*>();
  auto grouping = expr->right<BinaryExprAST*>();

  auto g_left = grouping->left<NumberExprAST*>();
  auto g_op   =  grouping->binOp();
  auto g_right = grouping->right<NumberExprAST*>();

  EXPECT_EQ(expr->binOp(), TokenType::Plus);
  EXPECT_EQ(left->value(), 2);

  EXPECT_EQ(g_left->value(), 100);
  EXPECT_EQ(g_op, TokenType::Star);
  EXPECT_EQ(g_right->value(), 2);
}

TEST(Parser, ComplexBinaryExpr) {
  Parser parser = Parser("42 + 100 * (60 + 2)");
  auto statements = parser.parse();

  ASSERT_EQ(statements.size(), 1);
  ASSERT_EQ(statements[0]->type(), ASTType::ExpressionStmt);

  auto stmt = statements[0].get()->as<ExpressionStmtAST*>();
  auto expr = stmt->child<BinaryExprAST*>();

  auto _42  = expr->left<NumberExprAST*>();
  auto right = expr->right<BinaryExprAST*>();

  auto _100 = right->left<NumberExprAST*>();
  auto grouping = right->right<BinaryExprAST*>();
  auto _60 = grouping->left<NumberExprAST*>();
  auto _2 = grouping->right<NumberExprAST*>();

  EXPECT_EQ(expr->binOp(), TokenType::Plus);
  EXPECT_EQ(_42->value(), 42);
  EXPECT_EQ(_100->value(), 100);
  EXPECT_EQ(_60->value(), 60);
  EXPECT_EQ(_2->value(), 2);
  EXPECT_EQ(right->binOp(), TokenType::Star);
  EXPECT_EQ(grouping->binOp(), TokenType::Plus);
}
