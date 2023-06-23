#ifndef ARM_PROJECT_UTILS_H
#define ARM_PROJECT_UTILS_H
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"


extern char typeArray[39][5];
extern int labelCheck(char *word);
extern char **createMallocedStrings(int count, int size);
extern void freeMallocedStrings(char **strings, int count);
extern funcPtrTable createMainFuncTable();

extern void putBits(uint32_t *, uint32_t, uint8_t);
extern void parseReg(char *, uint8_t *, uint8_t *);
extern void parseLiteral(char *, uint32_t *);
extern uint32_t parseHex(char *);

#endif //ARM_PROJECT_UTILS_H
