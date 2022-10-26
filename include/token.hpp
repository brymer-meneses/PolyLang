#pragma once
#include <string>

enum TokenType {
    tok_eof,
    tok_and,
    tok_or,
    tok_num,
    tok_string,
    tok_plus,
    tok_minus,
    tok_star,
    tok_bang,
    tok_equal_equal,
    tok_equal,
    tok_bang_equal,
    tok_greater_equal,
    tok_greater,
    tok_lesser,
    tok_lesser_equal,
    tok_identifier,
};

class Token  {
public:
    TokenType type;
    std::string lexeme;
    int pos;
    Token(TokenType type, std::string lexeme, int pos) : type(type), lexeme(lexeme), pos(pos) {} // initializer list 
};