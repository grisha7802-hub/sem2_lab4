#pragma once
#include "DynamicArray.hpp"
#include "Option.hpp"
#include <stdexcept>

template<class T>
class ArrayQueue {
private:
    DynamicArray<T> storage;
    int length;

    void EnsureCapacity(int required) {
        int capacity = storage.GetSize();
        if (required <= capacity) return;
        int newCapacity = capacity <= 0 ? 1 : capacity;
        while (newCapacity < required) newCapacity *= 2;
        storage.Resize(newCapacity);
    }
public:
    ArrayQueue(): storage(1), length(0) {}
    int GetSize() const { return length; }
    bool IsEmpty() const { return length == 0; }

    void Enqueue(const T& value) {
        EnsureCapacity(length + 1);
        storage.Set(length, value);
        length++;
    }

    T Dequeue() {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        T first = storage.Get(0);
        for (int i = 1; i < length; ++i) storage.Set(i - 1, storage.Get(i));
        length--;
        return first;
    }

    const T& Front() const {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        return storage.Get(0);
    }

    Option<T> TryDequeue() { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Dequeue()); }
    Option<T> TryFront() const { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Front()); }
};
