//
// Created by iu22 on 13/06/23.
//
#include <string.h>
#include "utils.h"


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







