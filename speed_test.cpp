#include "curry.h"

int main()
{
    auto f = [](int x, int y) { return x+y; };

#ifdef SLOW
    auto ff = uncurry(curry(f));
#else
    auto ff = f;
#endif

    for(int i = 0; i < 10000; ++i)
        for(int j = 0; j < 10000; ++j)
            ff(i, j);

    return 0;
}
