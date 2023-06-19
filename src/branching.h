
#ifndef ARMV8_2_BRANCHING_H
#define ARMV8_2_BRANCHING_H
#include <stdint.h>
#include "structures.h"

uint32_t unconditionalOffsetA(Node);
uint32_t unconditionalRegisterA(Node);
uint32_t conditionalBranchesA(Node);

#endif //ARMV8_2_BRANCHING_H