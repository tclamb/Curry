#ifndef _Curry_curry_h_
#define _Curry_curry_h_

#include <functional>
#include <tuple>
#include "eval.h"

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

    template <size_t... n>
    struct ct_integers_list {
        template <size_t m>
        struct push_back
        {
            using type = ct_integers_list<n..., m>;
        };
    };

    template <size_t max>
    struct ct_iota_1
    {
        using type = typename ct_iota_1<max-1>::type::template push_back<max>::type;
    };

    template <>
    struct ct_iota_1<0>
    {
        using type = ct_integers_list<0>;
    };

    template<typename T, typename U> struct tuple_subset {};

    template<typename Tuple, size_t... Indices>
    struct tuple_subset<Tuple, ct_integers_list<Indices...>>
    {
        using type = std::tuple<typename std::tuple_element<Indices, Tuple>::type...>;
    };

    template<typename T, std::size_t N, typename> struct curry_f {};

    template<typename R, typename... Args>
    struct curry_f<R(Args...), 0, typename std::enable_if<1 == sizeof...(Args)>::type>
    {
        using ArgsTuple = std::tuple<Args...>;
        constexpr static std::size_t nArgs = std::tuple_size<ArgsTuple>::value;
        //using NextArg = typename std::tuple_element<N, ArgsTuple>::type;

        std::function<R(Args...)> f;

        template<typename NextArg>
        R operator()(NextArg n) {
            return f(n);
        }
    };

    template<typename R, typename... Args>
    struct curry_f<R(Args...), 0, typename std::enable_if<1 < sizeof...(Args)>::type>
    {
        using ArgsTuple = std::tuple<Args...>;
        constexpr static std::size_t nArgs = std::tuple_size<ArgsTuple>::value;
        //using NextArg = typename std::tuple_element<N, ArgsTuple>::type;

        std::function<R(Args...)> f;

        template<typename NextArg>
        curry_f<R(Args...), 1, void> operator()(NextArg n) {
            return {f, std::forward_as_tuple(n)};
        }
    };

    template<typename R, typename... Args, std::size_t N>
    struct curry_f<R(Args...), N, typename std::enable_if<N+1 < sizeof...(Args) && N != 0>::type>
    {
        using ArgsTuple = std::tuple<Args...>;
        constexpr static std::size_t nArgs = std::tuple_size<ArgsTuple>::value;
        //using NextArg = typename std::tuple_element<N, ArgsTuple>::type;
        using SoFarTuple = typename tuple_subset<ArgsTuple, typename ct_iota_1<N-1>::type>::type;

        std::function<R(Args...)> f;
        SoFarTuple SoFar;

        template<typename NextArg>
        curry_f<R(Args...), N+1, void> operator()(NextArg n) {
            return {f, std::tuple_cat(SoFar, std::forward_as_tuple(n))};
        }
    };
    
    template<typename R, typename... Args, std::size_t N>
    struct curry_f<R(Args...), N, typename std::enable_if<N+1 == sizeof...(Args) && N != 0>::type>
    {
        using ArgsTuple = std::tuple<Args...>;
        constexpr static std::size_t nArgs = std::tuple_size<ArgsTuple>::value;
        //using NextArg = typename std::tuple_element<N, ArgsTuple>::type;
        using SoFarTuple = typename tuple_subset<ArgsTuple, typename ct_iota_1<N-1>::type>::type;

        std::function<R(Args...)> f;
        SoFarTuple SoFar;

        template<typename NextArg>
        R operator()(NextArg n) {
            return tuple_eval(f, std::tuple_cat(SoFar, std::forward_as_tuple(n)));
        }
    };
}

template<typename R>
std::function<R()> make_curried(std::function<R()> f) {
    return f;
}

template<typename R, typename... Args>
curry_f<R(Args...), 0, void> make_curried(std::function<R(Args...)> f) {
    return {f};
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
-> typename curry_t<R(A...)>::type {
    return curry(std::function<R(A...)>{f});
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
-> typename curry_t<typename remove_class<
    decltype(&std::remove_reference<F>::type::operator())
    >::type>::type {
    return curry(std::function<typename remove_class<
            decltype(&std::remove_reference<F>::type::operator())
            >::type>{f});
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
-> typename uncurry_t<R(A...)>::type {
    return uncurry(std::function<R(A...)>{f});
}

/* specialization for lambda functions and other functors */
template<typename F>
auto uncurry(F f)
-> typename uncurry_t<typename remove_class<
    decltype(&std::remove_reference<F>::type::operator())
    >::type>::type {
    return uncurry(std::function<typename remove_class<
            decltype(&std::remove_reference<F>::type::operator())
            >::type>{f});
}

#endif
