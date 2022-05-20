#include <cmath>

#include "instructions.h"

void fillInstructionSet(Instructions::Set& set) {
    auto minus = [](double a, double b) -> double { return a - b; };
    auto add = [](double a, double b) -> double { return a + b; };
    auto mult = [](double a, double b) -> double { return a * b; };
    auto div = [](double a, double b) -> double { return a / b; };
    auto ln = [](double a) -> double { return std::log(a); };
    auto exp = [](double a) -> double { return std::exp(a); };
    auto cos = [](double a) -> double { return std::cos(a); };
    auto lt = [](double a, double b) -> double { return a < b ? a : b; };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus, "$0 = $1 - $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add, "$0 = $1 + $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(mult, "$0 = $1 * $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(div, "$0 = $1 / $2;")));
    set.add(*(new Instructions::LambdaInstruction<double>(exp, "$0 = exp($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(ln, "$0 = log($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(cos, "$0 = cos($1);")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(lt, "$0 = $1 < $2 ? $1 : $2;")));
}