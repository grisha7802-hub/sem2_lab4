#pragma once

#include "ArraySequence.hpp"

template<class T>
class ArraySequenceMonad {
public:
    static ArraySequence<T> Return(const T& value) {
        ArraySequence<T> result;
        result.Append(value);
        return result;
    }

    template<class U>
    static ArraySequence<U> Bind(const ArraySequence<T>& source,
                                 ArraySequence<U> (*function)(const T&)) {
        ArraySequence<U> result;

        for (int sourceIndex = 0; sourceIndex < source.GetSize(); ++sourceIndex) {
            ArraySequence<U> inner = function(source.Get(sourceIndex));

            for (int innerIndex = 0; innerIndex < inner.GetSize(); ++innerIndex) {
                result.Append(inner.Get(innerIndex));
            }
        }

        return result;
    }
};
