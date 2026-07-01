#pragma once

#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<class L, class R>
class Either {
private:
    enum class Side {
        LeftValue,
        RightValue
    };

    struct LeftTag {};
    struct RightTag {};

    static constexpr std::size_t StorageSize = sizeof(L) > sizeof(R) ? sizeof(L) : sizeof(R);
    static constexpr std::size_t StorageAlign = alignof(L) > alignof(R) ? alignof(L) : alignof(R);

    Side side;
    typename std::aligned_storage<StorageSize, StorageAlign>::type storage;

    L* LeftPtr() {
        return std::launder(reinterpret_cast<L*>(&storage));
    }

    const L* LeftPtr() const {
        return std::launder(reinterpret_cast<const L*>(&storage));
    }

    R* RightPtr() {
        return std::launder(reinterpret_cast<R*>(&storage));
    }

    const R* RightPtr() const {
        return std::launder(reinterpret_cast<const R*>(&storage));
    }

    void DestroyValue() {
        if (IsLeft()) {
            LeftPtr()->~L();
        } else {
            RightPtr()->~R();
        }
    }

    void CopyFrom(const Either<L, R>& other) {
        side = other.side;
        if (other.IsLeft()) {
            new (&storage) L(*other.LeftPtr());
        } else {
            new (&storage) R(*other.RightPtr());
        }
    }

    void MoveFrom(Either<L, R>&& other) {
        side = other.side;
        if (other.IsLeft()) {
            new (&storage) L(std::move(*other.LeftPtr()));
        } else {
            new (&storage) R(std::move(*other.RightPtr()));
        }
    }

    Either(LeftTag, const L& value) : side(Side::LeftValue) {
        new (&storage) L(value);
    }

    Either(LeftTag, L&& value) : side(Side::LeftValue) {
        new (&storage) L(std::move(value));
    }

    Either(RightTag, const R& value) : side(Side::RightValue) {
        new (&storage) R(value);
    }

    Either(RightTag, R&& value) : side(Side::RightValue) {
        new (&storage) R(std::move(value));
    }

public:
    Either(const Either<L, R>& other) {
        CopyFrom(other);
    }

    Either(Either<L, R>&& other) {
        MoveFrom(std::move(other));
    }

    Either<L, R>& operator=(const Either<L, R>& other) {
        if (this != &other) {
            DestroyValue();
            CopyFrom(other);
        }
        return *this;
    }

    Either<L, R>& operator=(Either<L, R>&& other) {
        if (this != &other) {
            DestroyValue();
            MoveFrom(std::move(other));
        }
        return *this;
    }

    ~Either() {
        DestroyValue();
    }

    static Either<L, R> Left(const L& value) { return Either<L, R>(LeftTag{}, value); }
    static Either<L, R> Left(L&& value) { return Either<L, R>(LeftTag{}, std::move(value)); }
    static Either<L, R> Right(const R& value) { return Either<L, R>(RightTag{}, value); }
    static Either<L, R> Right(R&& value) { return Either<L, R>(RightTag{}, std::move(value)); }

    bool IsLeft() const { return side == Side::LeftValue; }
    bool IsRight() const { return side == Side::RightValue; }

    const L& GetLeftOrThrow() const {
        if (!IsLeft()) throw std::runtime_error("Either is Right");
        return *LeftPtr();
    }

    const R& GetRightOrThrow() const {
        if (!IsRight()) throw std::runtime_error("Either is Left");
        return *RightPtr();
    }

    L GetLeftOr(const L& fallback) const {
        return IsLeft() ? *LeftPtr() : fallback;
    }

    R GetRightOr(const R& fallback) const {
        return IsRight() ? *RightPtr() : fallback;
    }
};
