
#include "branch.h"
#include "misc.h"
#include "structure.h"


// Branch Instructions functions
void branchInstructions(uint32_t word) {
  uint8_t test = getSubWord(29, 31, word);
  // does reach this
  if (test == 0) { // 0b000
    // branch1 tests this
    unconditionalOffset(signExtend(getSubWord(0, 25, word), 26));
  } else if (test == 6) { // 0b110
    unconditionalRegister(getSubWord(5, 9, word));
  } else if (test == 2) { // 0b010
    conditionalBranches(signExtend(getSubWord(5, 23, word), 19),
                        getSubWord(0, 3, word));
  } else {
    printf("NO");
  }
}

// signed offset
void unconditionalOffset(int64_t simm26) { PC.value = (simm26 << 2); }

// address stored in register
void unconditionalRegister(uint8_t xn) {
  if (xn != 0b11111) {
    PC.value = genRegisters[xn].value;
  }
}
void conditionalBranches(int64_t simm19, uint8_t cond) {
  int64_t offset = simm19 << 2;
  if (cond == 0 && pState.Z == 1) { // EQ - 0b0000
    PC.value += signExtend(offset, 19);
  } else if (cond == 1 && pState.Z == 0) { // NE - 0b0001
    PC.value += signExtend(offset, 19);
  } else if (cond == 10 && pState.N == pState.V) { // GE - 0b1010
    PC.value += signExtend(offset, 19);
  } else if (cond == 11 && pState.N != pState.V) { // LT - 0b1011
    PC.value += signExtend(offset, 19);
  } else if (cond == 12 && pState.Z == 0 &&
             pState.N == pState.V) { // GT - 0b1100
    PC.value += signExtend(offset, 19);
  } else if (cond == 13 &&
             !(pState.Z == 0 && pState.N == pState.V)) { // LE - 0b1101
    PC.value += signExtend(offset, 19);
  } else if (cond == 14) { // AL - 0b1110
    PC.value += signExtend(offset, 19);
  }
}