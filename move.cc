#include <print>



template <typename T>
struct RemoveReference {
    using Type = T;
};

template <typename T>
struct RemoveReference<T&> {
    using Type = T;
};

template <typename T>
struct RemoveReference<T&&> {
    using Type = T;
};

template <typename T>
using RemoveReferenceT = typename RemoveReference<T>::Type;

template <typename T>
[[nodiscard]] static constexpr inline
RemoveReferenceT<T>&& my_move(T&& x) {
    return static_cast<RemoveReferenceT<T>&&>(x);
}

int main() {

    static_assert(std::is_same_v<decltype(my_move(1)), int&&>);

    int x = 2;
    static_assert(std::is_same_v<decltype(my_move(x)), int&&>);

}
