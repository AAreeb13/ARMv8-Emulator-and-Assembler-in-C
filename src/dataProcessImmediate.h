#ifndef ARM_PROJECT_DATAPROCESSIMMEDIATE_H
#define ARM_PROJECT_DATAPROCESSIMMEDIATE_H
#include <stdint.h>
#include "utils.h"
#include "structures.h"

static void parseArgsArithmetic(Node, uint8_t *, uint8_t *, uint16_t *, uint8_t *, uint8_t *);
static void parseArgsWideMove(Node, uint8_t *, uint8_t *, uint16_t *, uint8_t *);
static void composeWordArithmetic(uint32_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint8_t, uint8_t);
static void composeWordWideMove(uint32_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint8_t);
extern uint32_t arithmetic(Node);
extern uint32_t wideMove(Node);

#endif //ARM_PROJECT_DATAPROCESSIMMEDIATE_H
