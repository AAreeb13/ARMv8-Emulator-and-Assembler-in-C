#ifndef ARM_PROJECT_UTILS_H
#define ARM_PROJECT_UTILS_H

<<<<<<< HEAD
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"

extern void putBits(uint32_t *, uint32_t, uint8_t);
extern void parseReg(char *, uint8_t *, uint8_t *);
extern void parseLiteral(char *, uint32_t *);
extern uint32_t parseHex(char *);

extern char typeArray[33][5];
extern int labelCheck(char *word);

||||||| ef8acee
extern void putBits(uint32_t *, uint32_t, uint32_t);

=======
extern void putBits(uint32_t *, uint32_t, uint32_t);
extern uint8_t regAddr(char *);
>>>>>>> Areeb
#endif //ARM_PROJECT_UTILS_H
