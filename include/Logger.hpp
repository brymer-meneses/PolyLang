#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>

inline void LogError(const char* string) {

  std::fprintf(stderr, "Logger: %s\n", string);
}

#endif // !LOGGER_HPP
