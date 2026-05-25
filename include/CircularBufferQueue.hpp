#pragma once
#include "DynamicArray.hpp"
#include "Option.hpp"
#include <stdexcept>

template<class T>
class CircularBufferQueue {
private:
    DynamicArray<T> buffer;
    int capacity;
    int head;
    int tail;
    int length;
public:
    explicit CircularBufferQueue(int fixedCapacity = 128): buffer(fixedCapacity), capacity(fixedCapacity), head(0), tail(0), length(0) {
        if (capacity <= 0) throw std::invalid_argument("capacity must be > 0");
    }
    CircularBufferQueue(const CircularBufferQueue& other) = default;
    CircularBufferQueue& operator=(const CircularBufferQueue& other) = default;
    ~CircularBufferQueue() = default;

    int GetSize() const { return length; }
    bool IsEmpty() const { return length == 0; }
    bool IsFull() const { return length == capacity; }

    void Enqueue(const T& value) {
        if (IsFull()) throw std::out_of_range("Queue is full");
        buffer.Set(tail, value);
        tail = (tail + 1) % capacity;
        length++;
    }

    T Dequeue() {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        T value = buffer.Get(head);
        head = (head + 1) % capacity;
        length--;
        return value;
    }

    const T& Front() const {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        return buffer.Get(head);
    }

    Option<T> TryDequeue() {
        if (IsEmpty()) return Option<T>::None();
        return Option<T>::Some(Dequeue());
    }
    Option<T> TryFront() const {
        if (IsEmpty()) return Option<T>::None();
        return Option<T>::Some(Front());
    }
};
