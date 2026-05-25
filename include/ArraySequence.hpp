#pragma once
#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include "Option.hpp"
#include <stdexcept>
#include <utility>

template<class T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T> storage;
    int length;

    void EnsureCapacity(int required) {
        int capacity = storage.GetSize();
        if (required <= capacity) return;

        int newCapacity = (capacity <= 0) ? 1 : capacity;
        while (newCapacity < required) newCapacity *= 2;
        storage.Resize(newCapacity);
    }

public:
    ArraySequence() : storage(1), length(0) {}

    ArraySequence(const T* items, int count)
        : storage((count > 0) ? count : 1), length(0) {
        if (count < 0) throw std::invalid_argument("count < 0");
        for (int i = 0; i < count; ++i) {
            storage.Set(i, items[i]);
        }
        length = count;
    }

    Sequence<T>* Clone() const override { return new ArraySequence<T>(*this); }
    Sequence<T>* CreateEmpty() const override { return new ArraySequence<T>(); }

    int GetSize() const override { return length; }

    const T& Get(int index) const override {
        if (index < 0 || index >= length) throw std::out_of_range("IndexOutOfRange");
        return storage.Get(index);
    }

    T& Get(int index) override {
        if (index < 0 || index >= length) throw std::out_of_range("IndexOutOfRange");
        return storage.Get(index);
    }

    const T& GetFirst() const override {
        if (GetSize() == 0) throw std::out_of_range("IndexOutOfRange");
        return storage.Get(0);
    }

    const T& GetLast() const override {
        if (GetSize() == 0) throw std::out_of_range("IndexOutOfRange");
        return storage.Get(GetSize() - 1);
    }

    const T& operator[](int index) const { return Get(index); }
    T& operator[](int index) { return Get(index); }

    void Set(int index, const T& value) override {
        if (index < 0 || index >= length) throw std::out_of_range("IndexOutOfRange");
        storage.Set(index, value);
    }

    void Append(const T& value) override {
        EnsureCapacity(length + 1);
        storage.Set(length, value);
        ++length;
    }

    void Prepend(const T& value) override {
        InsertAt(0, value);
    }

    void InsertAt(int index, const T& value) override {
        if (index < 0 || index > length) throw std::out_of_range("IndexOutOfRange");

        EnsureCapacity(length + 1);
        for (int i = length; i > index; --i) {
            storage.Set(i, storage.Get(i - 1));
        }
        storage.Set(index, value);
        ++length;
    }

    template<class Predicate>
    ArraySequence<T> Filter(Predicate predicate) const {
        int maxCount = GetSize();
        T* temporary = new T[maxCount];
        int selectedCount = 0;

        for (int i = 0; i < maxCount; ++i) {
            const T& value = Get(i);
            if (predicate(value)) {
                temporary[selectedCount++] = value;
            }
        }

        ArraySequence<T> result(temporary, selectedCount);
        delete[] temporary;
        return result;
    }

    template<class U, class Mapper>
    ArraySequence<U> Map(Mapper mapper) const {
        int count = GetSize();
        U* mapped = new U[count];

        for (int i = 0; i < count; ++i) {
            mapped[i] = mapper(Get(i));
        }

        ArraySequence<U> result(mapped, count);
        delete[] mapped;
        return result;
    }

    template<class U, class Reducer>
    U Reduce(U initialValue, Reducer reducer) const {
        U accumulator = initialValue;
        for (int i = 0; i < GetSize(); ++i) {
            accumulator = reducer(accumulator, Get(i));
        }
        return accumulator;
    }

    Option<T> TryFind(bool (*predicate)(const T&)) const {
        for (int i = 0; i < GetSize(); ++i) {
            const T& value = Get(i);
            if (predicate(value)) {
                return Option<T>::Some(value);
            }
        }
        return Option<T>::None();
    }

    template<class U>
    ArraySequence<std::pair<T, U>> Zip(const ArraySequence<U>& other) const {
        int count = GetSize();
        if (other.GetSize() != count) throw std::invalid_argument("size mismatch");

        std::pair<T, U>* pairs = new std::pair<T, U>[count];
        for (int i = 0; i < count; ++i) {
            pairs[i] = std::pair<T, U>(Get(i), other.Get(i));
        }

        ArraySequence<std::pair<T, U>> result(pairs, count);
        delete[] pairs;
        return result;
    }

    template<class U>
    static void Unzip(const ArraySequence<std::pair<T, U>>& zipped, ArraySequence<T>& first, ArraySequence<U>& second) {
        first = ArraySequence<T>();
        second = ArraySequence<U>();

        for (int i = 0; i < zipped.GetSize(); ++i) {
            first.Append(zipped.Get(i).first);
            second.Append(zipped.Get(i).second);
        }
    }

    ArraySequence<ArraySequence<T>> Split(bool (*isDelimiter)(const T&)) const {
        ArraySequence<ArraySequence<T>> chunks;
        ArraySequence<T> currentChunk;

        for (int i = 0; i < GetSize(); ++i) {
            const T& value = Get(i);
            if (isDelimiter(value)) {
                chunks.Append(currentChunk);
                currentChunk = ArraySequence<T>();
            } else {
                currentChunk.Append(value);
            }
        }

        chunks.Append(currentChunk);
        return chunks;
    }

    void Slice(int startIndex, int removeCount, const ArraySequence<T>* inserted = nullptr) {
        int size = GetSize();
        int index = startIndex;

        if (index < 0) index = size + index;
        if (index < 0 || index > size) throw std::out_of_range("slice index");
        if (removeCount < 0) throw std::invalid_argument("removeCount < 0");

        int effectiveRemoveCount = removeCount;
        if (index + effectiveRemoveCount > size) {
            effectiveRemoveCount = size - index;
        }

        int insertCount = (inserted == nullptr) ? 0 : inserted->GetSize();
        int newSize = size - effectiveRemoveCount + insertCount;

        T* rebuilt = (newSize > 0) ? new T[newSize] : nullptr;
        int output = 0;

        for (int i = 0; i < index; ++i) rebuilt[output++] = Get(i);
        if (inserted != nullptr) {
            for (int i = 0; i < inserted->GetSize(); ++i) rebuilt[output++] = inserted->Get(i);
        }
        for (int i = index + effectiveRemoveCount; i < size; ++i) rebuilt[output++] = Get(i);

        int targetCapacity = (newSize > 0) ? newSize : 1;
        storage = DynamicArray<T>(targetCapacity);
        for (int i = 0; i < newSize; ++i) {
            storage.Set(i, rebuilt[i]);
        }
        length = newSize;
        delete[] rebuilt;
    }
};
