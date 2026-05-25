#pragma once
#include "ArraySequence.hpp"
#include "Queue.hpp"

template<class T>
class ArrayQueue : public Queue<T> {
public:
    ArrayQueue() : Queue<T>(new ArraySequence<T>()) {}
    explicit ArrayQueue(Sequence<T>* sequenceImplementation) : Queue<T>(sequenceImplementation) {}
    ArrayQueue(const T* items, int count) : Queue<T>(new ArraySequence<T>(items, count)) {}
};
