#include <cassert>
#include <print>
#include <vector>
#include <cstdlib>



template <typename T>
class Vector {
    std::size_t m_len = 0;
    std::size_t m_cap = 5;
    T* m_items;
    using Iterator = T*;
    using ConstIterator = T const*;

public:
    // not calling new[], as T might not have a default ctor
    Vector() : m_items(static_cast<T*>(std::malloc(sizeof(T) * m_cap))) { }

    ~Vector() {
        call_all_dtors();
        free(m_items);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (m_len >= m_cap) {
            m_cap *= 2;
            m_items = static_cast<T*>(realloc(m_items, sizeof(T) * m_cap));
        }

        // not using assignment, as T might not implement operator=()
        new (&m_items[m_len++]) T(std::forward<Args>(args)...);
    }

    void push_back(T const& x) {
        emplace_back(x);
    }

    void clear() {
        call_all_dtors();
        m_len = 0;
    }

    [[nodiscard]] bool empty() const {
        return m_len == 0;
    }

    [[nodiscard]] std::size_t size() const {
        return m_len;
    }

    [[nodiscard]] std::size_t capacity() const {
        return m_cap;
    }

    [[nodiscard]] T* data() {
        return m_items;
    }

    [[nodiscard]] Iterator begin() {
        return m_items;
    }

    [[nodiscard]] Iterator end() {
        return m_items + m_len;
    }

    [[nodiscard]] ConstIterator cbegin() const {
        return m_items;
    }

    [[nodiscard]] ConstIterator cend() const {
        return m_items + m_len;
    }

    [[nodiscard]] T& front() {
        return *begin();
    }

    [[nodiscard]] T& back() {
        return end()[-1]; // nice syntax
    }

private:
    void call_all_dtors() {
        for (std::size_t i=0; i < m_len; ++i)
            m_items[i].~T();
    }

};



int main() {

    struct S {
        int m_x;
        S(int x) : m_x(x) { }
        S() = delete;
        S& operator=(S const&) = delete;
    };

    std::vector<int> xs;

    Vector<int> a;
    Vector<std::string> b;
    Vector<S> c;

    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

    assert(a.front() == 1);
    assert(a.back() == 3);
    assert(a.size() == 3);
    assert(!a.empty());

    a.clear();

    assert(a.size() == 0);
    assert(a.empty());

    for (auto& elem : a) {
    }


    return 0;
}
