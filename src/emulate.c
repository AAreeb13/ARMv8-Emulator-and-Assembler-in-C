#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define WORD_SIZE 32
#define MEMORY_SIZE 2097152

#define NUM_REGS 31
// registers: X00 -> X30
struct Reg {
  char name[4];
  int value; // stands for Xn -> in binary
};

// array of general registers
struct Reg genRegisters[NUM_REGS];

void initialiseGeneralReg() {
  for (int i = 0; i < NUM_REGS; i++) {
    sprintf(genRegisters[i].name, "X%02d", i);
    genRegisters[i].value = 0;
  }
}

// create PC structure
struct Reg PC = {"PC", 0};

// create PState structure
struct PState {
  bool N; // last result (lr) was -ive
  bool Z; // lr was 0
  bool C; // lr caused a bit to be carried out
  bool F; // lr caused overflow
};

struct PState pState = {false, true, false, false};

//bit-accessible functions
unsigned char getBit(unsigned int, int);
void printByte(unsigned char);
unsigned int getSubWord(int,int,unsigned int);
int power(int,int);

//Single Data Transfer functions
void singleDataTransfer(unsigned int);
void unsignedImmediateOffset(unsigned int, int);
void preIndexed(unsigned int, int);
void registerOffset(unsigned int, int);
void loadLiteral(unsigned int, int);

//main memory
unsigned char *memory;

int main(int argc, char **argv) {
  FILE *file;
  // Make sure that there is a second parameter(binary file)
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  // Memory is given 2MB
  memory = (unsigned char *) malloc(MEMORY_SIZE);
  if (memory == NULL) {
    printf("Memory allocation failed.\n");
    return EXIT_FAILURE;
  }
  // Open file
  file = fopen(argv[1], "rb");
  if (file == NULL) {
    printf("Opening file failed.\n");
    return EXIT_FAILURE;
  }

  // File size
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  // Read file into memory
  size_t bytesRead = fread(memory, 1, fileSize, file);
  if (bytesRead != fileSize) {
    printf("Failed to read file.\n");
    free(memory);
    fclose(file);
    return EXIT_FAILURE;
  }

  // Close file
  fclose(file);



  // no. words
  // int numWords = fileSize / sizeof(unsigned int);

  // // process each word
  // for (int i = 0; i < numWords; i++) {
  //   unsigned int word = *(unsigned int *)(memory + i * sizeof(unsigned int));
  //   unsigned char bitArray[WORD_SIZE];

  //   for (int j = 0; j < WORD_SIZE; j++) {
  //     bitArray[j] = (word >> j) & 1;
  //   }
  // }
  // Print byte in hex
  // for (long i = 0; i < fileSize; i++) {
  //    printf("%08x ", memory[i]);
  printf("%d\n", getSubWord(4,7, 185));

  free(memory);
  return 0;
}
//accesses subsection of instruction between indexes start to end, inclusively.
//PRE: end>=start
unsigned int getSubWord(int start, int end, unsigned int word){
  int width = end - start + 1;
  unsigned int subword = (word >> start) & ((int) (power(2,width) - 1));
  return subword;
}

// @param i - ensure it's between 0 and 31 inclusive.
unsigned char getBit(unsigned int word, int i) {
  return (word >> i) & 1;
}

// @param byte - pass in memory[i].
void printByte(unsigned char byte) {
  for (int j = 7; j >= 0; j--) {
    unsigned char mask = 1 << j;
    unsigned char bit = (byte & mask) ? 1 : 0;
    printf("%d", bit);
  }
}

void singleDataTransfer(unsigned int word){
  //size of the load
  unsigned char sf = getBit(word,30);
  if (sf==1){
    //access 64bit target register
  }else{
    //access 32bit target register
  }
  // l = 1 if load operation else store.
  char l = getBit(word,22);

  
  char u = getBit(word,24);
  
  //Unsigned Immediate Offset
  if (u==1){
    unsignedImmediateOffset(word,l);
  }

}

void unsignedImmediateOffset(unsigned int word,int l){
    unsigned char rt_index= word & 15;

}
void preIndexed(unsigned int word,int l){

}
void postIndexed(unsigned int word, int l){

}
void registerOffset(unsigned int word,int l){

}
void loadLiteral(unsigned int word,int l){

}



int power( int base, int exponent) {
    if (exponent == 0) {
        return 1;
    } else if (exponent % 2 == 0) {
        int temp = power(base, exponent / 2);
        return temp * temp;
    } else {
        return base * power(base, exponent - 1);
    }
}