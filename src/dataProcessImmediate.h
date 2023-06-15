#ifndef ARM_PROJECT_DATAPROCESSIMMEDIATE_H
#define ARM_PROJECT_DATAPROCESSIMMEDIATE_H
#include <stdint.h>
#include "utils.h"
#include "structures.h"

extern void parseArgs(Node node, uint8_t *, uint8_t *, uint16_t *, uint8_t *, uint8_t *);
extern void composeWord(uint32_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint8_t, uint8_t);
extern uint32_t arithmetic(Node);

#endif //ARM_PROJECT_DATAPROCESSIMMEDIATE_H
