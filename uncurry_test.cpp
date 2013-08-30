#include <iostream>
#include "Curry.h"

auto six(int f) {
    return curry([f](int a, int b, int c, int d, int e){
        return a + b + c - d - e - f;
    });
}

int main() {
    auto zero = curry([](){ return 0; });
    auto id   = curry([](int a){ return a; });
    auto add2 = curry([](int a, int b){ return a+b; });
    auto add3 = curry([](int a, int b, int c){ return a+b+c; });
    auto add4 = curry([](int a, int b, int c, int d){ return a+b+c+d; });

    std::cout << "      nothing = " << uncurry(zero)() << std::endl
              << "            1 = " << uncurry(id  )(1) << std::endl
              << "        1 + 1 = " << uncurry(add2)(1, 1) << std::endl
              << "    1 + 1 + 1 = " << uncurry(add3)(1, 1, 1) << std::endl
              << "1 + 1 + 1 + 1 = " << uncurry(add4)(1, 1, 1, 1) << std::endl
              << " 1+1+1-1-1-1  = " << uncurry(six)(1, 1, 1, 1, 1, 1) << std::endl;

    return 0;
}
