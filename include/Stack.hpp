#pragma once
#include "ArraySequence.hpp"
#include "Option.hpp"
#include <stdexcept>

template<class T>
class Stack {
private:
    ArraySequence<T> data;
public:
    Stack() : data() {}
    Stack(const T* items, int count) : data(items, count) {}

    int GetSize() const { return data.GetSize(); }
    bool IsEmpty() const { return GetSize() == 0; }

    void Push(const T& value) { data.Append(value); }

    T Pop() {
        if (IsEmpty()) throw std::out_of_range("Stack is empty");
        int last = data.GetSize() - 1;
        T value = data.Get(last);
        data.Slice(last, 1, nullptr);
        return value;
    }

    const T& Peek() const {
        if (IsEmpty()) throw std::out_of_range("Stack is empty");
        return data.Get(data.GetSize() - 1);
    }

    Option<T> TryPop() {
        if (IsEmpty()) return Option<T>::None();
        return Option<T>::Some(Pop());
    }

    Option<T> TryPeek() const {
        if (IsEmpty()) return Option<T>::None();
        return Option<T>::Some(Peek());
    }

    Stack<T> Concat(const Stack<T>& other) const {
        Stack<T> result = *this;
        for (int i = 0; i < other.GetSize(); ++i) result.data.Append(other.data.Get(i));
        return result;
    }

    Stack<T> GetSubStack(int start, int end) const {
        if (start < 0 || end < start || end >= GetSize()) throw std::out_of_range("Invalid substack range");
        Stack<T> result;
        for (int i = start; i <= end; ++i) result.Push(data.Get(i));
        return result;
    }

    int FindSubStack(const Stack<T>& pattern) const {
        if (pattern.GetSize() == 0) return 0;
        if (pattern.GetSize() > GetSize()) return -1;
        for (int i = 0; i <= GetSize() - pattern.GetSize(); ++i) {
            bool ok = true;
            for (int j = 0; j < pattern.GetSize(); ++j) {
                if (data.Get(i + j) != pattern.data.Get(j)) { ok = false; break; }
            }
            if (ok) return i;
        }
        return -1;
    }

    template<class U>
    Stack<U> Map(U (*mapper)(const T&)) const {
        Stack<U> result;
        for (int i = 0; i < GetSize(); ++i) result.Push(mapper(data.Get(i)));
        return result;
    }

    Stack<T> Where(bool (*predicate)(const T&)) const {
        Stack<T> result;
        for (int i = 0; i < GetSize(); ++i) if (predicate(data.Get(i))) result.Push(data.Get(i));
        return result;
    }

    T Reduce(T (*reducer)(const T&, const T&), T start) const {
        T acc = start;
        for (int i = 0; i < GetSize(); ++i) acc = reducer(acc, data.Get(i));
        return acc;
    }

    const ArraySequence<T>& Raw() const { return data; }
};
