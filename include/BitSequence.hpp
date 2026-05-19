#pragma once
#include "ArraySequence.hpp"
#include "Bit.hpp"

class BitSequence : public Sequence<Bit> {
private:
    ArraySequence<Bit> bits;

public:
    BitSequence() : bits() {}
    BitSequence(const Bit* items, int count) : bits(items, count) {}

    int GetSize() const override { return bits.GetSize(); }
    const Bit& Get(int index) const override { return bits.Get(index); }
    Bit& Get(int index) override { return bits.Get(index); }
    const Bit& GetFirst() const override { return bits.GetFirst(); }
    const Bit& GetLast() const override { return bits.GetLast(); }

    void Append(const Bit& value) override { bits.Append(value); }
    void Prepend(const Bit& value) override { bits.Prepend(value); }
    void InsertAt(int index, const Bit& value) override { bits.InsertAt(index, value); }
    void Set(int index, const Bit& value) override { bits.Set(index, value); }

    BitSequence And(const BitSequence& other) const {
        int count = GetSize();
        if (other.GetSize() != count) throw std::invalid_argument("size mismatch");

        Bit* out = new Bit[count];
        for (int i = 0; i < count; ++i) out[i] = Get(i) & other.Get(i);
        BitSequence result(out, count);
        delete[] out;
        return result;
    }

    BitSequence Or(const BitSequence& other) const {
        int count = GetSize();
        if (other.GetSize() != count) throw std::invalid_argument("size mismatch");

        Bit* out = new Bit[count];
        for (int i = 0; i < count; ++i) out[i] = Get(i) | other.Get(i);
        BitSequence result(out, count);
        delete[] out;
        return result;
    }

    BitSequence Xor(const BitSequence& other) const {
        int count = GetSize();
        if (other.GetSize() != count) throw std::invalid_argument("size mismatch");

        Bit* out = new Bit[count];
        for (int i = 0; i < count; ++i) out[i] = Get(i) ^ other.Get(i);
        BitSequence result(out, count);
        delete[] out;
        return result;
    }

    BitSequence Not() const {
        int count = GetSize();
        Bit* out = new Bit[count];
        for (int i = 0; i < count; ++i) out[i] = ~Get(i);
        BitSequence result(out, count);
        delete[] out;
        return result;
    }
};
