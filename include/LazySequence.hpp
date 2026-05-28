#pragma once

#include "Cardinal.hpp"
#include "Sequence.hpp"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

template<class T>
class LazySequence {
private:
    Cardinal length;
    std::vector<T> materialized;
    std::function<T(std::size_t, const std::vector<T>&)> generator;

    void EnsureIndexMaterialized(std::size_t index) {
        if (length.IsFinite() && index >= length.Value()) {
            throw std::out_of_range("IndexOutOfRange");
        }

        while (materialized.size() <= index) {
            if (!generator) {
                throw std::out_of_range("IndexOutOfRange");
            }
            std::size_t nextIndex = materialized.size();
            materialized.push_back(generator(nextIndex, materialized));
        }
    }

public:
    LazySequence() : length(0), materialized(), generator(nullptr) {}

    LazySequence(const T* items, int count) : length(0), materialized(), generator(nullptr) {
        if (count < 0) throw std::invalid_argument("count < 0");
        materialized.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i) {
            materialized.push_back(items[i]);
        }
        length = Cardinal(materialized.size());
    }

    explicit LazySequence(Sequence<T>* seq) : length(0), materialized(), generator(nullptr) {
        if (seq == nullptr) throw std::invalid_argument("seq is null");
        int size = seq->GetSize();
        materialized.reserve(static_cast<std::size_t>(size));
        for (int i = 0; i < size; ++i) {
            materialized.push_back(seq->Get(i));
        }
        length = Cardinal(materialized.size());
    }

    LazySequence(const std::vector<T>& seed,
                 std::function<T(const std::vector<T>&)> recurrence,
                 Cardinal cardinal = Cardinal::Infinity())
        : length(cardinal), materialized(seed), generator(nullptr) {
        if (!recurrence) throw std::invalid_argument("recurrence is null");
        generator = [recurrence](std::size_t, const std::vector<T>& prefix) {
            return recurrence(prefix);
        };
    }

    LazySequence(const LazySequence<T>& other) = default;
    LazySequence<T>& operator=(const LazySequence<T>& other) = default;

    T GetFirst() {
        return Get(0);
    }

    T GetLast() {
        if (length.IsInfinite()) {
            throw std::runtime_error("GetLast on infinite sequence");
        }
        if (length.Value() == 0) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return Get(static_cast<int>(length.Value() - 1));
    }

    T Get(int index) {
        if (index < 0) throw std::out_of_range("IndexOutOfRange");
        EnsureIndexMaterialized(static_cast<std::size_t>(index));
        return materialized[static_cast<std::size_t>(index)];
    }

    LazySequence<T>* GetSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || endIndex < startIndex) throw std::out_of_range("IndexOutOfRange");
        EnsureIndexMaterialized(static_cast<std::size_t>(endIndex));

        std::vector<T> values;
        values.reserve(static_cast<std::size_t>(endIndex - startIndex + 1));
        for (int i = startIndex; i <= endIndex; ++i) {
            values.push_back(materialized[static_cast<std::size_t>(i)]);
        }
        return new LazySequence<T>(values.data(), static_cast<int>(values.size()));
    }

    Cardinal GetLength() const {
        return length;
    }

    std::size_t GetMaterializedCount() const {
        return materialized.size();
    }

    LazySequence<T>* Append(T item) {
        if (length.IsInfinite()) {
            throw std::runtime_error("Append to infinite sequence is undefined");
        }
        materialized.push_back(item);
        length = Cardinal(length.Value() + 1);
        return this;
    }

    LazySequence<T>* Prepend(T item) {
        materialized.insert(materialized.begin(), item);
        if (length.IsFinite()) {
            length = Cardinal(length.Value() + 1);
        }
        return this;
    }

    LazySequence<T>* InsertAt(T item, int index) {
        if (index < 0) throw std::out_of_range("IndexOutOfRange");
        if (length.IsFinite()) {
            if (static_cast<std::size_t>(index) > length.Value()) throw std::out_of_range("IndexOutOfRange");
            if (length.Value() > 0) {
                std::size_t ensureIndex = (index == static_cast<int>(length.Value()))
                    ? (length.Value() - 1)
                    : static_cast<std::size_t>(index);
                EnsureIndexMaterialized(ensureIndex);
            }
            materialized.insert(materialized.begin() + index, item);
            length = Cardinal(length.Value() + 1);
            return this;
        }

        EnsureIndexMaterialized(static_cast<std::size_t>(index));
        materialized.insert(materialized.begin() + index, item);
        return this;
    }

    template<class T2>
    LazySequence<T2>* Map(T2 (*mapper)(T)) {
        if (mapper == nullptr) throw std::invalid_argument("mapper is null");

        LazySequence<T>* self = this;
        std::vector<T2> seed;
        auto mapRecurrence = [self, mapper, seed](const std::vector<T2>& prefix) mutable -> T2 {
            int idx = static_cast<int>(prefix.size());
            return mapper(self->Get(idx));
        };
        return new LazySequence<T2>(seed, mapRecurrence, length);
    }

    template<class T2>
    T2 Reduce(T2 (*reducer)(T2, T), T2 initial) {
        if (reducer == nullptr) throw std::invalid_argument("reducer is null");
        if (length.IsInfinite()) throw std::runtime_error("Reduce on infinite sequence");

        T2 acc = initial;
        for (std::size_t i = 0; i < length.Value(); ++i) {
            acc = reducer(acc, Get(static_cast<int>(i)));
        }
        return acc;
    }

    LazySequence<T>* Where(bool (*predicate)(T)) {
        if (predicate == nullptr) throw std::invalid_argument("predicate is null");
        if (length.IsInfinite()) throw std::runtime_error("Where on infinite sequence is not bounded in this implementation");

        std::vector<T> filtered;
        for (std::size_t i = 0; i < length.Value(); ++i) {
            T value = Get(static_cast<int>(i));
            if (predicate(value)) filtered.push_back(value);
        }
        return new LazySequence<T>(filtered.data(), static_cast<int>(filtered.size()));
    }
};
