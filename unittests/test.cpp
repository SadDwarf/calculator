#include "../src/solution.hpp"
#include <gtest/gtest.h>


std::string build_rpn_string(std::vector<solution::Token> tokens) {
    using Type = solution::Token::Type;
    std::string res;
    for (const auto& token : tokens) {
        switch (token.type) {
            case Type::kNumber:
                res += solution::format_number(token.value);
                break;
            case Type::kPlus:
            case Type::kUnaryPlus:
                res += "+";
                break;
            case Type::kMinus:
            case Type::kUnaryMinus:
                res += "-";
                break;
            case Type::kMultiplication:
                res += "*";
                break;
            case Type::kDivision:
                res += "/";
                break;
            case Type::kOpenBracket:
                res += "(";
                break;
            case Type::kCloseBracket:
                res += ")";
                break;
        }
        res += " ";
    }
    if (!res.empty()) res.pop_back();
    return res;
}

TEST(CalculationTest, SimpleOperations) {
    ASSERT_EQ(solution::calculate("1+1"), 200);

    ASSERT_EQ(solution::calculate("1+2+3+5"), 1100);

    ASSERT_EQ(solution::calculate("10/3"), 333);

    ASSERT_EQ(solution::calculate("11/3"), 367);

    ASSERT_EQ(solution::calculate("2 * 5"), 1000);

    ASSERT_EQ(solution::calculate("42"), 4200);

    ASSERT_EQ(solution::calculate("-25-25"), -5000);

    ASSERT_EQ(solution::calculate("+25 +25"), 5000);

    ASSERT_EQ(solution::calculate("0.3*0.3"), 9);

    ASSERT_EQ(solution::calculate("0.3*0.03"), 1);
}

TEST(CalculationTest, Parentheses) {
    ASSERT_EQ(solution::calculate("(2 + 2) * 2"), 800);

    ASSERT_EQ(solution::calculate("((1 + 2) * 3) / 2"), 450);

    ASSERT_EQ(solution::calculate("(3 * 10) / (3 - 2)"), 3000);

    ASSERT_EQ(solution::calculate("-9-(9)"), -1800);

    ASSERT_THROW(solution::calculate("()"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("(10+2 * (2 - 1)"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1+1("), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1+2)"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("(1+1)(2+2)"), solution::InvalidSyntaxException);
}

TEST(CalculationTest, Errors) {
    ASSERT_THROW(solution::calculate("2++1"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("*2 - 1"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("/ 2 - 1"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1-1/"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1-1*"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1-1+"), solution::InvalidSyntaxException);

    ASSERT_THROW(solution::calculate("1-1-"), solution::InvalidSyntaxException);
}

TEST(BuildRpn, BuildRpn) {
    // build_reverse_polish_notation doesn't trim whitespaces from input string

    auto rpn = solution::build_reverse_polish_notation("(1+2)*4+3");
    ASSERT_EQ(build_rpn_string(rpn.GetRpn()), "1 2 + 4 * 3 +");

    auto rpn2 = solution::build_reverse_polish_notation("(1+2)/(5-2)");
    ASSERT_EQ(build_rpn_string(rpn2.GetRpn()), "1 2 + 5 2 - /");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}