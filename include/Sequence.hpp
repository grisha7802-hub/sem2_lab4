#pragma once

#include <stdexcept>
#include <utility>

template<class T>
class Sequence {
public:
    virtual ~Sequence() = default;
    virtual Sequence<T>* Clone() const = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;

    virtual int GetSize() const = 0;
    virtual const T& Get(int index) const = 0;
    virtual T& Get(int index) = 0;

    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;

    virtual void Append(const T& value) = 0;
    virtual void Prepend(const T& value) = 0;
    virtual void InsertAt(int index, const T& value) = 0;
    virtual void Set(int index, const T& value) = 0;

    template<class U>
    U Reduce(U initialValue, U (*reducer)(const U&, const T&)) const {
        if (reducer == nullptr) throw std::invalid_argument("reducer is null");

        U accumulator = initialValue;
        for (int index = 0; index < GetSize(); ++index) {
            accumulator = reducer(accumulator, Get(index));
        }
        return accumulator;
    }

    template<class U>
    void MapTo(Sequence<U>& destination, U (*mapper)(const T&)) const {
        if (mapper == nullptr) throw std::invalid_argument("mapper is null");

        for (int index = 0; index < GetSize(); ++index) {
            destination.Append(mapper(Get(index)));
        }
    }

    void WhereTo(Sequence<T>& destination, bool (*predicate)(const T&)) const {
        if (predicate == nullptr) throw std::invalid_argument("predicate is null");

        for (int index = 0; index < GetSize(); ++index) {
            const T& value = Get(index);
            if (predicate(value)) {
                destination.Append(value);
            }
        }
    }

    template<class U>
    void ZipTo(const Sequence<U>& other, Sequence<std::pair<T, U>>& destination) const {
        if (GetSize() != other.GetSize()) throw std::invalid_argument("size mismatch");

        for (int index = 0; index < GetSize(); ++index) {
            destination.Append(std::pair<T, U>(Get(index), other.Get(index)));
        }
    }
};
