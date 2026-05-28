#pragma once

#include "LazySequence.hpp"

#include <memory>
#include <utility>
#include <vector>

template<class T>
class Stream {
private:
    std::shared_ptr<LazySequence<T>> sequence;

public:
    Stream() : sequence(std::make_shared<LazySequence<T>>()) {}
    explicit Stream(LazySequence<T>* seq) : sequence(seq) {
        if (seq == nullptr) throw std::invalid_argument("seq is null");
    }

    static Stream<T> FromArray(const T* items, int count) {
        return Stream<T>(new LazySequence<T>(items, count));
    }

    static Stream<T> Recurrence(const std::vector<T>& seed,
                                std::function<T(const std::vector<T>&)> recurrence,
                                Cardinal cardinal = Cardinal::Infinity()) {
        return Stream<T>(new LazySequence<T>(seed, recurrence, cardinal));
    }

    T Get(int index) const { return sequence->Get(index); }

    std::vector<T> Take(int count) const {
        if (count < 0) throw std::invalid_argument("count < 0");
        std::vector<T> out;
        out.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i) out.push_back(sequence->Get(i));
        return out;
    }

    template<class U>
    Stream<U> Map(U (*mapper)(T)) const {
        return Stream<U>(sequence->template Map<U>(mapper));
    }

    template<class U>
    U Reduce(U (*reducer)(U, T), U initial) const {
        return sequence->template Reduce<U>(reducer, initial);
    }

    Stream<T> Where(bool (*predicate)(T)) const {
        return Stream<T>(sequence->Where(predicate));
    }

    template<class U>
    Stream<std::pair<T, U>> Zip(const Stream<U>& other) const {
        auto self = sequence;
        auto otherSeq = other.sequence;
        std::vector<std::pair<T, U>> seed;
        auto recurrence = [self, otherSeq](const std::vector<std::pair<T, U>>& prefix) -> std::pair<T, U> {
            int idx = static_cast<int>(prefix.size());
            return std::make_pair(self->Get(idx), otherSeq->Get(idx));
        };

        Cardinal resultCardinal = Cardinal::Infinity();
        if (self->GetLength().IsFinite() && otherSeq->GetLength().IsFinite()) {
            std::size_t minSize = self->GetLength().Value() < otherSeq->GetLength().Value()
                ? self->GetLength().Value() : otherSeq->GetLength().Value();
            resultCardinal = Cardinal(minSize);
        }
        return Stream<std::pair<T, U>>(new LazySequence<std::pair<T, U>>(seed, recurrence, resultCardinal));
    }
};
