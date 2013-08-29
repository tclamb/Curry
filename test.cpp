#include <iostream>
#include "Curry.h"

int add(int a, int b) {
    return a+b;
}

int add4(int a, int b, int c, int d) {
    return a + b + c + d;
}

int main() {
    auto mult3 = curry([](int x, int y, int z) { return x*y*z; });
    auto something = [](int a, int b, int c, int d, int e, int f)
                     { return a*b*c+d*e*f; };

    std::cout << "               14 + 1 = "
              << curry(add)(14)(1) << std::endl
              << "            1 * 3 * 5 = "
              << mult3(1)(3)(5) << std::endl
              << "        1 + 2 + 4 + 8 = "
              << curry(add4)(1)(2)(4)(8) << std::endl
              << "    1 + 2 + 3 + 4 + 5 = "
              << curry(
                       [](int a, int b, int c, int d, int e)
                       { return a+b+c+d+e; }
                      )(1)(2)(3)(4)(5) << std::endl
              << "1 * 2 * 3 + 1 * 3 * 3 = "
              << curry(something)(1)(2)(3)(1)(3)(3)
              << std::endl;

    return 0;
}

