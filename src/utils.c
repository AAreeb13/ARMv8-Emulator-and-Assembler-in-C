#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"

// puts bits in instruction word in desired location
// @param endIndex - index of last bit in @param bits
void putBits(uint32_t *total, uint32_t bits, uint8_t endIndex) {
  *total |= bits << endIndex;
}

// processes register argument
void parseReg(char *reg, uint8_t *sf, uint8_t *regValue) {
  *sf = reg[0] == 'x' ? 1 : 0;
  *regValue = strchr(reg, 'z') != NULL ? 0b11111 : atoi(++reg);
}

// processes literal
// @param literal can only be hex or decimal integer
void parseLiteral(char *literal, uint32_t *imm) {
  if (literal[2] == 'x') {
    if (sscanf(++literal, "%x", imm) != 1) {
      printf("Parsing literal failed!");
    }
  } else {
    if (sscanf(++literal, "%d", imm) != 1) {
      printf("Parsing literal failed!");
    }
  }
}

uint32_t parseHex(char hexString[]) {
    char *endPtr;
    uint32_t value;

    // Convert the hexadecimal string to a decimal value
    value = strtol(hexString, &endPtr, 16);

    // Check if conversion was successful
    if (*endPtr != '\0') {
        printf("Invalid hexadecimal string\n");
    }

    return value;
}
// typeArray[num of elems][length] - Contains all the types
// Needed for parsing and determining if a word is a label or not
char typeArray[33][5] = {"add", "adds", "sub", "subs", "cmp", "cmn",
                         "neg", "negs", "and", "ands", "bic", "bics",
                         "eor", "orr", "eon", "orn", "tst", "movk",
                         "movn", "movz", "mov", "mvn", "madd", "msub",
                         "mul", "mneg", "b", "b.", "br", "str",
                         "ldr", "nop", ".int"};

// Checks if given string is label by checking against typeArray
int labelCheck(char *word) {
  for (int i = 0; i < 33; i++) {
    if (strcmp(word, typeArray[i]) == 0) {
      return 0;
    }
  }
  return 1;
}

uint8_t regAddr(char *reg) {
    reg++;
    return atoi(reg);
}
