//
// Created by aaa1322 on 14/06/23.
//

#include "datatransfer.h"
#include "structures.h"
#include <stdint.h>
#include "utils.h"

//Pre args = { "Xn || Wn", "[Xm ,..]!" || <literal> || }.
uint32_t singleDataTransfer(Node instruction) {
    uint8_t totalBitsAdded = 0;
    char **args = instruction->args;
    uint32_t rt = regAddr(args[0]); // need to remove "x" or "w" //bits 0 to 4.
    uint32_t binary = 0; //initialise binary representation of instruction to 0.
    uint32_t bits25to28 = 0b1100;//bits 25 to 28.
    uint32_t isDataTransfer = args[1][0] == '['; //also the bit in position 31 and 29.
    uint32_t bit30 = args[0][0] == 'X'; //also known as sf bit.


    putBits(&binary,rt,0); totalBitsAdded+=5;
    putBits(&binary,bits25to28,25); totalBitsAdded+=4;
    putBits(&binary,isDataTransfer,29); totalBitsAdded++;
    putBits(&binary,bit30,30); totalBitsAdded++;
    putBits(&binary, isDataTransfer,31); totalBitsAdded++;
    if (isDataTransfer==1) {
        uint32_t isLoad = instruction->type == "ldr";//bit in position 22.
        putBits(&binary, isLoad, 22); totalBitsAdded++;
        putBits(&binary,0,23); totalBitsAdded++;
        uint32_t *xn = args[1]; xn++; // no longer pointing to ']' but to 'x' or 'w'.
        uint32_t offset;
        if (args ==2) {
            offset = zeroUnsignedOffset();

        }

        putBits(&binary,offset,10);totalBitsAdded+=12; // offset added
    }else{ //Load Literal

    }
    return binary;
}

uint32_t zeroUnsignedOffset() {
    return 0;
}






