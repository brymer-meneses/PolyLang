#include "Lexer.hpp"
#include "Token.hpp"
#include "Error.hpp"

#include <cctype>
#include <cstring>
#include <optional>
#include <vector>
#include <charconv>

#include <map>

const static std::map<std::string_view, TokenType> KEYWORDS = {
  { "and"    ,  TokenType::And    },
  { "or"     ,  TokenType::Or     },
  { "def"    ,  TokenType::Def    },
  { "if"     ,  TokenType::If     },
  { "elif"   ,  TokenType::ElseIf },
  { "else"   ,  TokenType::Else   },
  { "then"   ,  TokenType::Then   },
  { "let"    ,  TokenType::Let    },
  { "end"    ,  TokenType::End    },
  { "extern" ,  TokenType::Extern },
};

std::vector<Token> Lexer::scanTokens() {

  std::vector<Token> tokens;

  while (!isFinished()) {
    m_start = m_current;

    try {
       std::optional<Token> token = scanSingleToken();
      if (token.has_value()) 
        tokens.push_back(token.value());
    } catch (Error e) {
      std::cout << e.m_message << '\n';
    }

  };

  tokens.push_back(Token(TokenType::Eof, std::nullopt, computeLineLocation()));

  return tokens;
}

std::optional<Token> Lexer::scanSingleToken() {
  char c = advance();

  switch (c) {
    case '\n':
      m_line += 1;
      return std::nullopt;
    case ' ':
    case '\r':
    case '\t':
      return std::nullopt;
    case '(':
      return Token(TokenType::LeftParen, std::nullopt, computeLineLocation());
    case ')':
      return Token(TokenType::RightParen, std::nullopt, computeLineLocation());
    case '+':
      return Token(TokenType::Plus, std::nullopt, computeLineLocation());
    case '-':
      return Token(TokenType::Minus, std::nullopt, computeLineLocation());
    case ',':
      return Token(TokenType::Comma, std::nullopt, computeLineLocation());
    case '/':
      if (match('/')) {
        while (peek() != '\n' && peek() != '\0')
          advance();
        return std::nullopt;
      }
      return Token(TokenType::Slash, std::nullopt, computeLineLocation());
    case '*':
      return Token(TokenType::Star, std::nullopt, computeLineLocation());
    case '!':
      if (match('='))
        return Token(TokenType::BangEqual, std::nullopt, computeLineLocation());

      return Token(TokenType::Bang, std::nullopt, computeLineLocation());
    case '>':
      if (match('='))
        return Token(TokenType::GreaterEqual, std::nullopt, computeLineLocation());

      return Token(TokenType::Greater, std::nullopt, computeLineLocation());
    case '<':
      if (match('='))
        return Token(TokenType::LesserEqual, std::nullopt, computeLineLocation());

      return Token(TokenType::Lesser, std::nullopt, computeLineLocation());
    case '=':
      if (match('='))
        return Token(TokenType::EqualEqual, std::nullopt, computeLineLocation());

      return Token(TokenType::Equal, std::nullopt, computeLineLocation());
    case '"':
      return scanString();
    default:
      if (std::isalpha(c) || c == '_') {
        return scanIdentifier();
      } else if (std::isdigit(c)) {
        return scanNumber();
      } else {
        // TODO use libfmt to fix this mess
        throw Error("Unexpected character")
                   .setLineLoc(m_current-1, m_current-1, m_line);
      }
  }
  return std::nullopt;
}

Token Lexer::scanString() {
  while (peek() != '"' && !isFinished()) {
    if (peek() == '\n')
      m_line += 1;

    advance();
  }

  if (isFinished()) {
    throw Error("Unterminated string")
          .setLineLoc(m_current - 1, m_current - 1, m_line)
          .setTip("Consider placing an enclosing \"");
  }

  advance();

  std::string_view substring = m_source.substr(m_start+1, m_current - m_start-2);

  return Token(TokenType::String, substring, LineLoc(m_start+1, m_current-1, m_line));
}

Token Lexer::scanIdentifier() {
  while (std::isalnum(peek()) || peek() == '_')
    advance();

  std::string_view keyword = m_source.substr(m_start, m_current-m_start);

  TokenType type;
  try {
    type = KEYWORDS.at(keyword);
    return Token(type, std::nullopt, LineLoc(m_start, m_current, m_line));
  } catch (std::out_of_range e) {
    type = TokenType::Identifier;
    // pass lexeme to the keyword which will be used to reference a variable
    return Token(type, keyword, LineLoc(m_start, m_current, m_line));
  }

}
Token Lexer::scanNumber() {
  while (std::isdigit(peek()))
    advance();

  if (peek() == '.' && std::isdigit(peekNext())) {
    advance();

    while (std::isdigit(peek()))
      advance();
  }

  double num;
  std::string_view string_lexeme = m_source.substr(m_start, m_start - m_current);
  std::from_chars(string_lexeme.data(), string_lexeme.data() + string_lexeme.size(), num);

  return Token(TokenType::Number, num, LineLoc(m_start, m_current, m_line));
}
