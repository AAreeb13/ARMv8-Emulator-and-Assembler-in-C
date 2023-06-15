#include "dataProcessRegister.h"


static void parseArgs(Node node, uint8_t *sf, uint8_t *opr, uint8_t *rm, uint8_t *operand, uint8_t *rn, uint8_t *rd) {
  parseReg(node -> args[0], sf, rd);
  parseReg(node -> args[1], sf, rn);
  parseReg(node -> args[2], sf, rm);
  if (node -> num <= 3) {
    *opr = 0b1000;
    *operand = 0;
  } else if (!strcmp("lsl", node -> args[3])) {
    *opr = 0b1000;
    parseLiteral(node -> args[4], operand);
  } else if (!strcmp("lsr", node -> args[3])) {
    *opr = 0b1010;
    parseLiteral(node -> args[4], operand);
  } else /*asr*/ {
    *opr = 0b1100;
    parseLiteral(node -> args[4], operand);
  }
}

static void composeWord(uint32_t *result, uint8_t *sf, uint8_t *opc, uint8_t *type, uint8_t *opr, uint8_t *rm, uint8_t *operand, uint8_t *rn, uint8_t *rd) {
  putBits(result, sf, 31);
  putBits(result, opc, 29);
  putBits(result, type, 25);
  putBits(result, opr, 21);
  putBits(result, rm, 16);
  putBits(result, operand, 10);
  putBits(result, rn, 5);
  putBits(result, rd, 0);
}

// Data Processing Register Arithmetic - deals with add, adds, sub, subs
uint32_t arithmeticReg(Node node) {
  uint32_t result = 0;
  uint8_t sf;
  uint8_t opc;
  uint8_t type = 0b0101; /*includes M bit*/
  uint8_t opr;
  uint8_t rm;
  uint8_t operand;
  uint8_t rn;
  uint8_t rd;
  if (!strcmp("add", node -> type)) {
    opc = 0b00;
  } else if (!strcmp("adds", node -> type)) {
    opc = 0b01;
  } else if (!strcmp("sub", node -> type)) {
    opc = 0b10;
  } else /*subs*/ {
    opc = 0b11;
  }
  parseArgs(node, &sf, &opr, &rm, &operand, &rn, &rd);
  composeWord(&result, &sf, &opc, &type, &opr, &rm, &operand, &rn, &rd);
  return result;
}