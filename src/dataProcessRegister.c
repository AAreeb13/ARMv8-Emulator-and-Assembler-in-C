#include <stdint.h>
#include <stdbool.h>
#include "dataProcessRegister.h"
#include "misc.h"
#include "structure.h"
#include "dataProcessImmediate.h"

void dataProcessRegister(uint32_t word) {
  uint8_t M = getBit(word, 28);
  uint8_t opr = getSubWord(21, 24, word);
  uint8_t sf = getBit(word, 31);
  if (M == 0 && (getBit(opr, 3) == 1)) {
    arithmeticReg(word, sf);
  } else if (M == 1) {
    multiply(word, sf);
  } else {
    logical(word, sf);
  }
}

void arithmeticReg(uint32_t word, bool Xn) {
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t rd = getSubWord(0, 4, word);
  uint8_t operand = getSubWord(10, 15, word);
  uint8_t shift = getSubWord(22, 23, word);
  uint8_t opc = getSubWord(29, 30, word);
  uint64_t Op_2;
  switch (shift) {
    case 0: /*0b00*/
      Op_2 = lsl(genRegisters[rm].value, operand, Xn);
      break;
    case 1: /*0b01*/
      Op_2 = lsr(genRegisters[rm].value, operand, Xn);
      break;
    default: /*0b10*/
      Op_2 = asr(genRegisters[rm].value, operand, Xn);
      break;
  }

  switch (opc) {
    case 0: /*0b00*/
      addReg(rn, Op_2, rd, Xn);
      break;
    case 1: /*0b01*/
      adds(rn, Op_2, rd, Xn);
      break;
    case 2: /*0b10*/
      sub(rn, Op_2, rd, Xn);
      break;
    default:
      subs(rn, Op_2, rd, Xn); /*0b11*/
      break;
  }
}

void logical(uint32_t word, bool Xn) {
  uint8_t rd = getSubWord(0, 4, word);
  if (rd == 31)
    return;
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t operand = getSubWord(10, 15, word);
  uint8_t shift = getSubWord(22, 23, word);
  uint8_t opc = getSubWord(29, 30, word);
  uint8_t N = getBit(word, 21);
  uint64_t Op_2;
  switch (shift) {
    case 0: /*0b00*/
      Op_2 = lsl(genRegisters[rm].value, operand, Xn);
      break;
    case 1: /*0b01*/
      Op_2 = lsr(genRegisters[rm].value, operand, Xn);
      break;
    case 2: /*0b10*/
      Op_2 = asr(genRegisters[rm].value, operand, Xn);
      break;
    default: /*0b11*/
      Op_2 = ror(genRegisters[rm].value, operand, Xn);
      break;
  }

  switch (opc) {
    case 0: /*0b00*/
      genRegisters[rd].value = N ? bic(rn, Op_2, Xn) : and(rn, Op_2, Xn);
      break;
    case 1: /*0b01*/
      genRegisters[rd].value = N ? orn(rn, Op_2, Xn) : orr(rn, Op_2, Xn);
      break;
    case 2: /*0b10*/
      genRegisters[rd].value = N ? eon(rn, Op_2, Xn) : eor(rn, Op_2, Xn);
      break;
    default: /*0b11*/
      genRegisters[rd].value = N ? bics(rn, Op_2, Xn) : ands(rn, Op_2, Xn);
      break;
  }
}

int64_t and (int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 & Op_32;
  }
  return genRegisters[rn].value & Op_2;
}

int64_t bic(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 & (~Op_32);
  }
  return genRegisters[rn].value & (~Op_2);
}

int64_t orr(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 | Op_32;
  }
  return genRegisters[rn].value | Op_2;
}

int64_t orn(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 | (~Op_32);
  }
  return genRegisters[rn].value | (~Op_2);
}

int64_t eor(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 ^ Op_32;
  }
  return genRegisters[rn].value ^ Op_2;
}

int64_t eon(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 ^ (~Op_32);
  }
  return genRegisters[rn].value ^ (~Op_2);
}

int64_t ands(int64_t rn, uint64_t Op_2, bool Xn) {
  int64_t result = and(rn, Op_2, Xn);
  pState.N = Xn ? getBit(result, 63) : getBit(result, 31);
  pState.Z = result ? pState.Z : 1;
  pState.C = 0;
  pState.V = 0;
  return result;
}

int64_t bics(int64_t rn, uint64_t Op_2, bool Xn) {
  int64_t result = bic(rn, Op_2, Xn);
  pState.N = Xn ? getBit(result, 63) : getBit(result, 31);
  pState.Z = result ? pState.Z : 1;
  pState.C = 0;
  pState.V = 0;
  return result;
}

void multiply(uint32_t word, bool Xn) {
  //  assert(getSubWord(29, 30, word) == 0);
  uint8_t rd = getSubWord(0, 4, word);
  if (rd == 31)
    return;
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t ra = getSubWord(10, 14, word);
  uint8_t x = getBit(word, 15);
  if (!Xn) {
    uint32_t rm_32 = genRegisters[rm].value;
    uint32_t rn_32 = genRegisters[rn].value;
    uint32_t ra_32 = genRegisters[rn].value;
    uint32_t result = x ? ra_32 - (rn_32 * rm_32) : ra_32 + (rn_32 * rm_32);
    genRegisters[rd].value = result;
  } else {
    genRegisters[rd].value =
        x ? genRegisters[ra].value -
            (genRegisters[rn].value * genRegisters[rm].value)
          : genRegisters[ra].value +
            (genRegisters[rn].value * genRegisters[rm].value);
  }
}