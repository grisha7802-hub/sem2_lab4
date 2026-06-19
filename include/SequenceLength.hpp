#pragma once

#include <cstddef>
#include <limits>

class SequenceLength {
private:
    bool infinite;
    std::size_t finiteValue;

public:
    SequenceLength() : infinite(false), finiteValue(0) {}
    SequenceLength(std::size_t value) : infinite(false), finiteValue(value) {}

    static SequenceLength Infinity() {
        SequenceLength c;
        c.infinite = true;
        c.finiteValue = 0;
        return c;
    }

    bool IsInfinite() const { return infinite; }
    bool IsFinite() const { return !infinite; }
    std::size_t Value() const { return finiteValue; }
};
