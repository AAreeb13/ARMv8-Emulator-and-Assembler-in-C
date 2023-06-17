#include "utils.h"
#include <stdbool.h>

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

// inserts @param arg into @param index in @param node->args
void insertArg(Node node, char *arg, int index) {
  node -> num++;
  node -> args = realloc(node -> args, sizeof(char *) * node -> num);
  for (int i = (node -> num) - 1; i > index; i--) {
    int len = strlen(node -> args[i - 1]) + 1;
    node -> args[i] = realloc(node -> args[i], sizeof(char) * len);
    strcpy(node -> args[i], node -> args[i - 1]);
  }
  node -> args[index] = realloc(node -> args[index], sizeof(char) * (strlen(arg) + 1));
  strcpy(node -> args[index], arg);
}

// changes the type of @param node->type to @param type
void changeType(Node node, char *type) {
  int len = strlen(type) + 1;
  node -> type = realloc(node -> type, len);
  strcpy(node -> type, type);
}