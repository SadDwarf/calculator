#include <iostream>
#include "solution.hpp"


int main() {
    std::string expression;
    std::getline(std::cin, expression);
    int res;
    try {
        res = solution::calculate(std::move(expression));
    } catch (const solution::CalculatorBaseException& ex) {
        std::cout << ex.what();
        return 1;
    }
    std::cout << solution::format_number(res) << std::endl;

    return 0;
}