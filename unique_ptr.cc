#include <cassert>
#include <print>
#include <utility>

template <typename T>
using DefaultDeleter = decltype([](T* ptr) { delete ptr; });

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
    T* m_ptr = nullptr;
    Deleter m_del;

public:
    UniquePtr() = default;

    explicit UniquePtr(T* ptr) : m_ptr(ptr) { }

    ~UniquePtr() {
        m_del(m_ptr);
    }

    UniquePtr(UniquePtr const&) = delete;
    UniquePtr& operator=(UniquePtr const&) = delete;

    UniquePtr(UniquePtr&& other)
    : m_ptr(std::exchange(other.m_ptr, nullptr))
    { }

    UniquePtr& operator=(UniquePtr&& other) {
        if (&other != this) {
            reset(other.release());
        }
        return *this;
    }

    void swap(UniquePtr& other) {
        std::swap(other.m_ptr, m_ptr);
    }

    T* release() {
        return std::exchange(m_ptr, nullptr);
    }

    void reset(T* ptr) {
        m_del(m_ptr);
        m_ptr = ptr;
    }

    [[nodiscard]] T* get() {
        return m_ptr;
    }

    T operator*() {
        return *get();
    }

};

template <typename T, typename... Args>
[[nodiscard]] static constexpr inline
auto my_make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

int main() {

    UniquePtr foo = my_make_unique<int>(1);
    assert(*foo == 1);

    UniquePtr bar(std::move(foo));
    assert(foo.get() == nullptr);
    assert(*bar == 1);

    UniquePtr baz = my_make_unique<int>(2);
    baz = std::move(bar);
    assert(*baz == 1);
    assert(bar.get() == nullptr);

    int x = 45;
    UniquePtr<int, decltype([](int*){})> qux(&x);

}
