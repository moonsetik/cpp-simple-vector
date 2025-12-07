#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>

struct ReserveProxyObj;

ReserveProxyObj Reserve(size_t capacity);

struct ReserveProxyObj {
    explicit ReserveProxyObj(size_t capacity) : capacity(capacity) {}
    size_t capacity;
};

inline ReserveProxyObj Reserve(size_t capacity) {
    return ReserveProxyObj(capacity);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(ReserveProxyObj res) : capacity_(res.capacity) {
        if (capacity_ > 0) {
            data_ = new Type[capacity_];
        }
    }

    SimpleVector(const SimpleVector& other) : size_(other.size_), capacity_(other.size_) {
        if (other.size_ > 0) {
            data_ = new Type[size_];
            std::copy(other.begin(), other.end(), data_);
        }
    }

    SimpleVector(SimpleVector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            delete[] data_;

            data_ = rhs.data_;
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;

            rhs.data_ = nullptr;
            rhs.size_ = 0;
            rhs.capacity_ = 0;
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        Type* new_data = new Type[new_capacity];
        std::move(begin(), end(), new_data);

        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Type* new_data = new Type[new_capacity];

            std::move(data_, data_ + size_, new_data);
            new_data[size_] = item;

            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        else {
            data_[size_] = item;
        }
        ++size_;
    }

    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Type* new_data = new Type[new_capacity];

            std::move(data_, data_ + size_, new_data);
            new_data[size_] = std::move(item);

            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        else {
            data_[size_] = std::move(item);
        }
        ++size_;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t index = pos - data_;

        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Type* new_data = new Type[new_capacity];

            std::move(data_, data_ + index, new_data);
            new_data[index] = value;
            std::move(data_ + index, data_ + size_, new_data + index + 1);

            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        else {
            std::move_backward(data_ + index, data_ + size_, data_ + size_ + 1);
            data_[index] = value;
        }
        ++size_;
        return data_ + index;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t index = pos - data_;

        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Type* new_data = new Type[new_capacity];

            std::move(data_, data_ + index, new_data);
            new_data[index] = std::move(value);
            std::move(data_ + index, data_ + size_, new_data + index + 1);

            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        else {
            std::move_backward(data_ + index, data_ + size_, data_ + size_ + 1);
            data_[index] = std::move(value);
        }
        ++size_;
        return data_ + index;
    }

    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    Iterator Erase(ConstIterator pos) {
        size_t index = pos - data_;
        std::move(data_ + index + 1, data_ + size_, data_ + index);
        --size_;
        return data_ + index;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        if (size > 0) {
            data_ = new Type[size];
            std::fill(data_, data_ + size, Type{});
        }
    }

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
        if (size > 0) {
            data_ = new Type[size];
            std::fill(data_, data_ + size, value);
        }
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {
        if (size_ > 0) {
            data_ = new Type[size_];
            std::copy(init.begin(), init.end(), data_);
        }
    }

    ~SimpleVector() {
        delete[] data_;
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return data_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= capacity_) {
            if (new_size > size_) {
                for (size_t i = size_; i < new_size; i++) {
                    data_[i] = Type{};
                }
            }
            size_ = new_size;
            return;
        }

        size_t new_capacity = std::max(new_size, capacity_ * 2);
        Type* new_data = new Type[new_capacity];

        std::move(data_, data_ + size_, new_data);
        for (size_t i = size_; i < new_size; i++) {
            new_data[i] = Type{};
        }

        delete[] data_;
        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    Iterator begin() noexcept {
        return data_;
    }

    Iterator end() noexcept {
        return data_ + size_;
    }

    ConstIterator begin() const noexcept {
        return data_;
    }

    ConstIterator end() const noexcept {
        return data_ + size_;
    }

    ConstIterator cbegin() const noexcept {
        return data_;
    }

    ConstIterator cend() const noexcept {
        return data_ + size_;
    }

private:
    Type* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}