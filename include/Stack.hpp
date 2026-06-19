#pragma once
#include "ArraySequence.hpp"
#include "Option.hpp"
#include "Sequence.hpp"
#include <stdexcept>

template<class T>
class Stack {
private:
    Sequence<T>* data;
public:
    Stack() : data(new ArraySequence<T>()) {}

    Stack(Sequence<T>* sequenceImplementation) : data(sequenceImplementation) {
        if (data == nullptr) {
            throw std::invalid_argument("sequenceImplementation is null");
        }
    }

    Stack(const T* items, int count) : data(new ArraySequence<T>(items, count)) {}

    Stack(const Stack<T>& other) : data(other.data->Clone()) {}

    Stack(Stack<T>&& other) noexcept : data(other.data) { other.data = nullptr; }

    Stack<T>& operator=(const Stack<T>& other) {
        if (this == &other) {
            return *this;
        }
        Sequence<T>* cloned = other.data->Clone();
        delete data;
        data = cloned;
        return *this;
    }

    Stack<T>& operator=(Stack<T>&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }

    ~Stack() { delete data; }

    int GetSize() const { return data->GetSize(); }
    bool IsEmpty() const { return GetSize() == 0; }

    void Push(const T& value) { data->Append(value); }

    T Pop() {
        if (IsEmpty()) throw std::out_of_range("Stack is empty");
        int last = data->GetSize() - 1;
        T value = data->Get(last);

        Sequence<T>* rebuilt = data->CreateEmpty();
        for (int i = 0; i < last; ++i) {
            rebuilt->Append(data->Get(i));
        }
        delete data;
        data = rebuilt;
        return value;
    }

    const T& Peek() const {
        if (IsEmpty()) throw std::out_of_range("Stack is empty");
        return data->Get(data->GetSize() - 1);
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
        Stack<T> result(data->CreateEmpty());
        for (int i = 0; i < GetSize(); ++i) {
            result.data->Append(data->Get(i));
        }
        for (int i = 0; i < other.GetSize(); ++i) {
            result.data->Append(other.data->Get(i));
        }
        return result;
    }

    Stack<T> GetSubStack(int start, int end) const {
        if (start < 0 || end < start || end >= GetSize()) throw std::out_of_range("Invalid substack range");
        Stack<T> result(data->CreateEmpty());
        for (int i = start; i <= end; ++i) {
            result.Push(data->Get(i));
        }
        return result;
    }

    int FindSubStack(const Stack<T>& pattern) const {
        if (pattern.GetSize() == 0) return 0;
        if (pattern.GetSize() > GetSize()) return -1;
        for (int i = 0; i <= GetSize() - pattern.GetSize(); ++i) {
            bool ok = true;
            for (int j = 0; j < pattern.GetSize(); ++j) {
                if (data->Get(i + j) != pattern.data->Get(j)) {
                    ok = false;
                    break;
                }
            }
            if (ok) return i;
        }
        return -1;
    }

    template<class U>
    Stack<U> Map(U (*mapper)(const T&)) const {
        Sequence<U>* mapped = new ArraySequence<U>();
        data->MapTo(*mapped, mapper);
        Stack<U> result(mapped);
        return result;
    }

    Stack<T> Where(bool (*predicate)(const T&)) const {
        Sequence<T>* filtered = data->CreateEmpty();
        data->WhereTo(*filtered, predicate);
        Stack<T> result(filtered);
        return result;
    }

    T Reduce(T (*reducer)(const T&, const T&), T start) const {
        T acc = start;
        for (int i = 0; i < GetSize(); ++i) {
            acc = reducer(acc, data->Get(i));
        }
        return acc;
    }

    const Sequence<T>& Raw() const { return *data; }
};
