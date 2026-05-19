#pragma once
class Bit {
private:
    bool value;
public:
    Bit(bool v=false): value(v) {}
    bool Get() const { return value; }
    void Set(bool v) { value = v; }
    Bit operator&(const Bit& o) const { return Bit(value && o.value); }
    Bit operator|(const Bit& o) const { return Bit(value || o.value); }
    Bit operator^(const Bit& o) const { return Bit(value != o.value); }
    Bit operator~() const { return Bit(!value); }
};
