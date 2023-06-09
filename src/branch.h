//Contains helper and shift functions
#ifndef BRANCH_H
#define BRANCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Branch Instructions functions
extern void branchInstructions(uint32_t);
extern void unconditionalOffset(int64_t);
extern void unconditionalRegister(uint8_t);
extern void conditionalBranches(int64_t, uint8_t);

#endif