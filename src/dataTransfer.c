
#include <stdio.h>
#include "dataTransfer.h"
#include "structures.h"
#include "utils.h"

symbolTable mainSymTable2;

uint32_t getRegValue(char *r){
  return atoi(++r);
}

void giveSymTableTransfer(symbolTable table) {
  mainSymTable2 = table;
}

//Pre args = { "ldr" || "str","Xn || Wn", "[Xm ,..]!" || <literal> || }.
uint32_t singleDataTransfer(Node instruction) {
  uint8_t totalBitsAdded = 0;
  uint32_t bit30 = 0; //also known as sf bit.
  uint32_t rt = 0;
  //parseReg(instruction -> args[0],(uint8_t *) &rt,(uint8_t *)&bit30);// need to remove "x" or "w" //bits 0 to 4.
  parseReg(instruction -> args[0],(uint8_t *)&bit30,(uint8_t *)&rt);
  uint32_t binary = 0; //initialise binary representation of instruction to 0.
  uint32_t bits25to28 = 0b1100;//bits 25 to 28.
  uint32_t isDataTransfer = (instruction -> args[1][0] == '[') ? 1 : 0b0; //also the bit in position 31 and 29.

  putBits(&binary,rt,0); totalBitsAdded+=5;
  putBits(&binary,bits25to28,25); totalBitsAdded+=4;
  putBits(&binary,isDataTransfer,29); totalBitsAdded++;
  putBits(&binary,bit30,30); totalBitsAdded++;
  putBits(&binary,isDataTransfer,31);totalBitsAdded++;

  if (isDataTransfer==1) {// i.e not load from a literal
    uint32_t isLoad =!strcmp(instruction->type , "ldr");//bit in position 22.
    putBits(&binary, isLoad, 22); totalBitsAdded++;
    putBits(&binary,0,23); totalBitsAdded++;

    uint32_t xn=0;
    parseReg(++(instruction -> args[1]), (uint8_t *)&rt,(uint8_t *)&xn); --(instruction -> args[1]);
    putBits(&binary,xn,5);totalBitsAdded+=5;
    uint32_t offset = 0;

    if (instruction->num == 2) { // unsigned offset is 0
      offset = zeroUnsignedOffset();
      putBits(&binary,1,24); //U = 1
    }else if (instruction -> args[2][0] == '#' && instruction -> args[2][strlen(instruction -> args[2])-1] == ']'){ //unsigned offset with address code [xn, #<imm>]
      offset = unsignedImmediateOffset(instruction);
      putBits(&binary,1,24); //U = 1
    }else if (instruction -> args[2][0] == '#' && instruction -> args[2][strlen(instruction -> args[2])-1] == '!'){
      offset = preIndexed(instruction );
    }else if(instruction -> args[2][0] == '#'&&
    instruction -> args[2][strlen(instruction -> args[2])-1] != '!'&&
    instruction -> args[2][strlen(instruction-> args[2] - 1)]){
      offset = postIndexed(instruction);
    }else if (instruction -> args[1][1] == 'x' && instruction-> args[2][0] == 'x'){
      offset = registerOffset(instruction);
    }else{
      return binary;
    }
    putBits(&binary,offset,10);totalBitsAdded+=12; // offset added
  }else{ //Load Literal
    loadLiteral(&binary,instruction);
  }
    return binary;
}

uint32_t zeroUnsignedOffset() {
    return 0;
}
uint32_t unsignedImmediateOffset(Node instruction){
  uint8_t isX = instruction -> args[0][0] == 'x';
  char strImm[10];
  strncpy(strImm,instruction -> args[2],strlen(instruction -> args[2])-1);
  uint8_t n = (isX) ? 3 : 2;
  uint32_t imm12;parseLiteral(strImm,&imm12);
  imm12 = imm12 >> n;
  return imm12;
}
uint32_t preIndexed(Node instruction){
  uint32_t offset = 0 + 0b11; //added bits 11, 10
  char strSimm[5];
  strncpy(strSimm,instruction -> args[2],strlen(instruction -> args[2])-2); // provide signed literal in str, e.g "#1234" or "#-1234"
  uint32_t simm9; parseLiteral(strSimm,&simm9);
  simm9 = simm9 & 0b111111111;
  putBits(&offset, simm9, 2);
  return offset;

}
uint32_t postIndexed(Node instruction){
  uint32_t offset = 0 + 0b01; //added bits 11, 10
  uint32_t simm9 = 0;
  parseLiteral(instruction -> args[2],&simm9);
  simm9 = simm9 & 0b111111111;
  putBits(&offset, simm9, 2);
  return offset;
}
uint32_t registerOffset(Node instruction){
  uint32_t offset= 0;
  char xm[5];
  strncpy(xm,instruction -> args[2],strlen(instruction -> args[2])-1);
  xm[strlen(instruction -> args[2])-1] = '\0';
  uint8_t xmValue = 0;
  uint8_t redundantsf = 0;
  parseReg(xm,&redundantsf, &xmValue);
  putBits(&offset,xmValue,6);
  putBits(&offset, 0b100000011010,0);
  return offset;
}
void loadLiteral(uint32_t *binary, Node instruction){
  uint32_t  simm19 = 0;
  if(instruction -> args[1][0] == '#'){
    parseLiteral(instruction -> args[1],&simm19);
    simm19 = simm19 & 0b1111111111111111111;
  }else{
    simm19 = getAddress(mainSymTable2, instruction -> args[1]);
    simm19 = (simm19 - instruction -> memoryAddress ) >> 2;
  }
  simm19 = simm19 & 0b1111111111111111111;
  putBits(binary,simm19,5);

}


//INTEGER literal LABEL
uint32_t literal(Node instruction){
  char *strNum = instruction->args[0];
  char literal[strlen(strNum)+1];
  literal[0] = '#'; literal[1] = '\0';
  strcat(literal,strNum);
  uint32_t value;
  parseLiteral(literal, &value);
  return value;
}
