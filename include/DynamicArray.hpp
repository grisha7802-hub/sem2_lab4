#pragma once
#include <stdexcept>

template<class T>
class DynamicArray {
private:
    T* buffer;
    int size;

public:
    DynamicArray() : buffer(nullptr), size(0) {}

    DynamicArray(const T* items, int count) : buffer(nullptr), size(0) {
        if (count < 0) throw std::invalid_argument("count < 0");
        if (count > 0 && items == nullptr) throw std::invalid_argument("items is null");

        size = count;
        buffer = (size > 0) ? new T[size] : nullptr;

        for (int i = 0; i < size; ++i) {
            buffer[i] = items[i];
        }
    }

    DynamicArray(int newSize) : buffer(nullptr), size(0) {
        if (newSize < 0) throw std::invalid_argument("size < 0");
        size = newSize;
        buffer = (size > 0) ? new T[size] : nullptr;
    }

    DynamicArray(const DynamicArray<T>& other) : buffer(nullptr), size(other.size) {
        buffer = (size > 0) ? new T[size] : nullptr;
        for (int i = 0; i < size; ++i) {
            buffer[i] = other.buffer[i];
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this == &other) return *this;

        T* newBuffer = (other.size > 0) ? new T[other.size] : nullptr;
        for (int i = 0; i < other.size; ++i) {
            newBuffer[i] = other.buffer[i];
        }

        delete[] buffer;
        buffer = newBuffer;
        size = other.size;
        return *this;
    }

    ~DynamicArray() { delete[] buffer; }

    int GetSize() const { return size; }

    const T& Get(int index) const {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        return buffer[index];
    }

    T& Get(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        return buffer[index];
    }

    void Set(int index, const T& value) {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");
        buffer[index] = value;
    }

    void Resize(int newSize) {
        if (newSize < 0) throw std::invalid_argument("newSize < 0");

        T* newBuffer = (newSize > 0) ? new T[newSize] : nullptr;
        int copyCount = (newSize < size) ? newSize : size;

        for (int i = 0; i < copyCount; ++i) {
            newBuffer[i] = buffer[i];
        }

        delete[] buffer;
        buffer = newBuffer;
        size = newSize;
    }
};
