#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <type_traits>

template <std::size_t I, typename T>
struct TupleElement {
    T m_value;
};

template <std::size_t I, typename... Ts>
class TupleImpl;

template <std::size_t I>
class TupleImpl<I> { };

template <std::size_t I, typename Head, typename... Tail>
class TupleImpl<I, Head, Tail...>
    : public TupleElement<I, Head>
    , public TupleImpl<I+1, Tail...>
{

public:
    TupleImpl(Head hd, Tail ...tl)
        : TupleElement<I, Head>(hd)
        , TupleImpl<I+1, Tail...>(tl...)
    { }

};

template <typename... Ts>
class Tuple {
    TupleImpl<0, Ts...> m_tuple;

public:
    Tuple(Ts ...tuple) : m_tuple(tuple...) { }

    template <std::size_t I, typename... Tail>
    constexpr friend
    std::tuple_element_t<I, Tuple<Tail...>>& get(Tuple<Tail...>& tuple);

};

// structured bindings requirements:
// - std::tuple_size
// - std::tuple_element
// - get()

template <std::size_t I, typename... Tail>
[[nodiscard]] static constexpr inline
std::tuple_element_t<I, Tuple<Tail...>>& get(Tuple<Tail...>& tuple) {
    using ElemType = std::tuple_element_t<I, Tuple<Tail...>>;
    return tuple.m_tuple.TupleElement<I, ElemType>::m_value;
}

template <typename... Ts>
struct std::tuple_size<Tuple<Ts...>>
: public std::integral_constant<std::size_t, sizeof...(Ts)>
{ };

#if 1

template <std::size_t I, typename Head, typename... Tail>
struct std::tuple_element<I, Tuple<Head, Tail...>>
    : public std::tuple_element<I-1, Tuple<Tail...>>
{
    static_assert(I < std::tuple_size_v<Tuple<Head, Tail...>>);
};

template <typename Head, typename... Tail>
struct std::tuple_element<0, Tuple<Head, Tail...>> {
    using type = Head;
};

#else

// NOTE: C++26
template <std::size_t I, typename... Ts>
struct std::tuple_element<I, Tuple<Ts...>> {
    using type = Ts...[I];
};

#endif

int main() {

    struct S { int m_x; };

    Tuple tuple(1, 2.0f, true, std::string("foo"), S(3));

    static_assert(std::tuple_size_v<decltype(tuple)> == 5);

    static_assert(std::is_same_v<std::tuple_element_t<0, decltype(tuple)>, int>);
    static_assert(std::is_same_v<std::tuple_element_t<1, decltype(tuple)>, float>);
    static_assert(std::is_same_v<std::tuple_element_t<2, decltype(tuple)>, bool>);
    static_assert(std::is_same_v<std::tuple_element_t<3, decltype(tuple)>, std::string>);
    static_assert(std::is_same_v<std::tuple_element_t<4, decltype(tuple)>, S>);

    assert(get<0>(tuple) == 1);
    assert(get<1>(tuple) == 2.0f);
    assert(get<2>(tuple) == true);

    auto &[a, b, c, d, e] = tuple;

    assert(a == 1);
    assert(b == 2.0f);
    assert(c == true);
    assert(d == "foo");
    assert(e.m_x == 3);

    return 0;
}
