#pragma once
#include "ListSequence.hpp"
#include "Queue.hpp"

template<class T>
class ListQueue : public Queue<T> {
public:
    ListQueue() : Queue<T>(new ListSequence<T>()) {}
    explicit ListQueue(Sequence<T>* sequenceImplementation) : Queue<T>(sequenceImplementation) {}
    ListQueue(const T* items, int count) : Queue<T>(new ListSequence<T>(items, count)) {}
};
