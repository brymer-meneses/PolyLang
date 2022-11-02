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

inline std::string_view TokenTypeToString(TokenType type) {

  switch (type) {
    case TokenType::Eof:
      return "EOF";
    case TokenType::Equal:
      return "=";
    case TokenType::Plus:
      return "+";
    case TokenType::Minus:
      return "-";
    case TokenType::Star:
      return "*";
    case TokenType::Slash:
      return "/";
    case TokenType::Greater:
      return ">";
    case TokenType::Lesser:
      return "<";
    case TokenType::Bang:
      return "!";
    case TokenType::LeftParen:
      return "(";
    case TokenType::RightParen:
      return ")";
    case TokenType::Comma:
      return ",";
    case TokenType::BangEqual:
      return "!=";
    case TokenType::EqualEqual:
      return "==";
    case TokenType::GreaterEqual:
      return ">=";
    case TokenType::LesserEqual:
      return "<=";
    case TokenType::And:
      return "and";
    case TokenType::Or:
      return "or";
    case TokenType::If:
      return "if";
    case TokenType::End:
      return "end";
    case TokenType::Then:
      return "then";
    case TokenType::Else:
      return "else";
    case TokenType::ElseIf:
      return "elif";
    case TokenType::Let:
      return "let";
    case TokenType::Def:
      return "def";
    case TokenType::Identifier:
      return "identifier";
    case TokenType::Extern:
      return "extern";
    case TokenType::Number:
      return "number";
    case TokenType::String:
      return "string";
  }
}

struct Token {
  LineLoc m_lineLoc;
  TokenType m_type;
  std::optional<Object> m_value;

  Token(TokenType type, std::optional<Object> value, LineLoc lineLoc) : m_type(type), m_value(value), m_lineLoc(lineLoc) { }
};


#endif // !TOKEN_HPP
