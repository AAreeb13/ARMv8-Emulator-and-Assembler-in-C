
#include <stdint.h>
#include <stdbool.h>
#include "dataProcessImmediate.h"
#include "misc.h"
#include "structure.h"

void dataProcessImmediate(uint32_t word) {
  uint8_t opi = getSubWord(23, 25, word);
  uint8_t sf = getBit(word, 31);
  if (opi == 2) /*0b010*/ {
    if (sf == 1) {
      arithmetic(word, 1);
    } else {
      arithmetic(word, 0);
    }
  } else /*opi = 0b101*/ {
    if (sf == 1) {
      wideMove(word, 1);
    } else {
      wideMove(word, 0);
    }
  }
}

void arithmetic(uint32_t word, bool Xn) {
  uint8_t sh = getBit(word, 22);
  uint32_t Op_2;

  if (sh == 0) {
    Op_2 = getSubWord(10, 21, word);
  } else {
    Op_2 = getSubWord(10, 21, word) << 12;
  }

  uint8_t opc = getSubWord(29, 30, word);
  uint8_t rd = getSubWord(0, 4, word);
  uint8_t rn = getSubWord(5, 9, word);

  if (opc == 0) /*0b00*/ {
    add(rn, Op_2, rd, Xn);
  } else if (opc == 1) /*0b01*/ {
    adds(rn, Op_2, rd, Xn);
  } else if (opc == 2) /*0b10*/ {
    sub(rn, Op_2, rd, Xn);
  } else /*0b11*/ {
    subs(rn, Op_2, rd, Xn);
  }
}

void add(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value + (Op_2)
                              : ((int32_t)genRegisters[rn].value) + Op_2;
}

void adds(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  uint8_t length = Xn ? 64 : 32;
  int8_t arr_rn[length];
  int8_t arr_Op[length];
  int8_t carry[length + 1];
  int8_t result[length];
  // initialise arrays
  for (int i = 0; i < length; i++) {
    arr_rn[i] = getBit(genRegisters[rn].value, i);
    arr_Op[i] = getBit(Op_2, i);
    carry[i] = 0;
  }
  // bitwise additon
  for (int i = 0; i < length; i++) {
    int8_t total = arr_rn[i] + arr_Op[i] + carry[i];
    switch (total) {
      case 0:
        result[i] = 0;
        break;
      case 1:
        result[i] = 1;
        break;
      case 2:
        result[i] = 0;
        carry[i + 1] = 1;
        break;
      case 3:
        result[i] = 1;
        carry[i + 1] = 1;
        break;
    }
  }
  // calculate total
  int64_t value = 0;
  for (int i = 0; i < length; i++) {
    value += result[i] << i;
  }
  if (rd != 32) /*ZR*/ {
    genRegisters[rd].value = Xn ? value : (int32_t)value;
  }
  // set flags
  pState.C = carry[length];
  pState.V = !arr_rn[length - 1] && result[length - 1];
  pState.N = result[length - 1];
  pState.Z = !value;
}
void sub(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
                              : ((int32_t)genRegisters[rn].value) - Op_2;
}

void subs(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
                              : ((int32_t)genRegisters[rn].value) - Op_2;
  // UPDATE FLAGS!!!
}

void wideMove(uint32_t word, bool Xn) {
  uint8_t hw = getSubWord(21, 21, word);
  uint64_t Op = getSubWord(5, 20, word) << (hw * 16);
  uint8_t opc = getSubWord(29, 30, word);
  uint8_t rd = getSubWord(0, 4, word);
  switch (opc) {
    case 0: // 0b00
      movn(Op, rd, Xn);
      break;
    case 2: // 0b10
      movz(Op, rd);
      break;
    case 3: // 0b10
      movk(Op, rd, hw, Xn);
      break;
  }
}

void movn(uint64_t Op, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? ~Op : (int32_t)(~Op);
}

void movz(uint64_t Op, uint8_t rd) { genRegisters[rd].value = Op; }

void movk(uint64_t Op, uint8_t rd, uint8_t hw, bool Xn) {
  int64_t val = genRegisters[rd].value;
  int64_t lower = getSubWord(0, (hw * 16) - 1, val);
  int64_t upper = getSubWord((hw + 1) * 16, 63, val) << ((hw + 1) * 16);
  int64_t result = lower + Op + upper;
  genRegisters[rd].value = Xn ? result : (int32_t)result;
}