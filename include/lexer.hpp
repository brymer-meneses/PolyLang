#include <token.hpp>
#include <string>
#include <token.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

class Lexer {
    public:
        Lexer(std::string source) : source(source), start(0), current(0) {};
        std::vector<std::unique_ptr<Token>> getTokens();

    private:
        std::string source;
        int start;
        int line;
        int current;
        char advance();
        char peek();
        std::unique_ptr<Token> getToken();
        std::unique_ptr<Token> scanString();
        std::unique_ptr<Token> scanIdentifier();
        std::unique_ptr<Token> scanNumber();
        bool isAtEnd();
};