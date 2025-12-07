#pragma once

#include <cassert>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) : data_(new Type[size]) {}

    ~ArrayPtr() {
        delete[] data_;
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) noexcept : data_(other.data_) {
        other.data_ = nullptr;
    }

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (this != &other) {
            std::swap(data_, other.data_);
        }
        return *this;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(data_, other.data_);
    }

    Type* Get() const noexcept {
        return data_;
    }

    Type& operator[](size_t index) noexcept {
        assert(data_ != nullptr);
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(data_ != nullptr);
        return data_[index];
    }

    explicit operator bool() const noexcept {
        return data_ != nullptr;
    }

    void Release() noexcept {
        delete[] data_;
        data_ = nullptr;
    }

private:
    Type* data_ = nullptr;
};