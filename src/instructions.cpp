#include <cmath>

#include "instructions.h"
#include "Tetris.h"

void fillInstructionSet(Instructions::Set& set) {
    auto minus = [](double a, double b) -> double { return a - b; };
    auto add = [](double a, double b) -> double { return a + b; };
    auto mult = [](double a, double b) -> double { return a * b; };
    auto div = [](double a, double b) -> double { return a / b; };
    auto ln = [](double a) -> double { return std::log(a); };
    auto exp = [](double a) -> double { return std::exp(a); };
    auto cos = [](double a) -> double { return std::cos(a); };
    auto lt = [](double a, double b) -> double { return a < b ? a : b; };

    auto lineDensity = [](const double line[10]) -> double {
        int count = 0;
        for(int i = 0; i < 10; i++){
            if(line[i] > 0)
                count++;
        }

        return (double)count / 10.0;
    };

    auto columnDensity = [](const double col[20][1]) -> double {
        int count = 0;
        for(int i = 0; i < 20; i++){
            if(col[i][0] > 0)
                count++;
        }

        return (double)count / 20.0;
    };

    // For one column, computes the number of consecutive blocs starting from the bottom of the screen
    auto columnStack =  [](const double col[20][1]) -> double {
        int count = 0;
        for(int i = 19; i >= 0 && col[i][0] > 0; i--)
            count++;

        return count;
    };

    auto multByConst = [](double a, Data::Constant c) -> double { return a*(double)c; };


    set.add(*(new Instructions::LambdaInstruction<double, double>(minus, "$0 = $1 - $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add, "$0 = $1 + $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(mult, "$0 = $1 * $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(div, "$0 = $1 / $2;")));
    set.add(*(new Instructions::LambdaInstruction<double>(exp, "$0 = exp($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(ln, "$0 = log($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(cos, "$0 = cos($1);")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(lt, "$0 = $1 < $2 ? $1 : $2;")));

    set.add(*(new Instructions::LambdaInstruction<const double[10]>(lineDensity)));
    set.add(*(new Instructions::LambdaInstruction<const double [20][1]>(columnDensity)));
    set.add(*(new Instructions::LambdaInstruction<double, Data::Constant>(multByConst)));
    set.add(*(new Instructions::LambdaInstruction<const double [20][1]>(columnStack)));
}