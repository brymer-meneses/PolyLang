
#include <array>
#include <iterator>
#include <gtest/gtest.h>
#include <Lexer.hpp>

TEST(Lexer, SingleCharTokens) {
  Lexer lexer = Lexer("+ = / * > < !");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 8);

  std::array<TokenType, 8> correctTokenTypes = {
    TokenType::Plus,
    TokenType::Equal,
    TokenType::Slash,
    TokenType::Star,
    TokenType::Greater,
    TokenType::Lesser,
    TokenType::Bang,
    TokenType::Eof,
  };

  for (unsigned int i=0; i < tokens.size(); i++) {
    EXPECT_EQ(tokens[i].m_type, correctTokenTypes[i]) << i;
  }
}

TEST(Lexer, DoubleCharTokens) {
  Lexer lexer = Lexer(">= <= != ==");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 5);

  std::array<TokenType, 5> correctTokenTypes = {
    TokenType::GreaterEqual,
    TokenType::LesserEqual,
    TokenType::BangEqual,
    TokenType::EqualEqual,
    TokenType::Eof,
  };

  for (unsigned int i=0; i < tokens.size(); i++) {
    EXPECT_EQ(tokens[i].m_type, correctTokenTypes[i]) << i;
  }
}

TEST(Lexer, SimpleComment) {
  Lexer lexer = Lexer("// this should be ignored");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].m_type, TokenType::Eof);
}

TEST(Lexer, ComplexComment) {
  Lexer lexer = Lexer("// this should be ignored \n however this should not");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 5);
  EXPECT_EQ(tokens[-1].m_type, TokenType::Eof);
}

TEST(Lexer, String) {
  Lexer lexer = Lexer(" \"This is a string!\" \"This is another string!\" ");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 3);
  EXPECT_EQ(tokens[0].m_type, TokenType::String);
  EXPECT_EQ(tokens[1].m_type, TokenType::String);

  EXPECT_EQ(tokens[0].m_value.value(), Object("This is a string!"));
  EXPECT_EQ(tokens[1].m_value.value(), Object("This is another string!"));
}

TEST(Lexer, Keywords) {
  Lexer lexer = Lexer("let and or def if else then end elif");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 10);

  std::array<TokenType, 10> correctTokenTypes = {
    TokenType::Let,
    TokenType::And,
    TokenType::Or,
    TokenType::Def,
    TokenType::If,
    TokenType::Else,
    TokenType::Then,  
    TokenType::End,
    TokenType::ElseIf,
    TokenType::Eof,  
  };

  for (unsigned int i=0; i < tokens.size(); i++) {
    EXPECT_EQ(tokens[i].m_type, correctTokenTypes[i]) << i;
  }
}

TEST(Lexer, Identifiers) {
  Lexer lexer = Lexer("_test varName var123 _var_123");
  auto tokens = lexer.scanTokens();

  ASSERT_EQ(tokens.size(), 5);

  for (unsigned int i=0; i < tokens.size() - 1; i++) {
    EXPECT_EQ(tokens[i].m_type, TokenType::Identifier) << i;
  }

  ASSERT_EQ(tokens[-1].m_type, TokenType::Eof);
}

TEST(Lexer, Number) {
  Lexer lexer = Lexer("3.141519 1.618 2.71828 137 42");
  auto tokens = lexer.scanTokens();

  std::array<double, 5> correctNumbers = {3.141519, 1.618, 2.71828, 137, 42};

  ASSERT_EQ(tokens.size(), 6);

  for (unsigned int i=0; i < tokens.size() - 1; i++) {
    EXPECT_EQ(tokens[i].m_type, TokenType::Number);
    EXPECT_EQ(tokens[i].m_value.value(), Object(correctNumbers[i]));
  }

  ASSERT_EQ(tokens[-1].m_type, TokenType::Eof);

}
