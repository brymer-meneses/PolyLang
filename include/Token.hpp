#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "Location.hpp"
#include "Object.hpp"
#include <optional>

enum class TokenType {
  // end of file
  Eof,

  // single character tokens
  Equal,
  Plus,
  Minus,
  Star,
  Slash,
  Greater,
  Lesser,
  Bang,
  LeftParen,
  RightParen,
  Comma,

  // two character tokens
  BangEqual,
  EqualEqual,
  GreaterEqual,
  LesserEqual,

  // keyword tokens
  And,
  Or,
  If,
  End,
  Then,
  Else,
  ElseIf,
  Let,
  Def,
  Identifier,
  Extern,

  // type tokens
  Number,
  String,

};

struct Token {
  LineLoc m_lineLoc;
  TokenType m_type;
  std::optional<Object> m_value;

  Token(TokenType type, std::optional<Object> value, LineLoc lineLoc) : m_type(type), m_value(value), m_lineLoc(lineLoc) { }
};


#endif // !TOKEN_HPP
