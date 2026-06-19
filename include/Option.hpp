#pragma once
#include <new>
#include <stdexcept>
#include <utility>

template<class T>
class Option {
private:
    bool hasValue;
    alignas(T) unsigned char storage[sizeof(T)];

    T* Ptr() {
        return std::launder(reinterpret_cast<T*>(storage));
    }

    const T* Ptr() const {
        return std::launder(reinterpret_cast<const T*>(storage));
    }

    void DestroyValue() {
        if (hasValue) {
            Ptr()->~T();
            hasValue = false;
        }
    }

    void CopyFrom(const Option<T>& other) {
        if (other.hasValue) {
            new (storage) T(*other.Ptr());
            hasValue = true;
        } else {
            hasValue = false;
        }
    }

    void MoveFrom(Option<T>&& other) {
        if (other.hasValue) {
            new (storage) T(std::move(*other.Ptr()));
            hasValue = true;
        } else {
            hasValue = false;
        }
    }

public:
    Option() : hasValue(false) {}

    Option(const T& value) : hasValue(false) {
        new (storage) T(value);
        hasValue = true;
    }

    Option(T&& value) : hasValue(false) {
        new (storage) T(std::move(value));
        hasValue = true;
    }

    Option(const Option<T>& other) : hasValue(false) {
        CopyFrom(other);
    }

    Option(Option<T>&& other) : hasValue(false) {
        MoveFrom(std::move(other));
    }

    Option<T>& operator=(const Option<T>& other) {
        if (this != &other) {
            DestroyValue();
            CopyFrom(other);
        }
        return *this;
    }

    Option<T>& operator=(Option<T>&& other) {
        if (this != &other) {
            DestroyValue();
            MoveFrom(std::move(other));
        }
        return *this;
    }

    ~Option() {
        DestroyValue();
    }

    static Option<T> None() { return Option<T>(); }
    static Option<T> Some(const T& value) { return Option<T>(value); }
    static Option<T> Some(T&& value) { return Option<T>(std::move(value)); }

    bool IsSome() const { return hasValue; }

    const T& GetOrThrow() const {
        if (!hasValue) throw std::runtime_error("Option is None");
        return *Ptr();
    }

    T GetOr(const T& fallback) const {
        return hasValue ? *Ptr() : fallback;
    }
};
