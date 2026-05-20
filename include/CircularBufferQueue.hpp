#pragma once
#include "Option.hpp"
#include <stdexcept>

template<class T>
class CircularBufferQueue {
private:
    T* buffer;
    int capacity;
    int head;
    int tail;
    int length;
public:
    explicit CircularBufferQueue(int fixedCapacity = 128): buffer(nullptr), capacity(fixedCapacity), head(0), tail(0), length(0) {
        if (capacity <= 0) throw std::invalid_argument("capacity must be > 0");
        buffer = new T[capacity];
    }
    CircularBufferQueue(const CircularBufferQueue& other): buffer(new T[other.capacity]), capacity(other.capacity), head(other.head), tail(other.tail), length(other.length) {
        for (int i = 0; i < capacity; ++i) buffer[i] = other.buffer[i];
    }
    CircularBufferQueue& operator=(const CircularBufferQueue& other) {
        if (this == &other) return *this;
        delete[] buffer;
        capacity = other.capacity; head = other.head; tail = other.tail; length = other.length;
        buffer = new T[capacity];
        for (int i = 0; i < capacity; ++i) buffer[i] = other.buffer[i];
        return *this;
    }
    ~CircularBufferQueue(){ delete[] buffer; }

    int GetSize() const { return length; }
    bool IsEmpty() const { return length == 0; }
    bool IsFull() const { return length == capacity; }

    void Enqueue(const T& value) {
        if (IsFull()) throw std::out_of_range("Queue is full");
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        length++;
    }

    T Dequeue() {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        T value = buffer[head];
        head = (head + 1) % capacity;
        length--;
        return value;
    }

    const T& Front() const {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        return buffer[head];
    }

    Option<T> TryDequeue() { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Dequeue()); }
    Option<T> TryFront() const { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Front()); }
};
