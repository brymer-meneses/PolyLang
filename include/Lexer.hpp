#ifndef LEXER_HPP
#define LEXER_HPP

#include "Location.hpp"
#include "Token.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Lexer {

private:
  std::size_t m_start;
  std::size_t m_current;
  std::size_t m_line;
  std::string_view m_source;

public:
  Lexer(std::string_view source)
      : m_source(source), m_current(0), m_start(0), m_line(1) {}

  std::vector<Token> scanTokens();

private:
  std::optional<Token> scanSingleToken();
  Token scanIdentifier();
  Token scanString();
  Token scanNumber();

  // Helper Methods are defined here
  bool isFinished() { return m_current >= m_source.length(); }

  LineLoc computeLineLocation() {
    return LineLoc(m_start, m_current - 1, m_line);
  }

  char advance() {
    if (isFinished())
      return '\0';

    m_current += 1;
    return m_source[m_current - 1];
  }

  bool match(const char c) {
    if (peek() == c) {
      m_current += 1;
      return true;
    }

    return false;
  }

  char peek() {
    if (isFinished())
      return '\0';
    return m_source[m_current];
  }

  char peekNext() {
    if (isFinished())
      return '\0';
    return m_source[m_current + 1];
  }
};

#endif // !LEXER_HPP
