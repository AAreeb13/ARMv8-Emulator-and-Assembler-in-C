#include "branching.h"
#include "structures.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

symbolTable mainSymTable1;

void giveSymTableBranch(symbolTable table) {
    mainSymTable1 = table;
}

// b label -> branch to address encoded by literal
uint32_t unconditionalOffsetA(Node node) {
    uint32_t labelAddress = getAddress(mainSymTable1, node->args[0]);
    //printf("label address: %X\n", labelAddress);
    //printf("memory address: %d\n", node->memoryAddress);
    //printf("offset: %d\n", labelAddress - node->memoryAddress);
    uint32_t result;
    result = labelAddress - node->memoryAddress;
    result = result / 4;
    putBits(&result, 0b101, 26);
    //printf("result: %d\n", result);
    return result;
}

// br xn -> branch to address in Xn (??)
uint32_t unconditionalRegisterA(Node node) {
    uint32_t result = 0;
    putBits(&result, 0b1101011, 25);
    putBits(&result, 0b11111, 16);
    uint32_t regValue = 0;
    uint8_t dummySf = 0;
    parseReg(node->args[0], &dummySf, &regValue);
    printf("reg value = %d \n", regValue);
    putBits(&result, regValue, 5);
    return result;
}


// b.cond label -> branch to literal iff PSTATE satisfied cond
// takes in literal address, not the label address
// gives Node type as b.cond
uint32_t conditionalBranchesA(Node node) {
    // need symbol table to get label address
    char *mnemonic = node->type;
    uint32_t labelAddress = getAddress(mainSymTable1, node->args[0]);
    uint32_t result = 0;
    int32_t simm19 = 0;
    uint8_t cond = 0;

    if (!strcmp(mnemonic, "b.eq")) {cond = 0b0000;}
    else if (!strcmp(mnemonic, "b.ne")) {cond = 0b0001;}
    else if (!strcmp(mnemonic, "b.ge")) {cond = 0b1010;}
    else if (!strcmp(mnemonic, "b.lt")) {cond = 0b1011;}
    else if (!strcmp(mnemonic, "b.gt")) {cond = 0b1100;}
    else if (!strcmp(mnemonic, "b.le")) {cond = 0b1101;}
    else if (!strcmp(mnemonic, "b.al")) {cond = 0b1110;}
    printf("cond: %d\n", cond);

    simm19 = (labelAddress - node->memoryAddress) / 4;
    simm19 = simm19 & 0b1111111111111111111;

    putBits(&result, simm19, 5);
    putBits(&result, 0b10101, 26);
    //result += 0b1010 << 26;
    result += cond;
    //putBits(&result, ((labelAddress - node->memoryAddress) / 4), 5);
    //putBits(&result, cond, 0);

    return result;
}