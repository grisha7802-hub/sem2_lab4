#pragma once
#include "Sequence.hpp"
#include "LinkedList.hpp"

template<class T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T> storage;

public:
    ListSequence() : storage() {}
    ListSequence(const T* items, int count) : storage(items, count) {}

    Sequence<T>* Clone() const override { return new ListSequence<T>(*this); }
    Sequence<T>* CreateEmpty() const override { return new ListSequence<T>(); }

    int GetSize() const override { return storage.GetSize(); }

    const T& Get(int index) const override { return storage.Get(index); }
    T& Get(int index) override { return storage.Get(index); }

    const T& GetFirst() const override { return storage.GetFirst(); }
    const T& GetLast() const override { return storage.GetLast(); }

    void Append(const T& value) override { storage.Append(value); }
    void Prepend(const T& value) override { storage.Prepend(value); }
    void InsertAt(int index, const T& value) override { storage.InsertAt(index, value); }
    void Set(int index, const T& value) override { storage.Set(index, value); }
};
