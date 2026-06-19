#pragma once

#include "ArraySequence.hpp"
#include "Cardinal.hpp"
#include "Option.hpp"
#include "Sequence.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

template<class T>
class Generator {
private:
    std::function<bool(std::size_t)> hasNextRule;
    std::function<T(std::size_t)> nextRule;
    std::size_t position;

public:
    Generator() : hasNextRule(nullptr), nextRule(nullptr), position(0) {}

    Generator(std::function<bool(std::size_t)> hasNext, std::function<T(std::size_t)> next)
        : hasNextRule(hasNext), nextRule(next), position(0) {
        if (!nextRule) throw std::invalid_argument("next rule is null");
    }

    bool HasNext() const {
        return !hasNextRule || hasNextRule(position);
    }

    T GetNext() {
        if (!HasNext()) throw std::out_of_range("IndexOutOfRange");
        return nextRule(position++);
    }

    Option<T> TryGetNext() {
        if (!HasNext()) return Option<T>::None();
        return Option<T>::Some(GetNext());
    }
};

template<class T>
class LazySequence {
private:
    Cardinal length;
    ArraySequence<T> materialized;
    std::function<T(std::size_t, const Sequence<T>&)> generator;

    void EnsureIndexMaterialized(std::size_t index) {
        if (length.IsFinite() && index >= length.Value()) {
            throw std::out_of_range("IndexOutOfRange");
        }

        while (static_cast<std::size_t>(materialized.GetSize()) <= index) {
            if (!generator) {
                throw std::out_of_range("IndexOutOfRange");
            }
            std::size_t nextIndex = static_cast<std::size_t>(materialized.GetSize());
            materialized.Append(generator(nextIndex, materialized));
        }
    }

public:
    LazySequence() : length(0), materialized(), generator(nullptr) {}

    LazySequence(const T* items, int count) : length(0), materialized(items, count), generator(nullptr) {
        if (count < 0) throw std::invalid_argument("count < 0");
        length = Cardinal(static_cast<std::size_t>(materialized.GetSize()));
    }

    explicit LazySequence(const Sequence<T>& seq) : length(0), materialized(), generator(nullptr) {
        for (int i = 0; i < seq.GetSize(); ++i) {
            materialized.Append(seq.Get(i));
        }
        length = Cardinal(static_cast<std::size_t>(materialized.GetSize()));
    }

    explicit LazySequence(Sequence<T>* seq) : LazySequence(RequireSequence(seq)) {}

    LazySequence(const Sequence<T>& seed,
                 std::function<T(const Sequence<T>&)> recurrence,
                 Cardinal cardinal = Cardinal::Infinity())
        : length(cardinal), materialized(), generator(nullptr) {
        if (!recurrence) throw std::invalid_argument("recurrence is null");
        for (int i = 0; i < seed.GetSize(); ++i) {
            materialized.Append(seed.Get(i));
        }
        generator = [recurrence](std::size_t, const Sequence<T>& prefix) {
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
        return materialized.Get(index);
    }

    LazySequence<T>* GetSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || endIndex < startIndex) throw std::out_of_range("IndexOutOfRange");
        EnsureIndexMaterialized(static_cast<std::size_t>(endIndex));

        ArraySequence<T> values;
        for (int i = startIndex; i <= endIndex; ++i) {
            values.Append(materialized.Get(i));
        }
        return new LazySequence<T>(values);
    }

    Cardinal GetLength() const {
        return length;
    }

    std::size_t GetMaterializedCount() const {
        return static_cast<std::size_t>(materialized.GetSize());
    }

    Generator<T> CreateGenerator() {
        auto source = std::make_shared<LazySequence<T>>(*this);
        return Generator<T>(
            [source](std::size_t index) {
                return source->GetLength().IsInfinite() || index < source->GetLength().Value();
            },
            [source](std::size_t index) {
                return source->Get(static_cast<int>(index));
            }
        );
    }

    LazySequence<T>* Append(T item) {
        if (length.IsInfinite()) {
            return this;
        }
        materialized.Append(item);
        length = Cardinal(length.Value() + 1);
        return this;
    }

    LazySequence<T>* Append(Sequence<T>* items) {
        if (items == nullptr) throw std::invalid_argument("items is null");
        if (length.IsInfinite()) return this;
        for (int index = 0; index < items->GetSize(); ++index) {
            materialized.Append(items->Get(index));
        }
        length = Cardinal(length.Value() + static_cast<std::size_t>(items->GetSize()));
        return this;
    }

    LazySequence<T>* Prepend(T item) {
        materialized.Prepend(item);
        if (length.IsFinite()) {
            length = Cardinal(length.Value() + 1);
        }
        return this;
    }

    LazySequence<T>* Prepend(Sequence<T>* items) {
        if (items == nullptr) throw std::invalid_argument("items is null");
        for (int index = items->GetSize() - 1; index >= 0; --index) {
            materialized.Prepend(items->Get(index));
        }
        if (length.IsFinite()) {
            length = Cardinal(length.Value() + static_cast<std::size_t>(items->GetSize()));
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
            materialized.InsertAt(index, item);
            length = Cardinal(length.Value() + 1);
            return this;
        }

        EnsureIndexMaterialized(static_cast<std::size_t>(index));
        materialized.InsertAt(index, item);
        return this;
    }

    LazySequence<T>* InsertAt(Sequence<T>* items, int index) {
        if (items == nullptr) throw std::invalid_argument("items is null");
        if (index < 0) throw std::out_of_range("IndexOutOfRange");
        for (int offset = 0; offset < items->GetSize(); ++offset) {
            InsertAt(items->Get(offset), index + offset);
        }
        return this;
    }

    LazySequence<T>* RemoveAt(int index) {
        if (index < 0) throw std::out_of_range("IndexOutOfRange");
        if (length.IsFinite() && static_cast<std::size_t>(index) >= length.Value()) {
            throw std::out_of_range("IndexOutOfRange");
        }
        EnsureIndexMaterialized(static_cast<std::size_t>(index));

        ArraySequence<T> rebuilt;
        for (int i = 0; i < materialized.GetSize(); ++i) {
            if (i != index) rebuilt.Append(materialized.Get(i));
        }
        materialized = rebuilt;
        if (length.IsFinite()) {
            length = Cardinal(length.Value() - 1);
        }
        return this;
    }

    template<class T2>
    LazySequence<T2>* Map(T2 (*mapper)(T)) {
        if (mapper == nullptr) throw std::invalid_argument("mapper is null");

        LazySequence<T>* self = this;
        ArraySequence<T2> seed;
        auto mapRecurrence = [self, mapper](const Sequence<T2>& prefix) -> T2 {
            int idx = prefix.GetSize();
            return mapper(self->Get(idx));
        };
        return new LazySequence<T2>(seed, mapRecurrence, length);
    }

    LazySequence<T>* Concat(LazySequence<T>* list) {
        if (list == nullptr) throw std::invalid_argument("list is null");

        auto first = std::make_shared<LazySequence<T>>(*this);
        auto second = std::make_shared<LazySequence<T>>(*list);
        ArraySequence<T> seed;

        Cardinal resultLength = Cardinal::Infinity();
        if (first->GetLength().IsFinite() && second->GetLength().IsFinite()) {
            resultLength = Cardinal(first->GetLength().Value() + second->GetLength().Value());
        }

        auto recurrence = [first, second](const Sequence<T>& prefix) -> T {
            std::size_t index = static_cast<std::size_t>(prefix.GetSize());
            if (first->GetLength().IsInfinite() || index < first->GetLength().Value()) {
                return first->Get(static_cast<int>(index));
            }
            return second->Get(static_cast<int>(index - first->GetLength().Value()));
        };

        return new LazySequence<T>(seed, recurrence, resultLength);
    }

    template<class U>
    LazySequence<std::pair<T, U>>* Zip(LazySequence<U>* list) {
        if (list == nullptr) throw std::invalid_argument("list is null");

        auto first = std::make_shared<LazySequence<T>>(*this);
        auto second = std::make_shared<LazySequence<U>>(*list);
        ArraySequence<std::pair<T, U>> seed;

        Cardinal resultLength = Cardinal::Infinity();
        if (first->GetLength().IsFinite() && second->GetLength().IsFinite()) {
            std::size_t minLength = first->GetLength().Value() < second->GetLength().Value()
                ? first->GetLength().Value()
                : second->GetLength().Value();
            resultLength = Cardinal(minLength);
        }

        auto recurrence = [first, second](const Sequence<std::pair<T, U>>& prefix) -> std::pair<T, U> {
            int index = prefix.GetSize();
            return std::make_pair(first->Get(index), second->Get(index));
        };

        return new LazySequence<std::pair<T, U>>(seed, recurrence, resultLength);
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

        auto source = std::make_shared<LazySequence<T>>(*this);
        if (length.IsFinite()) {
            ArraySequence<T> filtered;
            for (std::size_t i = 0; i < length.Value(); ++i) {
                T value = Get(static_cast<int>(i));
                if (predicate(value)) filtered.Append(value);
            }
            return new LazySequence<T>(filtered);
        }

        ArraySequence<T> seed;
        auto cursor = std::make_shared<std::size_t>(0);
        auto recurrence = [source, predicate, cursor](const Sequence<T>&) -> T {
            while (true) {
                T value = source->Get(static_cast<int>((*cursor)++));
                if (predicate(value)) return value;
            }
        };
        return new LazySequence<T>(seed, recurrence, Cardinal::Infinity());
    }

private:
    static Sequence<T>& RequireSequence(Sequence<T>* seq) {
        if (seq == nullptr) throw std::invalid_argument("seq is null");
        return *seq;
    }
};
