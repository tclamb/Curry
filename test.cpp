#include <iostream>
#include "curry.h"

struct vec
{
    float x, y, z;

    vec(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
};

float dot_product(vec a, vec b) {
    return a.x * b.x
         + a.y * b.y
         + a.z * b.z;
}

vec cross_product(vec a, vec b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

float scalar_triple_product(vec a, vec b, vec c) {
    return dot_product(a, cross_product(b, c));
}

int main() {
    auto add = [](int a, int b, int c){ return a + b + c; };
    auto curried_add = curry(add);
    auto add2 = uncurry(curried_add(0));
    auto plus_one = curried_add(0)(1);

    
    std::cout << "0 + 1 = " << plus_one(0) << std::endl
              << "1 + 1 = " << plus_one(1) << std::endl
              << "2 + 2 = " << add2(2, 2) << std::endl;
    
    
    auto c_stp = curry(&scalar_triple_product);
    auto xcomp_of_cross = uncurry(c_stp({1,0,0}));
    auto c_magsq = curry([](vec a){ return a.x*a.x + a.y*a.y + a.z*a.z; });
    
    std::cout << "(1, 0, 0) dot (0, 1, 0) cross (0, 0, 1) = "
              << "(1, 0, 0) dot (1, 0, 0) = "
              << c_stp({1,0,0})({0,1,0})({0,0,1}) << std::endl
              << "[(10, 1, 0) cross (9.251, 0, 4)]_x = "
              << xcomp_of_cross({10, 1, 0}, {9.251, 0, 4}) << std::endl //should be 4
              << "|(1, 0, 0)|^2 = "
              << c_magsq({1,0,0}) << std::endl;

    return 0;
}
