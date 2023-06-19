#include "utils.h"

extern char typeArray[33][5];
extern nodeFunc funcArray[];
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




// Checks if given string is label by checking against typeArray
int labelCheck(char *word) {
  for (int i = 0; i < 33; i++) {
    if (strcmp(word, typeArray[i]) == 0) {
      return 0;
    }
  }
  return 1;
}

char **createMallocedStrings(int count, int size) {
  char **strings = malloc(sizeof(char *) * count);
  if (strings == NULL) {
    printf("Malloc failed when allocating strings");
    return NULL;
  }
  for (int i = 0; i < count; i++) {
    strings[i] = malloc(size);
    if (strings[i] == NULL) {
      printf("Malloc failed when allocating strings[%d]", i);
      for (int j = 0; j < i; j++) {
        free(strings[j]);
      }
    }
  }
  return strings;
}


void freeMallocedStrings(char **strings, int count) {
  for (int i = 0; i < count; i ++) {
    free(strings[i]);
  }
  free(strings);
}

//Creates the table with the needed types and pointers
funcPtrTable createMainFuncTable() {
  funcPtrEntry table[33];
  for (int i = 0; i < 33; i++) {
    table[i] = createFuncEntry(typeArray[i], funcArray[i]);
  }
  return (createFuncTable(33,table));
}

