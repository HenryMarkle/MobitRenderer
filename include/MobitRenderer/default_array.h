#pragma once

#include <memory>
#include <initializer_list>

namespace mr {

template<typename T>
class default_array {

private:

    T *arr;
    T default_value;
    size_t _size;

public:

    T& operator[](size_t) noexcept;
    T& at(size_t, T&&) noexcept;
    void resize(size_t) noexcept;
    void resize(size_t, T&&) noexcept;
    size_t size() const noexcept;
    void fill() noexcept;
    void fill(T&&) noexcept;

    default_array(size_t size);
    default_array(size_t size, T &&default_value);
    default_array(std::initializer_list<T> values);
    ~default_array();

};

template<typename T>
T& default_array<T>::operator[](size_t index) noexcept {
    if (index >= this->_size) return default_value;
    return arr[index];
}

template<typename T>
T& default_array<T>::at(size_t index, T &&default_value) noexcept {
    if (index >= this->_size) return default_value;
    return arr[index];
}

template<typename T>
void default_array<T>::resize(size_t size) noexcept {
    if (size == this->_size) return;

    size_t min_len = this->_size > size ? size : this->_size;

    T *new_arr = new T[size];
    std::fill(new_arr, new_arr + size, default_value);

    for (size_t x = 0; x < min_len; x++) new_arr[x] = arr[x];

    delete[] arr;

    arr = new_arr;
    _size = size;
}

template<typename T>
void default_array<T>::resize(size_t size, T &&default_value) noexcept {
    if (size == this->_size) return;

    size_t min_len = this->_size > size ? size : this->_size;

    T *new_arr = new T[size];
    std::fill(new_arr, new_arr + size, default_value);

    for (size_t x = 0; x < min_len; x++) new_arr[x] = arr[x];

    delete[] arr;

    arr = new_arr;
    _size = size;
}

template<typename T>
size_t default_array<T>::size() const noexcept { return this->_size; }

template<typename T>
void default_array<T>::fill() noexcept {
    std::fill(arr, arr + _size, default_value);
}

template<typename T>
void default_array<T>::fill(T &&value) noexcept {
    std::fill(arr, arr + _size, value);
}

template<typename T>
default_array<T>::default_array(size_t size) : _size(size), default_value(T()), arr(nullptr) {
    arr = new T[size];
    std::fill(arr, arr + size, default_value);
};

template<typename T>
default_array<T>::default_array(size_t size, T &&default_value) : 
    _size(size), 
    default_value(default_value), 
    arr(nullptr) {

    arr = new T[size];
    std::fill(arr, arr + size, default_value);
};

template<typename T>
default_array<T>::default_array(std::initializer_list<T> values) : 
    _size(size), 
    default_value(T()), 
    arr(nullptr) {

    arr = new T[values.size()];
    
    std::copy(values.begin(), values.end(), arr);
};

template<typename T>
default_array<T>::~default_array() {
    delete[] arr;
}

};
