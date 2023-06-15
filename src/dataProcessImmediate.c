#include "dataProcessImmediate.h"

// processes registers and operand
void parseArgs(Node node, uint8_t *sf, uint8_t *sh, uint16_t *imm, uint8_t *rn, uint8_t *rd) {
  *sh = node -> num > 3 ? 1 : 0;
  parseReg(node -> args[0], sf, rd);
  parseReg(node -> args[1], sf, rn);
  parseLiteral(node -> args[2], imm);
}

// produces the instruction word with given components
void composeWord(uint32_t *result, uint8_t sf, uint8_t opc, uint8_t type, uint8_t opi, uint8_t sh, uint16_t imm, uint8_t rn, uint8_t rd) {
  putBits(result, sf, 31);
  putBits(result, opc, 29);
  putBits(result, type, 26);
  putBits(result, opi, 23);
  putBits(result, sh, 22);
  putBits(result, imm, 10);
  putBits(result, rn, 5);
  putBits(result, rd, 0);
}

uint32_t arithmetic(Node node) {
  uint32_t result = 0;
  uint8_t sf;
  uint8_t opc;
  uint8_t opi = 0b010;
  uint8_t sh;
  uint16_t imm;
  uint8_t rn;
  uint8_t rd;
  if (strcmp("add", node -> type) == 0) {
    opc = 0b00;
  } else if (strcmp("adds", node -> type) == 0) {
    opc = 0b01;
  } else if (strcmp("sub", node -> type) == 0) {
    opc = 0b10;
  } else {
    opc = 0b11;
  }
  parseArgs(node, &sf, &sh, &imm, &rn, &rd);
  composeWord(&result, sf, opc, 0b100, opi, sh, imm, rn, rd);
  return result;
}