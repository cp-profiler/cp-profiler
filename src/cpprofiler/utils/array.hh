#ifndef CPPROFILER_UTILS_ARRAY_HH
#define CPPROFILER_UTILS_ARRAY_HH

#include <new>
#include <initializer_list>
#include <algorithm>

namespace cpprofiler { namespace utils {

template<typename T>
class Array {

    int m_size;
    T* m_data;

public:

    /// Creates an uninitialized array of size `size`
    Array(int size);
    Array(std::initializer_list<T> init_list);
    Array();
    ~Array();

    Array(const Array& other) = delete;
    Array& operator=(const Array& other) = delete;

    T& operator[](int pos);

    const T& operator[](int pos) const;

    int size() const;

};

template<typename T>
Array<T>::Array(int size) : m_size(size) {

    auto size_in_bytes = size * sizeof(T);
    // m_data = reinterpret_cast<T*>(new char[size_in_bytes]);
    m_data = new T[size_in_bytes];
}

template<typename T>
Array<T>::Array(std::initializer_list<T> init_list)
: m_size(init_list.size()) {
    auto size_in_bytes = m_size * sizeof(T);
    m_data = reinterpret_cast<T*>(new char[size_in_bytes]);

    auto i = 0u;
    for (auto&& el : init_list) {
        m_data[i] = el;
        ++i;
    }

    // std::insert(std::begin(m_data), init_list.begin(), init_list.end());
}

template<typename T>
Array<T>::Array() : m_size(0) {
}

template<typename T>
Array<T>::~Array() {
    delete[] m_data;
}

template<typename T>
T& Array<T>::operator[](int pos) {
    return m_data[pos];
}

template<typename T>
const T& Array<T>::operator[](int pos) const {
    return m_data[pos];
}

template<typename T>
int Array<T>::size() const {
    return m_size;
}

}}


#endif