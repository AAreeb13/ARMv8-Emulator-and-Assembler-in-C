#include <stdbool.h>
#include <stdint.h>
#include "misc.h"

#define MASK_32 ((UINT32_C(1) << 31) - 1)
#define MASK_64 ((UINT64_C(1) << 63) - 1)

uint64_t getSubWord64(uint8_t start, uint8_t end, uint64_t data) {
  int width = end - start + 1;
  uint64_t subword64 = (data >> start) & ((1 << width) - 1);
  return subword64;
}

// extends N-bit number to 64 bits
int64_t signExtend(int32_t simmN, int N) {
    int64_t signedBit = simmN & (1 << (N - 1));
    return simmN - signedBit - signedBit;
}

// @param i - ensure it's between 0 and 31 inclusive.
uint8_t getBit(uint32_t word, uint8_t i) { return (word >> i) & 1; }

// accesses subsection of instruction between indexes start to end, inclusively.
// PRE: end>=start
uint32_t getSubWord(uint8_t start, uint8_t end, uint32_t word) {
    int width = end - start + 1;
    unsigned int subword = (word >> start) & ((1 << width) - 1);
    return subword;
}

int64_t ror(int64_t value, uint8_t shiftAmount, bool Xn) {
    if (!Xn) {
        shiftAmount %= 32;
        int32_t result = value;
        for (int i = 0; i < shiftAmount; i++) {
            int32_t firstBit = getBit(result, 0) ? 1 << 31 : 0;
            result = ((result >> 1) & MASK_32) + (firstBit);
        }
        return result;
    }
    shiftAmount %= 64;
    int64_t result = value;
    for (int i = 0; i < shiftAmount; i++) {
        int64_t firstBit = getBit(result, 0) ? UINT64_C(1) << 63 : 0;
        result = ((result >> 1) & MASK_64) + (firstBit);
    }
    return result;
}

int64_t lsr(int64_t value, uint8_t shiftAmount, bool Xn) {
    if (!Xn) {
        int32_t val = value;
        uint8_t bit = getBit(value, 31);
        val = val >> shiftAmount;
        if (bit) {
            val += bit << (32 - shiftAmount);
        }
        return val;
    } else {
        uint8_t bit = getBit(value, 63);
        value = value >> shiftAmount;
        if (bit) {
            value += bit << (64 - shiftAmount);
        }
        return value;
    }
}

int64_t lsl(int64_t value, uint8_t shiftAmount, bool Xn) {
    if (!Xn) {
        int32_t val = value;
        return val << shiftAmount;
    }
    return value << shiftAmount;
}

int64_t asr(int64_t value, uint8_t shiftAmount, bool Xn) {
    if (!Xn) {
        int32_t val = value;
        return val >> shiftAmount;
    }
    return value >> shiftAmount;
}