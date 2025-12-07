#pragma once

#include "array_ptr.h"

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
            data_ = ArrayPtr<Type>(capacity_);
        }
    }

    SimpleVector(const SimpleVector& other) : size_(other.size_), capacity_(other.size_) {
        if (other.size_ > 0) {
            data_ = ArrayPtr<Type>(size_);
            std::copy(other.begin(), other.end(), data_.Get());
        }
    }

    SimpleVector(SimpleVector&& other) noexcept {
        swap(other);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty()) {
                Clear();
                if (capacity_ > 0) {
                    data_ = ArrayPtr<Type>();
                    capacity_ = 0;
                }
            }
            else {
                SimpleVector temp(rhs);
                swap(temp);
            }
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            swap(rhs);
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        ArrayPtr<Type> new_data(new_capacity);
        std::move(begin(), end(), new_data.Get());
        data_.swap(new_data);
        capacity_ = new_capacity;
    }

    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);

            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            new_data[size_] = item;

            data_.swap(new_data);
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
            ArrayPtr<Type> new_data(new_capacity);

            std::move(data_.Get(), data_.Get() + size_, new_data.Get());
            new_data[size_] = std::move(item);

            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        else {
            data_[size_] = std::move(item);
        }
        ++size_;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();

        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);

            std::move(begin(), begin() + index, new_data.Get());
            new_data[index] = value;
            std::move(begin() + index, end(), new_data.Get() + index + 1);

            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        else {
            std::move_backward(begin() + index, end(), end() + 1);
            data_[index] = value;
        }
        ++size_;
        return begin() + index;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - begin();

        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_data(new_capacity);

            std::move(begin(), begin() + index, new_data.Get());
            new_data[index] = std::move(value);
            std::move(begin() + index, end(), new_data.Get() + index + 1);

            data_.swap(new_data);
            capacity_ = new_capacity;
        }
        else {
            std::move_backward(begin() + index, end(), end() + 1);
            data_[index] = std::move(value);
        }
        ++size_;
        return begin() + index;
    }

    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        size_t index = pos - begin();
        std::move(begin() + index + 1, end(), begin() + index);
        --size_;
        return begin() + index;
    }

    void swap(SimpleVector& other) noexcept {
        data_.swap(other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        if (size > 0) {
            data_ = ArrayPtr<Type>(size);
            std::generate(data_.Get(), data_.Get() + size, [] { return Type{}; });
        }
    }

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
        if (size > 0) {
            data_ = ArrayPtr<Type>(size);
            std::fill(data_.Get(), data_.Get() + size, value);
        }
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {
        if (size_ > 0) {
            data_ = ArrayPtr<Type>(size_);
            std::copy(init.begin(), init.end(), data_.Get());
        }
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
        assert(index < size_);
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
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
                std::generate(begin() + size_, begin() + new_size, [] { return Type{}; });
            }
            size_ = new_size;
            return;
        }

        size_t new_capacity = std::max(new_size, capacity_ * 2);
        ArrayPtr<Type> new_data(new_capacity);

        std::move(begin(), end(), new_data.Get());
        std::generate(new_data.Get() + size_, new_data.Get() + new_size, [] { return Type{}; });

        data_.swap(new_data);
        size_ = new_size;
        capacity_ = new_capacity;
    }

    Iterator begin() noexcept {
        return data_.Get();
    }

    Iterator end() noexcept {
        return data_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return data_.Get();
    }

    ConstIterator end() const noexcept {
        return data_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return data_.Get();
    }

    ConstIterator cend() const noexcept {
        return data_.Get() + size_;
    }

private:
    ArrayPtr<Type> data_;
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