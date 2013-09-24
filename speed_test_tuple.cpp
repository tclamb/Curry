#include "curry.h"

int main()
{
    std::function<int(int,int)> f = [](int x, int y) { return x+y; };

#ifdef SLOW
    auto ff = make_curried(f);
#else
    auto ff = curry(f);
#endif

    for(int i = 0; i < 10000; ++i)
        for(int j = 0; j < 10000; ++j)
            ff(i)(j);

    return 0;
}
