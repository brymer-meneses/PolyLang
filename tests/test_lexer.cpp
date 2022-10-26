#include "gtest/gtest.h"
#include <lexer.hpp>

TEST(TestLexer, TestLexerComment) {
    std::string input = "# this is a comment";
    Lexer lexer(input);
    auto tokens = lexer.getTokens();
    ASSERT_EQ(tokens.size(), 0);
}


// TEST(TestLexer, TestLexerVariables) {
//     std::string input = "this is not a comment";
//     Lexer lexer(input);
//     auto tokens = lexer.getTokens();
//     ASSERT_EQ(tokens.size(), 5);
// }