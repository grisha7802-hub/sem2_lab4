#pragma once
#include <stdexcept>

template<class T>
class Option {
private:
    bool hasValue;
    T storedValue;

public:
    Option() : hasValue(false), storedValue() {}
    explicit Option(const T& value) : hasValue(true), storedValue(value) {}

    static Option<T> None() { return Option<T>(); }
    static Option<T> Some(const T& value) { return Option<T>(value); }

    bool IsSome() const { return hasValue; }

    const T& GetOrThrow() const {
        if (!hasValue) throw std::runtime_error("Option is None");
        return storedValue;
    }

    T GetOr(const T& fallback) const {
        return hasValue ? storedValue : fallback;
    }
};
