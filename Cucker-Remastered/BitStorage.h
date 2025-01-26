#pragma once

template<class Type = size_t>
class BitStorage {
private:
    using UINT = unsigned int;
    Type Bits = 0;
public:
    BitStorage() = default;
    BitStorage(Type num):Bits(num) {}
    void set(Type num) {
        this->Bits = num;
    }
    void bit(UINT pos, bool Value) {
        if (Value)
            Bits |= Value << (pos);
        else
            Bits &= ~(1 << (pos));
    }
    bool bit(UINT pos) {
        return (Bits >> pos) & 1;
    }
    Type range(UINT start, UINT end) {
        return (Bits >> start) & (Bits >> start);
    }
    Type get() {
        return Bits;
    }
    template<class CharT>
    inline void to_str(CharT* out) {
        int j = 0;
        if (Bits == 0) {
            out[j++] = '0';
            out[j++] = 0;
            return;
        }
        UINT len = sizeof(Type) * 8;
        while (j < len) {
            out[j] = (Bits >> j) & 1 ? '1' : '0';
            j++;
        }
        out[j] = 0;
    }
    template<class Type>
    BitStorage<Type> operator=(UINT bits) {
        this->Bits = bits;
        return *this;
    }
    bool operator[](UINT pos) {
        return bit(pos);
    }
    Type operator~() {
        return ~Bits;
    }
};