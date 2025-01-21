#include <fstream>
#include "thisFuncSingleton.hpp"

int ListFunc::run() {
    std::string line;

    while (true) {
        std::getline(std::cin, line);
        if (line == "exit" || std::cin.eof()) {
            break;
        } else if (line.empty() || line[0] == '#') {
            continue;
        }

        try {
            Lexer lexer(line);
            std::vector<Token> tokens = lexer.lex();

            Parser parser(tokens.begin());
            FunctionScope localScope(globalScope, nullptr, std::vector<std::shared_ptr<Node>>());
            std::shared_ptr<Value> val = parser.parse(std::cout)->eval(localScope);

            if (val) {
                std::cout << ">> " << val->toString() << '\n';
            }
        } catch (const std::runtime_error &execException) {
            std::cerr << execException.what() << std::endl;
            continue;
        } catch (...) {
            return -1;
        }
    }
    return 0;
}

int ListFunc::run(const char* path) {
    std::string line;

    std::ifstream file(path);
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line == "exit") {
                break;
            }

            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::cout << line << '\n';

            try {
                Lexer lexer(line);
                std::vector<Token> tokens = lexer.lex();

                Parser parser(tokens.begin());
                FunctionScope localScope(globalScope, nullptr, std::vector<std::shared_ptr<Node>>());
                std::shared_ptr<Value> val = parser.parse(std::cout)->eval(localScope);

                if (val) {
                    std::cout << ">> " << val->toString() << '\n';
                }
            } catch (const std::runtime_error &execException) {
                std::cerr << execException.what() << std::endl;
                continue;
            } catch (...) {
                return -1;
            }
        }
        file.close();
        return 0;
    }
    throw std::runtime_error("Problem while opening file!");
}
