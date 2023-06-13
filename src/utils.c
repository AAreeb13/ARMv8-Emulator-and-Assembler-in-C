#include "utils.h"

// puts bits in instruction word in desired location
// @param endIndex - index of last bit in @param bits
void putBits(uint32_t *total, uint32_t bits, uint32_t endIndex) {
  *total += bits << endIndex;
}