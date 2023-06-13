#include "storeandLoad.h"
#include "misc.h"
#include "structure.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// STORING AND LOADING
uint8_t singleDataTransfer(uint32_t word) {
  // size of the load
  uint8_t sf = getBit(word, 30);
  uint8_t rt_index = getSubWord(0, 4, word); // target register address.
  int8_t l = getBit(word, 22); // 1 if load, 1 if store, else invald address.
  uint64_t address;
  ////
  if (getBit(word, 31) == 1) { // Single Data Transfer
    address = (getBit(word, 24) == 1) ? unsignedImmediateOffset(word, sf)
                                      : // Unsigned Immediate Offset or...
                  ((getBit(word, 21) == 1) ? registerOffset(word)
                                           : //...Register Offset or...
                       ((getBit(word, 11) == 1)
                            ? preIndexed(word)
                            : postIndexed(word))); //...Pre or Post Index
  } else {                                         // Load Literal
    address = loadLiteral(word);
    l = 1;
  }
  ////

  // must be within necessary range
  if (address >= (1 << 21) || address <= 0) {
    l = -1;
    printf("Address out of range\n");
  }

  if (l == 1) {
    // LOADING TO REGISTER OR STORING IN MAIN MEMORY
    load(address, rt_index, sf);
  } else if (l == 0) {
    store(address, rt_index, sf);
  } else {
    return EXIT_FAILURE;
  } // if invalid address
  return EXIT_SUCCESS;
} // Pre U = 1, sf = 1 or 0
uint64_t unsignedImmediateOffset(uint32_t word, uint8_t sf) {
  uint8_t xn_index = getSubWord(5, 9, word); // register address
  uint64_t xn = genRegisters[xn_index].value;
  uint64_t imm12 = getSubWord(10, 12, word);
  uint64_t uoffset = (sf == 1) ? imm12 << 3 : imm12 << 2;

  uint64_t address = uoffset + xn;
  return address;
}
uint64_t preIndexed(uint32_t word) {
  int64_t simm9 = getSubWord(12, 20, word);
  int64_t signedBit = simm9 & (1 << 8); // access value at final position
  simm9 = simm9 - signedBit - signedBit;
  uint8_t xn_index = getSubWord(5, 9, word); // register address
  genRegisters[xn_index].value += simm9;

  uint64_t address = genRegisters[xn_index].value;

  return address;
}
uint64_t postIndexed(uint32_t word) {
  int64_t simm9 = (int64_t)getSubWord(12, 20, word);
  int64_t signedBit = simm9 & (1 << 8);
  simm9 = simm9 - signedBit - signedBit;
  int64_t xn = genRegisters[getSubWord(5, 9, word)].value; // old register value
  genRegisters[getSubWord(5, 9, word)].value += simm9;     // new register value

  uint64_t address = xn;
  return address;
}
uint64_t registerOffset(uint32_t word) {
  uint8_t xn_index = getSubWord(5, 9, word);
  uint64_t xn = genRegisters[xn_index].value;
  uint8_t xm_index = getSubWord(16, 20, word);
  uint64_t xm = genRegisters[xm_index].value;

  uint64_t address = xn + xm;
  return address;
}

int64_t signExtendTo64(int64_t value, uint8_t length) {
    // Extract the sign bit (11th bit)
    int64_t signBit = (value >> (length - 1)) & 1;

    // Perform sign extension if the sign bit is 1
    if (signBit == 1) {
        value |= (~0ULL << length); // Fill the higher bits with 1's
    }
    return value;
}

uint64_t loadLiteral(uint32_t word) {
    int64_t simm19 = (int64_t) getSubWord(5, 23, word);
    int64_t simm19Times4 = simm19 << 2;
    int64_t signExtended = signExtendTo64(simm19Times4, 21);

    uint64_t address = (uint64_t) (signExtended + PC.value);
    return address;
}

// Pre: sf = 1 or 0 , address is non-negative and less than 2^21
void load(uint64_t address, uint8_t rt_index, uint8_t sf) {
  int n =
      (sf == 1)
          ? 8
          : 4; // decides whether registers are 64bit version or 32bit version
  uint64_t loadedValue = 0;
  for (int i = 0; i < n; i++) {
    loadedValue += (memory[address + i] << i * 8);
  }
  genRegisters[rt_index].value = loadedValue;
}
// Pre sf = 1 or 0 , address is non-negative and less than 2^21
void store(uint64_t address, uint8_t rt_index, uint8_t sf) {
  int n =
      (sf == 1)
          ? 8
          : 4; // decides whether registers are 64bit version or 32bit version
  for (int i = 0; i < n; i++) {
    memory[address + i] = (uint8_t)getSubWord64(
        i * 8, i * 8 + 7, genRegisters[rt_index].value); //
  }
}
