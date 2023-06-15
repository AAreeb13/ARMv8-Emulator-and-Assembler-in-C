//
// Created by iu22 on 13/06/23.
//

#include "utils.h"
#include <stdbool.h>

// puts bits in instruction word in desired location
// @param endIndex - index of last bit in @param bits
void putBits(uint32_t *total, uint32_t bits, uint8_t endIndex) {
    *total |= bits << endIndex;
}

// processes register argument
void parseReg(char *reg, uint8_t *sf, uint8_t *regValue) {
    *sf = reg[0] == 'x' ? 1 : 0;
    *regValue = atoi(++reg);
}

// processes literal
// @param literal can only be hex or decimal integer
void parseLiteral(char *literal, uint32_t *imm) {
    if (literal[2] == 'x') {
        if (sscanf(++literal, "%x", imm) != 1) {
            printf("Parsing literal failed!");
        }
    } else {
        if (sscanf(++literal, "%d", imm) != 1) {
            printf("Parsing literal failed!");
        }
    }
}

uint32_t parseHex(char hexString[]) {
    char *endPtr;
    uint32_t value;

    // Convert the hexadecimal string to a decimal value
    value = strtol(hexString, &endPtr, 16);

    // Check if conversion was successful
    if (*endPtr != '\0') {
        printf("Invalid hexadecimal string\n");
    }

    return value;
}

