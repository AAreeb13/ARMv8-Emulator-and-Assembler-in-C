//
// Created by sa1722 on 15/06/23.
//

#include "branching.h"
#include "structures.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

// b label -> branch to address encoded by literal
uint32_t unconditionalOffsetA(Node node) {
    // need symbol table to get label address
    uint32_t labelAddress;
    uint32_t result;
    putBits(&result, 0b101, 26);
    result += (labelAddress - node.memoryAddress) / 4);
    return result;
}

// br xn -> branch to address in Xn (??)
uint32_t unconditionalRegisterA(Node node) {
    uint32_t result;
    putBits(&result, 0b1101011, 26);
    putBits(&result, 0b11111, 16);
    putBits(&result, parseReg(node.args[0], 0, 0), 5);
    return result;
}

// b.cond label -> branch to literal iff PSTATE satisfied cond
uint32_t conditionalBranchesA(Node node, char mnemonic[]) {
    // need symbol table to get label address
    uint32_t labelAddress;
    uint32_t result;
    uint8_t cond;

    if (strcmp(mnemonic, "eq")) {cond = 0b0000}
    else if (strcmp(mnemonic, "ne")) {cond = 0b0001}
    else if (strcmp(mnemonic, "ge")) {cond = 0b1010}
    else if (strcmp(mnemonic, "lt")) {cond = 0b1011}
    else if (strcmp(mnemonic, "gt")) {cond = 0b1100}
    else if (strcmp(mnemonic, "le")) {cond = 0b1101}
    else if (strcmp(mnemonic, "al")) {cond = 0b1110}

    putBits(&result, 0b10101, 26);
    putBits(&result, (labelAddress - node.memoryAddress) / 4), 5);
    putBits(&result, cond, 0);

    return result;
}