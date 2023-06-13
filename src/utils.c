#include "utils.h"

// Put bits to specified location
void putBits(uint32_t *total, uint32_t bits, uint32_t endIndex) {
  *total += bits << endIndex;
}