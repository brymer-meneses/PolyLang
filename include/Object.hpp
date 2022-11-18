#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <variant>

using Object = std::variant<double, std::string_view>;

#endif // !OBJECT_HPP
