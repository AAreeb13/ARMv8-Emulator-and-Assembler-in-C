//Include standard files
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


//Include modules
#include "structure.h"
#include "misc.h"
#include "dataProcessImmediate.h"
#include "dataProcessRegister.h"
#include "storeandLoad.h"
#include "branch.h"


//Constants
#define MEMORY_SIZE (1 << 21) // 2MB
#define MASK_32 ((UINT32_C(1) << 31) - 1)
#define MASK_64 ((UINT64_C(1) << 63) - 1)
#define NUM_GENREGS 31
// registers: X00 -> X30



//Global Variables 
uint8_t *memory;
struct Reg PC = {"PC", 0};
const struct Reg ZR = {"ZR", 0};
struct PState pState = {false, true, false, false};
struct Reg genRegisters[NUM_GENREGS + 1];



int main(int argc, char **argv) {
    // Make sure there is right number of arguments
  if (argc > 3) {
      printf("Wrong num of arguments!");
    return EXIT_FAILURE;
  }

  
  uint8_t *memory = allocateMemory();
  
  initialiseGeneralReg();

  uint32_t word;

  
  while (true) {
      if (PC.value >= MEMORY_SIZE) { //Address out of bound
          break;
      }
      // Fetch
      word = *(uint32_t *)(memory + PC.value);
      if (word == 0x8a000000) { // Half Instruction
          break;
      } else if (getSubWord(26, 28, word) == 4) /*0b100x*/ {
          dataProcessImmediate(word);
      } else if (getSubWord(25, 27, word) == 5) /*0bx101*/ {
          dataProcessRegister(word);
      } else if ((getBit(word, 27) == 1) && (getBit(word, 25) == 0)) /*0bx1x0*/ {
          uint8_t isValidAddress = singleDataTransfer(word);
          if (isValidAddress == EXIT_FAILURE){break;}
      } else if (getSubWord(26, 28, word) == 5) /*0b101x*/ {
          branchInstructions(word);
          PC.value -= 4; //PC is incremented by +4
      }
      PC.value +=4;
  }
  
  if (argc == 3) {
    cpuState(argv[2]);
  } else {
    cpuState(argv[1]);
  }
  free(memory);
  return 0;
}