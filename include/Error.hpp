#ifndef ERROR_HPP
#define ERROR_HPP

#include "Location.hpp"
#include <cassert>
#include <optional>
#include <string>
#include <iostream>

struct Error {
  std::string m_message;
  std::optional<std::string> m_tip;
  std::optional<FileLoc> m_fileLoc;
  std::optional<LineLoc> m_lineLoc;

  Error(std::string message)
      : m_message(message), m_tip(std::nullopt), m_fileLoc(std::nullopt), m_lineLoc(std::nullopt) {}

  Error& setTip(std::string tip) {
    m_tip = tip;
    return *this;
  }

  Error& setFileLoc(FileLoc fileLoc) {
    assert(!m_lineLoc.has_value());

    m_fileLoc = fileLoc;
    return *this;
  }

  Error& setLineLoc(std::size_t start, std::size_t end, std::size_t line) {
    assert(!m_fileLoc.has_value());

    m_lineLoc = LineLoc(start, end, line);
    return *this;
  }

};

#endif // !ERROR_HPP
