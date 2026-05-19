#pragma once
#include "ArraySequence.hpp"

template<class T>
class ImmutableArraySequence : public Sequence<T> {
private:
    ArraySequence<T> data;

public:
    ImmutableArraySequence() : data() {}
    ImmutableArraySequence(const T* items, int count) : data(items, count) {}

    int GetSize() const override { return data.GetSize(); }
    const T& Get(int index) const override { return data.Get(index); }
    T& Get(int index) override { return data.Get(index); }
    const T& GetFirst() const override { return data.GetFirst(); }
    const T& GetLast() const override { return data.GetLast(); }

    void Append(const T& value) override { data.Append(value); }
    void Prepend(const T& value) override { data.Prepend(value); }
    void InsertAt(int index, const T& value) override { data.InsertAt(index, value); }
    void Set(int index, const T& value) override { data.Set(index, value); }

    ImmutableArraySequence<T> Appended(const T& value) const {
        ImmutableArraySequence<T> copy(*this);
        copy.data.Append(value);
        return copy;
    }
};
