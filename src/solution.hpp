#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace solution {

struct Token {
    enum class Type : short {
        kNumber,
        kPlus,
        kMinus,
        kMultiplication,
        kDivision,
        kOpenBracket,
        kCloseBracket,
        kUnaryMinus,
        kUnaryPlus,
    };

    int32_t value;
    Type type;
};

struct CalculatorBaseException : public std::runtime_error {
    CalculatorBaseException(const char* msg) : runtime_error(msg) {}
};

struct InvalidSyntaxException : public CalculatorBaseException {
    InvalidSyntaxException(const char* msg) : CalculatorBaseException(msg) {}
};

struct DivisionByZeroException : public CalculatorBaseException {
    DivisionByZeroException() : CalculatorBaseException("division by zero") {}
};

class ReversePolishNotation {
public:
    void AddToken(const Token& token);
    int Calculate();
    std::vector<Token> GetRpn(); // for test purposes

private:
    static int GetOperatorPrecedence(Token::Type type);
    void StackPopByPrecedence(int precedence);
    void BuildRpn();

    std::vector<Token> output_value_{};
    std::stack<Token> stack_{};
};

int calculate(std::string&& expression);

std::string format_number(int n);

ReversePolishNotation build_reverse_polish_notation(const std::string& expression);

}