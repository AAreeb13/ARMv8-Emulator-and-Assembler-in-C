#include "dataProcessImmediate.h"

// processes registers and operand
static void parseArgsArithmetic(Node node, uint8_t *sf, uint8_t *sh, uint16_t *imm, uint8_t *rn, uint8_t *rd) {
  if (node -> num <= 3) {
    *sh = 0;
  } else {
    uint8_t shift;
    parseLiteral(node -> args[4], &shift);
    *sh = shift == 0 ? 0 : 1;
  }
  parseReg(node -> args[0], sf, rd);
  parseReg(node -> args[1], sf, rn);
  parseLiteral(node -> args[2], imm);
}

static void parseArgsWideMove(Node node, uint8_t *sf, uint8_t *hw, uint16_t *imm, uint8_t *rd) {
  if (node -> num <= 2) {
    *hw = 0;
  } else {
    uint8_t shift;
    parseLiteral(node -> args[3], &shift);
    *hw = shift / 16;
  }
  parseReg(node -> args[0], sf, rd);
  parseLiteral(node -> args[1], imm);
}

// produces the instruction word with given components
static void composeWordArithmetic(uint32_t *result, uint8_t sf, uint8_t opc, uint8_t type, uint8_t opi, uint8_t sh, uint16_t imm, uint8_t rn, uint8_t rd) {
  putBits(result, sf, 31);
  putBits(result, opc, 29);
  putBits(result, type, 26);
  putBits(result, opi, 23);
  putBits(result, sh, 22);
  putBits(result, imm, 10);
  putBits(result, rn, 5);
  putBits(result, rd, 0);
}

static void composeWordWideMove(uint32_t *result, uint8_t sf, uint8_t opc, uint8_t type, uint8_t opi, uint8_t hw, uint16_t imm, uint8_t rd) {
  putBits(result, sf, 31);
  putBits(result, opc, 29);
  putBits(result, type, 26);
  putBits(result, opi, 23);
  putBits(result, hw, 21);
  putBits(result, imm, 5);
  putBits(result, rd, 0);
}

// Data Processing Immediate Arithmetic - deals with add, adds, sub, subs
uint32_t arithmeticImm(Node node) {
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
  } else /*subs*/ {
    opc = 0b11;
  }
  parseArgsArithmetic(node, &sf, &sh, &imm, &rn, &rd);
  composeWordArithmetic(&result, sf, opc, 0b100, opi, sh, imm, rn, rd);
  return result;
}

// Data Processing Wide Move - deals with movn, movz, movk
uint32_t wideMove(Node node) {
  uint32_t result = 0;
  uint8_t sf;
  uint8_t opc;
  uint8_t opi = 0b101;
  uint8_t hw;
  uint16_t imm;
  uint8_t rd;
  if (strcmp("movn", node -> type) == 0) {
    opc = 0b00;
  } else if (strcmp("movz", node -> type) == 0) {
    opc = 0b10;
  } else /*movk*/ {
    opc = 0b11;
  }
  parseArgsWideMove(node, &sf, &hw, &imm, &rd);
  composeWordWideMove(&result, sf, opc, 0b100, opi, hw, imm, rd);
  return result;
}

// ALIASES

// cmp rn, <op2> = subs rzrm rn, <op2>
uint32_t compare(Node node) {
    char** newArgs; // NEED TO ADD RZR AT THE FRONT
    Node copy = createNode(node.memoryAddress,
                           node.type,
                           newArgs,
                           num + 1);
    uint32_t result = arithmeticImm(copy);
    freeNode(copy);
    return result;
}

// cmp rn, <op2> = subs rzrm rn, <op2>
// cmn rn, <op2> = adds rzrm rn, <op2>
uint32_t compare(Node node) {
    char** newArgs[num+1][5];
    newArgs[0] = "11111"; // adding RZR at the front
    for (int i = 0; i < num; i++) {
        newArgs[i + 1] = node->args[i]
    }

    Node copy = createNode(node.memoryAddress,
                           strcmp(node.type, "cmp") ? "subs" : "adds",
                           newArgs,
                           num + 1);
    uint32_t result = arithmeticImm(copy);
    freeNode(copy);
    return result;
}

// neg(s) rd, <op2> = sub(s) rd, rzr, <op2>
uint32_t negate(Node node) {
    char** newArgs[num+1][5];
    newArgs[0] = node->args[0];
    newArgs[1] = "11111"; // Adding RZR in the middle
    if (num == 2) {
        newArgs[2] = node->args[2];
    }

    Node copy = createNode(node.memoryAddress,
                           strcmp(node.type, "neg") ? "sub" : "subs",
                           newArgs,
                           num + 1);
    uint32_t result = arithmeticImm(copy);
    freeNode(copy);
    return result;
}


// tst rn, <op2> = ands rzr, rn, <op2>
uint32_t testBits(Node node) {
    char** newArgs[num+1][5];
    newArgs[0] = "11111"; // adding RZR at the front
    for (int i = 0; i < num; i++) {
        newArgs[i + 1] = node->args[i]
    }

    Node copy = createNode(node.memoryAddress,
                           "ands",
                           newArgs,
                           num + 1);
    uint32_t result = arithmeticImm(copy);
    freeNode(copy);
    return result;
}