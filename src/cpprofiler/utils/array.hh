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

    Array(const Array& other);
    Array& operator=(const Array& other) = delete;

    Array(Array&& other);

    T& operator[](int pos);

    const T& operator[](int pos) const;

    int size() const;

};

template<typename T>
Array<T>::Array(int size) : m_size(size) {
    m_data = new T[size];
}

template<typename T>
Array<T>::Array(std::initializer_list<T> init_list)
: m_size(init_list.size()) {
    m_data = new T[m_size];

    int i = 0;
    for (auto&& el : init_list) {
        m_data[i] = el;
        ++i;
    }
}

template<typename T>
Array<T>::Array() : m_size(0) {
}

template<typename T>
Array<T>::Array(const Array& other) : m_size(other.size()) {
    m_data = new T[m_size];

    for (auto i = 0; i < m_size; ++i) {
        m_data[i] = other[i];
    }
}

template<typename T>
Array<T>::Array(Array&& other) : m_size(other.size()) {
    m_data = other.m_data;
    other.m_size = 0;
    other.m_data = nullptr;
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