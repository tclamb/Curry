function currying and uncurrying for C++
currently reimplementing using tuples for massive speed gains
as is, not very practical

example usage:
```cpp
#include <iostream>
#include "curry.h"

struct vec { float x, y, z; };

float dot_product(vec a, vec b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

vec cross_product(vec a, vec b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

float scalar_triple_product(vec a, vec b, vec c) { return dot_product(a, cross_product(b, c)); }

int main() {
    auto curried_stp = curry(&scalar_triple_product);
    auto x_of_cross_product = uncurry(curried_stp({1,0,0}));

    std::cout << "(1, 0, 0) dot (0, 1, 0) cross (0, 0, 1) = "
                  << "(1, 0, 0) dot (1, 0, 0) = "
                  << curried_stp({1,0,0})({0,1,0})({0,0,1}) << std::endl
              << "[(10, 1, 0) cross (9.251, 0, 4)]_x = "
                  << x_of_cross_product({10, 1, 0}, {9.251, 0, 4}) << std::endl;

    return 0;
}
```

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/tclamb/curry/trend.png)](https://bitdeli.com/free "Bitdeli Badge")
