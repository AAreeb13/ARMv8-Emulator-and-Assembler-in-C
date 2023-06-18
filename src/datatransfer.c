
#include <stdio.h>
#include "datatransfer.h"
#include "structures.h"
#include "utils.h"

//Pre args = { "ldr" || "str","Xn || Wn", "[Xm ,..]!" || <literal> || }.
uint32_t singleDataTransfer(Node instruction) {
  uint8_t totalBitsAdded = 0;
  char **args = instruction->args;
  uint32_t bit30; //also known as sf bit.
  uint32_t rt;
  parseReg(args[0],(uint8_t *) &rt,(uint8_t *)&bit30);// need to remove "x" or "w" //bits 0 to 4.
  uint32_t binary = 0; //initialise binary representation of instruction to 0.
  uint32_t bits25to28 = 0b1100;//bits 25 to 28.
  uint32_t isDataTransfer = args[1][0] == '['; //also the bit in position 31 and 29.



  putBits(&binary,rt,0); totalBitsAdded+=5;
  putBits(&binary,bits25to28,25); totalBitsAdded+=4;
  putBits(&binary,isDataTransfer,29); totalBitsAdded++;
  putBits(&binary,bit30,30); totalBitsAdded++;
  putBits(&binary, isDataTransfer,31); totalBitsAdded++;
  if (isDataTransfer==1) {
    uint32_t isLoad = instruction->type == "ldr";//bit in position 22.
    putBits(&binary, isLoad, 22); totalBitsAdded++;
    putBits(&binary,0,23); totalBitsAdded++;

    uint32_t xn;parseReg(args[1], (uint8_t *)&rt,(uint8_t *)&xn);
    putBits(&binary,xn,5);
    uint32_t offset;

    if (instruction->num == 2) { // unsigned offset is 0
      offset = zeroUnsignedOffset();
      putBits(&binary,1,24);
    }else if (args[2][0] == '#' && args[2][strlen(args[2])-1] == ']'){ //unsigned offset with address code [xn, #<imm>]
      offset = unsignedImmediateOffset(args);
    }else if (args[2][0] == '#' && args[2][strlen(args[2])-1] == '!'){
      offset = preIndexed(args);
    }else if(args[2][0] == '#'&&
    args[2][strlen(args[2])-1] != '!'&&
    args[2][strlen(args[2] - 1)]){
      offset = postIndexed(args);
    }else{
      offset = registerOffset(args);
    }
    putBits(&binary,offset,10);totalBitsAdded+=12; // offset added
  }else{ //Load Literal
    loadLiteral(binary,args);
  }
    return binary;
}

uint32_t zeroUnsignedOffset() {
    return 0;
}
uint32_t unsignedImmediateOffset(char **args){
  uint8_t isX = args[0][0] == 'X';
  char strImm[5];
  strncpy(strImm,args[2],strlen(args[2])-1);
  uint8_t n = (isX) ? 3 : 2;
  uint32_t imm12;parseLiteral(strImm,&imm12);
  imm12 = imm12 >> n;
  return imm12;
}
uint32_t preIndexed(char **args){
  uint32_t offset = 0 + 0b11; //added bits 11, 10
  char strSimm[5];
  strncpy(strSimm,args[2],strlen(args[2])-2); // provide signed literal in str, e.g "#1234" or "#-1234"
  uint32_t simm9; parseLiteral(strSimm,&simm9);
  simm9 = simm9 & 0b111111111;
  putBits(&offset, simm9, 2);
  return offset;

}
uint32_t postIndexed(char **args){
  uint32_t offset = 0 + 0b01; //added bits 11, 10
  uint32_t simm9;
  parseLiteral(args[2],&simm9);
  simm9 = simm9 & 0b111111111;
  putBits(&offset, simm9, 2);
  return offset;
}
uint32_t registerOffset(char **args){
  uint32_t offset;
  char xm[5];
  strncpy(xm,args[2],strlen(args[2])-1);
  xm[strlen(args[2])-1] = '\0';
  //uint8_t *redundantsf;
  uint8_t xmValue;
  parseReg(xm,0, &xmValue);
  putBits(&offset,xmValue,6);
  putBits(&offset, 0b100000011010,0);
  return offset;
}
void loadLiteral(uint32_t binary, char **args){
  uint32_t  simm19;
  if(args[2][0] == '#'){
    parseLiteral(args[2],&simm19);
    simm19 = simm19 & 0b1111111111111111111;
  }else{
    symbolTable st;
    simm19 = getAddress(st, args[1]);
    //label
  }
  putBits(&binary,simm19,5);

}





