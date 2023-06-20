#include "dataProcessRegister.h"

// processes registers for multiply()
static void parseRegs(Node node, uint8_t *sf, uint8_t *rm, uint8_t *ra, uint8_t *rn, uint8_t *rd) {
  parseReg(node -> args[0], sf, rd);
  parseReg(node -> args[1], sf, rn);
  parseReg(node -> args[2], sf, rm);
  parseReg(node -> args[3], sf, ra);
}

// processes registers and operand for arith_or_logic()
static void parseArgs(Node node, uint8_t *sf, uint8_t *shift, uint8_t *rm, uint8_t *operand, uint8_t *rn, uint8_t *rd) {
  parseReg(node -> args[0], sf, rd);
  parseReg(node -> args[1], sf, rn);
  parseReg(node -> args[2], sf, rm);
  *shift = node -> num <= 3 || !strcmp("lsl", node -> args[3]) ? 0
      : !strcmp("lsr", node -> args[3]) ? 0b01
      : !strcmp("asr", node -> args[3]) ? 0b10
      : 0b11; /*ror*/
  if (node -> num <= 3) {
    *operand = 0;
  } else {
    parseLiteral(node -> args[4], operand);
  }
}

// produces the instruction word with given components for arith_or_logic()
static void composeWord(uint32_t *result, uint8_t sf, uint8_t opc, uint8_t type, uint8_t bit24, uint8_t shift, uint8_t N,uint8_t rm, uint8_t operand, uint8_t rn, uint8_t rd) {
  putBits(result, sf, 31);
  putBits(result, opc, 29);
  putBits(result, type, 25);
  putBits(result, bit24, 24);
  putBits(result, shift, 22);
  putBits(result, N, 21);
  putBits(result, rm, 16);
  putBits(result, operand, 10);
  putBits(result, rn, 5);
  putBits(result, rd, 0);
}

// produces the instruction word with given components for multiply()
static void composeWordMul(uint32_t *result, uint8_t sf, uint16_t section, uint8_t rm, uint8_t x, uint8_t ra, uint8_t rn, uint8_t rd) {
  putBits(result, sf, 31);
  putBits(result, section, 21);
  putBits(result, rm, 16);
  putBits(result, x, 15);
  putBits(result, ra, 10);
  putBits(result, rn, 5);
  putBits(result, rd, 0);
}

// deals with all arithmetic register and logical cases
uint32_t arith_or_logic(Node node) {
  uint32_t result = 0;
//  uint8_t sf;
//  uint8_t opc;
//  uint8_t type = 0b0101; /*includes M bit*/
//  uint8_t bit24;
//  uint8_t shift;
//  uint8_t N;
//  uint8_t rm;
//  uint8_t operand;
//  uint8_t rn;
//  uint8_t rd;
  uint8_t *sf = malloc(sizeof(uint8_t));
  uint8_t opc;
  uint8_t type = 0b0101; /*includes M bit*/
  uint8_t bit24;
  uint8_t *shift = malloc(sizeof(uint8_t));
  uint8_t N;
  uint8_t *rm = malloc(sizeof(uint8_t));
  uint8_t *operand = malloc(sizeof(uint8_t));
  uint8_t *rn = malloc(sizeof(uint8_t));
  uint8_t *rd = malloc(sizeof(uint8_t));
  char *op = node -> type;
  if (!strcmp("add", op) || !strcmp("and", op) || !strcmp("bic", op)) {
    opc = 0b00;
    bit24 = !strcmp("add", op) ? 1 : 0;
    N = strcmp("bic", op) ? 0 : 1;
  } else if (!strcmp("adds", op) || !strncmp("or", op, 2)) {
    opc = 0b01;
    bit24 = !strcmp("adds", op) ? 1 : 0;
    N = strcmp("orn", op) ? 0 : 1;
  } else if (!strcmp("sub", op) || !strncmp("eo", op, 2)) {
    opc = 0b10;
    bit24 = !strcmp("sub", op) ? 1 : 0;
    N = strcmp("eon", op) ? 0 : 1;
  } else /* subs / ands / bics */ {
    opc = 0b11;
    bit24 = !strcmp("subs", op) ? 1 : 0;
    N = strcmp("bics", op) ? 0 : 1;
  }
  parseArgs(node, sf, shift, rm, operand, rn, rd);
  composeWord(&result, *sf, opc, type, bit24, *shift, N, *rm, *operand, *rn, *rd);
  free(sf); free(shift); free(rm); free(operand); free(rn); free(rd);
  return result;
}

// deals with madd and msub
uint32_t multiply(Node node) {
  uint32_t result = 0;
  uint8_t sf;
  uint16_t section = 0b0011011000; /* bits 30 to 21 */
  uint8_t rm;
  uint8_t x = strcmp("madd", node -> type) ? 1 : 0;
  uint8_t ra;
  uint8_t rn;
  uint8_t rd;
  parseRegs(node, &sf, &rm, &ra, &rn, &rd);
  composeWordMul(&result, sf, section, rm, x, ra, rn, rd);
  return result;
}