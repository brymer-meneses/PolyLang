#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <variant>

typedef std::variant<double, std::string_view> Object;

#endif // !OBJECT_HPP
