#pragma once

#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

class ListFunc {
public:
    ListFunc(const ListFunc& other) = delete;
    ListFunc& operator=(const ListFunc& other) = delete;

    static ListFunc& getInstance() {
        static ListFunc object;
        return object;
    }

    int run();
    int run(const char* path);

private:
    GlobalScope globalScope;
    ListFunc() {
        globalScope.loadDefaultLibrary();
    }
};