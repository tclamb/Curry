#ifndef _Curry_curry_h_
#define _Curry_curry_h_

#include <functional>

namespace
{
    /* SFNIAE helper struct for determining return type of curry */
    template<typename T> struct curry_t {};

    template<typename R, typename A>
    struct curry_t<R(A)>
    {
        using type = std::function<R(A)>;
    };

    template<typename R, typename Head, typename... Tail>
    struct curry_t<R(Head, Tail...)>
    {
        using type = std::function<typename curry_t<R(Tail...)>::type(Head)>;
    };
}

/* terminate recursion when passed a single-argument function */
template<typename R, typename A>
std::function<R(A)> curry(std::function<R(A)> f) {
    return f;
}

/* slip on one layer at a time */
template<typename R, typename Head, typename Body, typename... Tail>
auto curry(std::function<R(Head, Body, Tail...)> f)
-> typename curry_t<R(Head, Body, Tail...)>::type {
    return [f](Head&& h) {
        return curry(
            std::function<R(Body, Tail...)>{
                [f, h](Body b, Tail... t) {
                    return f(h, b, t...);
                }
            }
        );
    };
}

/* specialization for zero argument functions */
template<typename R>
std::function<R()> curry(std::function<R()> f) {
    return f;
}

/* specialization for function pointers */
template<typename R, typename... A>
auto curry(R(*f)(A...))
-> decltype(curry(std::function<R(A...)>(f))) {
    return curry(std::function<R(A...)>(f));
}

namespace
{
    /* SFNIAE helper struct for type extraction of lambdas */
    template<typename T> struct remove_class {};
    template<typename R, typename C, typename... A>
        struct remove_class<R(C::*)(A...)> { using type = R(A...); };
    template<typename R, typename C, typename... A>
        struct remove_class<R(C::*)(A...) const> { using type = R(A...); };
}

/* specialization for lambda functions and other functors */
template<typename F>
auto curry(F f) 
-> decltype(curry(std::function<typename remove_class<
    decltype(&std::remove_reference<F>::type::operator())
    >::type>(f))) {
    return curry(std::function<typename remove_class<
            decltype(&std::remove_reference<F>::type::operator())
            >::type>(f));
}

namespace
{
    /* SFINAE helper struct for determining return type of uncurry */
    template<typename T> struct uncurry_t {};

    template<typename R, typename... A>
    struct uncurry_t<std::function<R(A...)>>
    {
        using type = std::function<R(A...)>;
    };

    template<typename R, typename... Head, typename Tail>
    struct uncurry_t<std::function<std::function<R(Tail)>(Head...)>>
    {
        using type = typename uncurry_t<std::function<R(Head..., Tail)>>::type;
    };
}

/* terminate recursion when return type is not a function */
template<typename R, typename... A>
std::function<R(A...)> uncurry(std::function<R(A...)> f) {
    return f;
}

/* peel off one layer at a time */
template<typename R, typename... Head, typename Tail>
auto uncurry(std::function<std::function<R(Tail)>(Head...)> f)
-> typename uncurry_t<std::function<std::function<R(Tail)>(Head...)>>::type {
    return uncurry(
        std::function<R(Head..., Tail)>{
            [f](Head... h, Tail t) {
                return f(h...)(t);
            }
        }
    );
}

/* specialization for function pointers */
template<typename R, typename... A>
auto uncurry(R(*f)(A...))
-> decltype(uncurry(std::function<R(A...)>(f))) {
    return uncurry(std::function<R(A...)>(f));
}

/* specialization for lambda functions and other functors */
template<typename F>
auto uncurry(F f)
-> decltype(uncurry(std::function<typename remove_class<
    decltype(&std::remove_reference<F>::type::operator())
    >::type>(f))) {
    return uncurry(std::function<typename remove_class<
            decltype(&std::remove_reference<F>::type::operator())
            >::type>(f));
}

#endif
