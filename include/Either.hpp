#pragma once

#include <stdexcept>
#include <utility>
#include <variant>

template<class L, class R>
class Either {
private:
    struct LeftTag {};
    struct RightTag {};

    std::variant<L, R> value;

    Either(LeftTag, const L& leftValue)
        : value(std::in_place_index<0>, leftValue) {}

    Either(LeftTag, L&& leftValue)
        : value(std::in_place_index<0>, std::move(leftValue)) {}

    Either(RightTag, const R& rightValue)
        : value(std::in_place_index<1>, rightValue) {}

    Either(RightTag, R&& rightValue)
        : value(std::in_place_index<1>, std::move(rightValue)) {}

public:
    static Either<L, R> Left(const L& value) { return Either<L, R>(LeftTag{}, value); }
    static Either<L, R> Left(L&& value) { return Either<L, R>(LeftTag{}, std::move(value)); }
    static Either<L, R> Right(const R& value) { return Either<L, R>(RightTag{}, value); }
    static Either<L, R> Right(R&& value) { return Either<L, R>(RightTag{}, std::move(value)); }

    bool IsLeft() const { return value.index() == 0; }
    bool IsRight() const { return value.index() == 1; }

    const L& GetLeftOrThrow() const {
        if (!IsLeft()) throw std::runtime_error("Either is Right");
        return std::get<0>(value);
    }

    const R& GetRightOrThrow() const {
        if (!IsRight()) throw std::runtime_error("Either is Left");
        return std::get<1>(value);
    }

    L GetLeftOr(const L& fallback) const {
        return IsLeft() ? std::get<0>(value) : fallback;
    }

    R GetRightOr(const R& fallback) const {
        return IsRight() ? std::get<1>(value) : fallback;
    }
};
