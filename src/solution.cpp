#include "solution.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <stack>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace solution {

namespace {

const std::unordered_set<char> kNumberSeparators{',', '.'};

// token types allowed in the beginning of expression
std::unordered_set<Token::Type> kAllowedInBegin{
        Token::Type::kNumber,
        Token::Type::kUnaryMinus,
        Token::Type::kUnaryPlus,
        Token::Type::kOpenBracket,
};

// token types allowed in the end of expression
std::unordered_set<Token::Type> kAllowedInEnd{
        Token::Type::kNumber,
        Token::Type::kCloseBracket,
};

// used to check allowed token before current
// expression is invalid if token before current has incorrect type
std::unordered_map<Token::Type, std::unordered_set<Token::Type>> kAllowedBefore {
        {
                Token::Type::kNumber,
                {
                        Token::Type::kPlus,
                        Token::Type::kMinus,
                        Token::Type::kMultiplication,
                        Token::Type::kDivision,
                        Token::Type::kOpenBracket,
                        Token::Type::kUnaryMinus,
                        Token::Type::kUnaryPlus,
                }
        },
        {
                Token::Type::kPlus,
                {
                        Token::Type::kNumber,
                        Token::Type::kCloseBracket,
                }
        },
        {
                Token::Type::kMinus,
                {
                        Token::Type::kNumber,
                        Token::Type::kCloseBracket,
                }
        },
        {
                Token::Type::kMultiplication,
                {
                        Token::Type::kNumber,
                        Token::Type::kCloseBracket,
                }
        },
        {
                Token::Type::kDivision,
                {
                        Token::Type::kNumber,
                        Token::Type::kCloseBracket,
                }
        },
        {
                Token::Type::kOpenBracket,
                {
                        Token::Type::kPlus,
                        Token::Type::kMinus,
                        Token::Type::kMultiplication,
                        Token::Type::kDivision,
                        Token::Type::kOpenBracket,
                        Token::Type::kUnaryMinus,
                        Token::Type::kUnaryPlus,
                }
        },
        {
                Token::Type::kCloseBracket,
                {
                        Token::Type::kNumber,
                }
        },
        {
                Token::Type::kUnaryMinus,
                {
                        Token::Type::kOpenBracket,
                }
        },
        {
                Token::Type::kUnaryPlus,
                {
                        Token::Type::kOpenBracket,
                }
        },
};

/**
* Parse number from string
*
* @return pair with parsed number and position of char next to number
*/
std::pair<int32_t , size_t> parse_number(const std::string &expression, size_t begin_ind) {
    // parse decimal part
    size_t ind = begin_ind;
    while (ind < expression.size() && isdigit(expression[ind])) {
        ind++;
    }
    int32_t result = 100 * std::stol(expression.substr(begin_ind, ind - begin_ind));

    // parse fraction digits
    if (ind < expression.size() && kNumberSeparators.count(expression[ind])) {
        ind++;
        begin_ind = ind;
        while (ind < expression.size() && isdigit(expression[ind])) {
            ind++;
        }
        if (begin_ind == ind) throw InvalidSyntaxException("invalid syntax: no digits after .");
        int fraction_part = std::stoi(expression.substr(begin_ind, ind - begin_ind));

        if (fraction_part >= 100) {
            throw InvalidSyntaxException("invalid syntax: too many digits after separator");
        } else if (fraction_part < 10 && ind - begin_ind == 1) {
            result += fraction_part * 10;
        } else {
            result += fraction_part;
        }
    }
    return std::make_pair(result, ind);
}

void check_token_syntax_throw(const Token::Type type, const std::unordered_set<Token::Type>& allowed) {
    if (!allowed.count(type)) {
        throw InvalidSyntaxException("bad tokens order");
    }
}

void check_token_before_throw(const Token::Type type, std::optional<Token::Type> last_token_type) {
    if (!last_token_type) {
        check_token_syntax_throw(type, kAllowedInBegin);
        return;
    }
    const auto it = kAllowedBefore.find(type);
    if (it == kAllowedBefore.end()) {
        throw CalculatorBaseException("unknown token type");
    }
    check_token_syntax_throw(*last_token_type, it->second);
}

Token::Type parse_non_number_token_type(char c, std::optional<Token::Type> last_token_type) {
    using Type = Token::Type;
    switch (c) {
        case '+':
            if (!last_token_type || *last_token_type == Type::kOpenBracket) {
                return Type::kUnaryPlus;
            } else {
                return Type::kPlus;
            }
        case '-':
            if (!last_token_type || *last_token_type == Type::kOpenBracket) {
                return Type::kUnaryMinus;
            } else {
                return Type::kMinus;
            }
        case '*':
            return Type::kMultiplication;
        case '/':
            return Type::kDivision;
        case '(':
            return Type::kOpenBracket;
        case ')':
            return Type::kCloseBracket;
        default:
            throw InvalidSyntaxException("syntax error: unknown symbol found: " + c);
    }
}

int32_t multiply(int32_t a, int32_t b) {
    int64_t res(a);
    res *= b;
    bool carry = res % 100 >= 50;
    res /= 100;
    if (carry) res++;
    return static_cast<int32_t>(res);
}

int32_t divide(int32_t a, int32_t b) {
    double res(a);
    if (b == 0) throw DivisionByZeroException();
    res /= b;
    res *= 100;
    return static_cast<int32_t>(lround(res));
}

}

void ReversePolishNotation::AddToken(const Token& token) {
    switch (token.type) {
        case Token::Type::kUnaryMinus:
        case Token::Type::kUnaryPlus:
        case Token::Type::kOpenBracket:
            stack_.push(token);
            break;
        case Token::Type::kCloseBracket:
            while (!stack_.empty() && stack_.top().type != Token::Type::kOpenBracket) {
                output_value_.push_back(stack_.top());
                stack_.pop();
            }
            if (stack_.empty())
                throw InvalidSyntaxException("invalid parentheses");
            stack_.pop();
            break;
        case Token::Type::kPlus:
        case Token::Type::kMinus:
        case Token::Type::kMultiplication:
        case Token::Type::kDivision:
            StackPopByPrecedence(GetOperatorPrecedence(token.type));
            stack_.push(token);
            break;
        case Token::Type::kNumber:
            output_value_.push_back(token);
            break;
        default:
            throw CalculatorBaseException("incorrect token type in Rpn::AddOperator");
    }
}

int ReversePolishNotation::Calculate() {
    BuildRpn();
    for (const Token& token : output_value_) {
        if (token.type == Token::Type::kNumber) {
            stack_.push(token);
        } else {
            int b;
            switch (token.type) {
                case Token::Type::kUnaryMinus:
                    stack_.top().value = 0 - stack_.top().value;
                    break;
                case Token::Type::kUnaryPlus:
                    break;
                case Token::Type::kPlus:
                    b = stack_.top().value;
                    stack_.pop();
                    stack_.top().value += b;
                    break;
                case Token::Type::kMinus:
                    b = stack_.top().value;
                    stack_.pop();
                    stack_.top().value -= b;
                    break;
                case Token::Type::kMultiplication:
                    b = stack_.top().value;
                    stack_.pop();
                    stack_.top().value = multiply(stack_.top().value, b);
                    break;
                case Token::Type::kDivision:
                    b = stack_.top().value;
                    stack_.pop();
                    stack_.top().value = divide(stack_.top().value, b);
                    break;
            }
        }
    }
    return stack_.top().value;
}

void ReversePolishNotation::BuildRpn() {
    while (!stack_.empty()) {
        if (stack_.top().type == Token::Type::kOpenBracket) {
            throw InvalidSyntaxException("invalid parentheses");
        }
        output_value_.push_back(stack_.top());
        stack_.pop();
    }
}

int ReversePolishNotation::GetOperatorPrecedence(const Token::Type type) {
    static const std::unordered_map<Token::Type, int> kPrecedenceMap{
            {Token::Type::kMinus,          1},
            {Token::Type::kPlus,           1},
            {Token::Type::kDivision,       2},
            {Token::Type::kMultiplication, 2},
            {Token::Type::kUnaryPlus,      3},
            {Token::Type::kUnaryMinus,     3},
    };
    auto it = kPrecedenceMap.find(type);
    if (it != kPrecedenceMap.end()) {
        return it->second;
    }
    return 0;
}

void ReversePolishNotation::StackPopByPrecedence(int precedence) {
    while (!stack_.empty() && GetOperatorPrecedence(stack_.top().type) >= precedence) {
        output_value_.push_back(stack_.top());
        stack_.pop();
    }
}

std::vector<Token> ReversePolishNotation::GetRpn() {
    BuildRpn();
    return output_value_;
}

std::string format_number(int n) {
    const std::string kDelimiter{"."};
    std::string res = std::to_string(n / 100);
    int tail = n % 100;
    if (tail != 0) {
        if (tail % 10 == 0) {
            res += kDelimiter + std::to_string(tail / 10);
        } else {
            res += kDelimiter;
            if (tail < 10) {
                res += "0";
            }
            res += std::to_string(tail);
        }
    }
    return res;
}

ReversePolishNotation build_reverse_polish_notation(const std::string& expression) {
    size_t i = 0;
    std::optional<Token::Type> last_token_type;
    ReversePolishNotation builder;
    while (i < expression.size()) {
        const char& c = expression[i];
        Token token;
        if (std::isdigit(c)) {
            auto parse_result = parse_number(expression, i);
            token.type = Token::Type::kNumber;
            token.value = parse_result.first;
            i = parse_result.second;
        } else {
            token.type = parse_non_number_token_type(c, last_token_type);
            i++;
        }
        check_token_before_throw(token.type, last_token_type);
        last_token_type = token.type;
        builder.AddToken(token);
    }
    check_token_syntax_throw(*last_token_type, kAllowedInEnd);
    return builder;
}

int calculate(std::string&& expression) {
    // trim whitespaces
    expression.erase(
            std::remove_if(expression.begin(), expression.end(), [](const char &c) { return std::isspace(c); }),
            expression.end());
    if (expression.empty()) throw InvalidSyntaxException("empty input");

    // parse raw string and form result into a Reverse Polish notation
    auto builder = build_reverse_polish_notation(expression);

    // calculate expression
    return builder.Calculate();
}

}