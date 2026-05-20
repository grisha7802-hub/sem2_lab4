#pragma once
#include "LinkedList.hpp"
#include "Option.hpp"
#include <stdexcept>

template<class T>
class ListQueue {
private:
    LinkedList<T> storage;
public:
    int GetSize() const { return storage.GetSize(); }
    bool IsEmpty() const { return GetSize() == 0; }

    void Enqueue(const T& value) { storage.Append(value); }

    T Dequeue() {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        T first = storage.GetFirst();
        storage.RemoveAt(0);
        return first;
    }

    const T& Front() const {
        if (IsEmpty()) throw std::out_of_range("Queue is empty");
        return storage.GetFirst();
    }

    Option<T> TryDequeue() { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Dequeue()); }
    Option<T> TryFront() const { if (IsEmpty()) return Option<T>::None(); return Option<T>::Some(Front()); }
};
