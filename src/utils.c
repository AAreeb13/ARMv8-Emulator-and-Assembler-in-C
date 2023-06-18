//
// Created by iu22 on 13/06/23.
//
#include <string.h>
#include "utils.h"
#include <malloc.h>
#include "structures.h"


extern char typeArray[33][5];
extern nodeFunc funcArray[];


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

// Creates the table with the needed types and pointers
funcPtrTable createMainFuncTable() {
  funcPtrEntry table[33];
  for (int i = 0; i < 33; i++) {
    table[i] = createFuncEntry(typeArray[i], funcArray[i]);
  }
  return (createFuncTable(33,table));
}











