#pragma once

#include <iostream>
#include <string>

struct Token {
    enum class Type {
        COMMA,

        OPEN_ROUND,
        CLOSE_ROUND,
        
        OPEN_SQUARE,
        CLOSE_SQUARE,

        ARROW,

        ARG,
        FUNC,
        
        KW_INT,
        KW_DOUBLE,
        KW_LIST,

        eof,
    };

    Type type;
    std::string data;
    int startIdx;
};