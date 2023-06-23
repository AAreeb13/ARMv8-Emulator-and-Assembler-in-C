
#ifndef ARMV8_2_BRANCHING_H
#define ARMV8_2_BRANCHING_H
#include <stdint.h>
#include "structures.h"
#include "assemble.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

extern uint32_t unconditionalOffsetA(Node);
extern uint32_t unconditionalRegisterA(Node);
extern uint32_t conditionalBranchesA(Node);
extern void giveSymTableBranch(symbolTable table);


//extern symbolTable mainSymTable;
#endif //ARMV8_2_BRANCHING_H