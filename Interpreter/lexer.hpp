#pragma once

#include <vector>
#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string&);
    std::vector<Token> lex();

private:
    std::string input;
};