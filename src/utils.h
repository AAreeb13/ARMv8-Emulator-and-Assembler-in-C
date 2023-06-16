#ifndef ARM_PROJECT_UTILS_H
#define ARM_PROJECT_UTILS_H
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"

extern void putBits(uint32_t *, uint32_t, uint8_t);
extern void parseReg(char *, uint8_t *, uint8_t *);
extern void parseLiteral(char *, uint32_t *);
extern void insertArg(Node, char *, int);
extern void changeType(Node, char *);

#endif //ARM_PROJECT_UTILS_H
