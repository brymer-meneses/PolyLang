
#include <lexer.hpp>

static std::map<std::string, TokenType> keywords = {
    {"and", tok_and},
    {"or", tok_or},
    // {"true", tok_true},
    // {"false", tok_false},
    // {"if", tok_if},
    // {"else", tok_else},
    // {"while", tok_while},
    // {"for", tok_for},
    // {"fun", tok_fun},
    // {"return", tok_return},
    // {"var", tok_var},
    // {"class", tok_class},
    // {"this", tok_this},
    // {"super", tok_super},
    // {"nil", tok_nil},
};

bool Lexer::isAtEnd() {
    return this->current == this->source.length() - 1;
}

char Lexer::advance() {
    if (this->isAtEnd()) {
        return '\0';
    }

    this->current += 1;
    return this->source[this->current - 1];
}

char Lexer::peek() {
    if (this->current == this->source.length()) {
        return '\0';
    }

    return this->source[this->current + 1];
}

std::vector<std::unique_ptr<Token>> Lexer::getTokens() {
    std::vector<std::unique_ptr<Token>> tokens;

    while (!this->isAtEnd()) {
        tokens.push_back(this->getToken());
    }

    return tokens;
}

std::unique_ptr<Token> Lexer::getToken() {
    char c = this->advance();
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
            this->advance();
            return this->getToken();
        case '\n':
            this->advance();
            this->line += 1;
            return this->getToken();
        case '"':
            return this->scanString();
        case '+':
            return std::make_unique<Token>(TokenType::tok_plus, "+", this->current);
        case '-':
            return std::make_unique<Token>(TokenType::tok_minus, "-", this->current);
        case '*':
            return std::make_unique<Token>(TokenType::tok_star, "*", this->current);
        case '#':
            while (this->peek() != '\n' || this->isAtEnd()) {
                this->advance();
            }
            this->advance();
            return this->getToken();
        case '>':
            if (this->peek() == '=') {
                this->advance();
                return std::make_unique<Token>(TokenType::tok_greater_equal, ">=", this->current);
            }
            return std::make_unique<Token>(TokenType::tok_greater, ">", this->current);
        case '<':
            if (this->peek() == '=') {
                this->advance();
                return std::make_unique<Token>(TokenType::tok_lesser_equal, "<=", this->current);
            }
            return std::make_unique<Token>(TokenType::tok_lesser, "<", this->current);
        case '!':
            if (this->peek() == '=') {
                this->advance();
                return std::make_unique<Token>(TokenType::tok_bang_equal, "!=", this->current);
            }
            return std::make_unique<Token>(TokenType::tok_bang, "!", this->current);
        case '=':
            if (this->peek() == '=') {
                this->advance();
                return std::make_unique<Token>(TokenType::tok_equal_equal, "==", this->current);
            }
            return std::make_unique<Token>(TokenType::tok_equal, "=", this->current);
        case '\0':
            printf("eof token added!");
            return std::make_unique<Token>(TokenType::tok_eof, "EOF", this->current);
        default:
            if (isdigit(this->peek())) {
                this->scanNumber();
            } else if (isalnum(this->peek()) || this->peek() == '_' ) {
                this->scanIdentifier();
            }
            
            return std::make_unique<Token>(TokenType::tok_identifier, std::string(1, c), this->current);
    }    
}

std::unique_ptr<Token> Lexer::scanString() {
    int start = this->current;
    while (this->peek() != '"' && !this->isAtEnd())
        this->advance();
    
    return std::make_unique<Token>(TokenType::tok_string, this->source.substr(start, this->current - start), this->current);
}

std::unique_ptr<Token> Lexer::scanIdentifier() {
    int start = this->current - 1;
    while (isalnum(this->peek()) || this->peek() == '_' )
        this->advance();

    std::string identifier = this->source.substr(start, this->current - start + 1);
    // std::cout << identifier << std::endl;

    if (keywords.find(identifier) != keywords.end()) {
        return std::make_unique<Token>(keywords.at(identifier), identifier, this->current);
    }

    return std::make_unique<Token>(TokenType::tok_identifier, identifier, this->current);

}

std::unique_ptr<Token> Lexer::scanNumber() {

}