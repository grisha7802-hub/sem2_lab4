#pragma once

#include <cstddef>
#include <limits>

class Cardinal {
private:
    bool infinite;
    std::size_t finiteValue;

public:
    Cardinal() : infinite(false), finiteValue(0) {}
    explicit Cardinal(std::size_t value) : infinite(false), finiteValue(value) {}

    static Cardinal Infinity() {
        Cardinal c;
        c.infinite = true;
        c.finiteValue = 0;
        return c;
    }

    bool IsInfinite() const { return infinite; }
    bool IsFinite() const { return !infinite; }
    std::size_t Value() const { return finiteValue; }
};
