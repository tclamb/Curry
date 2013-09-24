#include <type_traits>
#include <utility>
 
// functions, functors, lambdas, etc.
template<
    class F, class... Args,
    class = typename std::enable_if<!std::is_member_function_pointer<F>::value>::type,
    class = typename std::enable_if<!std::is_member_object_pointer<F>::value>::type
    >
auto eval(F&& f, Args&&... args) -> decltype(f(std::forward<Args>(args)...))
{
    return f(std::forward<Args>(args)...);
}
 
// const member function
template<class R, class C, class P, class... Args>
auto eval(R(C::*f)() const, P&& p, Args&&... args) -> R
{
    return (*p.*f)(std::forward<Args>(args)...);
}
 
template<class R, class C, class... Args>
auto eval(R(C::*f)() const, C& c, Args&&... args) -> R
{
    return (c.*f)(std::forward<Args>(args)...);
}
 
// non-const member function
template<class R, class C, class P, class... Args>
auto eval(R(C::*f)(), P&& p, Args&&... args) -> R
{
    return (*p.*f)(std::forward<Args>(args)...);
}
 
// member object
template<class R, class C>
auto eval(R(C::*m), const C& c) -> const R&
{
    return c.*m;
}
 
template<class R, class C>
auto eval(R(C::*m), C& c) -> R&
{
    return c.*m;
}

template<size_t index>
struct tuple_eval_helper
{
    template<class Func,class TArgs,class... Args>
    static auto evaluate(Func&& f, TArgs&& t_args, Args&&... args)
        -> decltype(tuple_eval_helper<index-1>::evaluate(
            std::forward<Func>(f),
            std::forward<TArgs>(t_args),
            std::get<index>(std::forward<TArgs>(t_args)),
            std::forward<Args>(args)...
            ))
    {
        return tuple_eval_helper<index-1>::evaluate(
            std::forward<Func>(f),
            std::forward<TArgs>(t_args),
            std::get<index>(std::forward<TArgs>(t_args)),
            std::forward<Args>(args)...
            );
    }
};
 
template<>
struct tuple_eval_helper<0>
{
    template<class Func,class TArgs,class... Args>
    static auto evaluate(Func&& f, TArgs&& t_args, Args&&... args)
        -> decltype(eval(
            std::forward<Func>(f),
            std::get<0>(std::forward<TArgs>(t_args)),
            std::forward<Args>(args)...
            ))
    {
        return eval(
            std::forward<Func>(f),
            std::get<0>(std::forward<TArgs>(t_args)),
            std::forward<Args>(args)...
            );
    }
};
 
template<class Func,class TArgs>
auto tuple_eval(Func&& f, TArgs&& t_args)
    -> decltype(tuple_eval_helper<std::tuple_size<
            typename std::remove_reference<TArgs>::type>::value-1>::evaluate(
        std::forward<Func>(f),
        std::forward<TArgs>(t_args)
        ))
{
    return tuple_eval_helper<std::tuple_size<
            typename std::remove_reference<TArgs>::type>::value-1>::evaluate(
        std::forward<Func>(f),
        std::forward<TArgs>(t_args)
        );
}
