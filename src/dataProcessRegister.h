#ifndef ARM_PROJECT_DATAPROCESSREGISTER_H
#define ARM_PROJECT_DATAPROCESSREGISTER_H
#include <stdint.h>
#include "structures.h"
#include "utils.h"

static void parseArgs(Node, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
static void composeWord(uint32_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
extern uint32_t arithmeticReg(Node);

#endif //ARM_PROJECT_DATAPROCESSREGISTER_H