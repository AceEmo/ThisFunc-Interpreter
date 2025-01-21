#include "thisFuncSingleton.hpp"

int main(int argc, const char** argv) {

    std::cout << "\033[1m\033[36mWelcome to thisFunc's interpreter!\033[0m" << std::endl;
    std::cout << "\033[1m\033[34m              Made by Emil Peev\033[0m" << std::endl;
    std::cout << "\033[1m\033[36m---------------------------------\033[0m" << std::endl;
    switch (argc) {
    case 1:
        return ListFunc::getInstance().run();
    case 2:
        return ListFunc::getInstance().run(argv[1]);
    default:
        return -1;
    }
}