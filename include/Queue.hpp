#pragma once

#include "ArraySequence.hpp"
#include "Option.hpp"
#include "Sequence.hpp"
#include <stdexcept>

template<class T>
class Queue {
private:
    Sequence<T>* data;

public:
    Queue() : data(new ArraySequence<T>()) {}

    explicit Queue(Sequence<T>* sequenceImplementation) : data(sequenceImplementation) {
        if (data == nullptr) {
            throw std::invalid_argument("sequenceImplementation is null");
        }
    }

    Queue(const T* items, int count) : data(new ArraySequence<T>(items, count)) {}

    Queue(const Queue<T>& other) : data(other.data->Clone()) {}

    Queue(Queue<T>&& other) noexcept : data(other.data) { other.data = nullptr; }

    Queue<T>& operator=(const Queue<T>& other) {
        if (this == &other) {
            return *this;
        }
        Sequence<T>* cloned = other.data->Clone();
        delete data;
        data = cloned;
        return *this;
    }

    Queue<T>& operator=(Queue<T>&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }

    ~Queue() { delete data; }

    int GetSize() const { return data->GetSize(); }
    bool IsEmpty() const { return GetSize() == 0; }

    void Enqueue(const T& value) { data->Append(value); }

    T Dequeue() {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");

        T first = data->GetFirst();
        Sequence<T>* rebuilt = data->CreateEmpty();
        for (int i = 1; i < data->GetSize(); ++i) {
            rebuilt->Append(data->Get(i));
        }
        delete data;
        data = rebuilt;
        return first;
    }

    const T& Front() const {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        return data->GetFirst();
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
