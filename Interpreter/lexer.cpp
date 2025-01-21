#include <cstring>
#include "lexer.hpp"

Lexer::Lexer(const std::string& input) : input(input) {}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;
    int currentIdx = 0;

    while (currentIdx < input.length()) {
        int tokenStartIdx = currentIdx;
        char next = input[currentIdx];

        if (next == ' ' || next == '\t') {
            ++currentIdx;
        }
        else if (next == ',') {
            ++currentIdx;
            tokens.push_back({ Token::Type::COMMA, ",", tokenStartIdx });
        }
        else if (next == '[') {
            ++currentIdx;
            tokens.push_back({Token::Type::OPEN_SQUARE, "[", tokenStartIdx});
        }
        else if (next == ']') {
            ++currentIdx;
            tokens.push_back({Token::Type::CLOSE_SQUARE, "]", tokenStartIdx});
        }
        else if (next == '(') {
            ++currentIdx;
            tokens.push_back({Token::Type::OPEN_ROUND, "(", tokenStartIdx});
        }
        else if (next == ')') {
            ++currentIdx;
            tokens.push_back({Token::Type::CLOSE_ROUND, ")", tokenStartIdx});
        }
        else if (next == '<' && currentIdx + 1 < input.length() && input[currentIdx + 1] == '-') {
            currentIdx += 2;
            tokens.push_back({Token::Type::ARROW, "<-", tokenStartIdx});
        }
        else if (next == '#') {
            std::string word;
            ++currentIdx;

            while (currentIdx < input.length() && isdigit(input[currentIdx])) {
                word += input[currentIdx];
                ++currentIdx;
            }

            tokens.push_back({Token::Type::ARG, word, tokenStartIdx});
        }
        else if (next == 'l' && input.substr(currentIdx, 4) == "list") {
            currentIdx += 4;
            tokens.push_back({Token::Type::KW_LIST, "list", tokenStartIdx});
        }
        else if ((next >= '0' && next <= '9') || next == '-' || next == '+') {
            std::string word;
            bool decimal = false, empty = true;

            if (next == '-' || next == '+') {
                word += input[currentIdx];
                ++currentIdx;
            }

            while (currentIdx < input.length() && isdigit(input[currentIdx])) {
                empty = false;
                word += input[currentIdx];
                ++currentIdx;
            }

            if (currentIdx < input.length() && input[currentIdx] == '.') {
                decimal = true;
                word += input[currentIdx];
                ++currentIdx;
            }

            while (currentIdx < input.length() && isdigit(input[currentIdx])) {
                word += input[currentIdx];
                ++currentIdx;
            }

            if (empty) {
                throw std::runtime_error("Lexer error while lexing a number");
            }

            if (decimal) {
                tokens.push_back({Token::Type::KW_DOUBLE, word, tokenStartIdx});
            }
            else {
                tokens.push_back({Token::Type::KW_INT, word, tokenStartIdx});
            }
        }
        else if (isalpha(next)) {
            std::string word;
            
            while (currentIdx < input.length() && (isalpha(input[currentIdx]) || isdigit(input[currentIdx]))) {
                word += input[currentIdx];
                ++currentIdx;
            }

            tokens.push_back({Token::Type::FUNC, word, tokenStartIdx});
        }
        else {
            throw std::runtime_error("Unknown character while generating tokens");
        }
    }
    tokens.push_back({Token::Type::eof, "", currentIdx});
    return tokens;
}