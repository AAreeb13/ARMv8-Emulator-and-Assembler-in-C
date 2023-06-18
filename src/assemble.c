#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "structures.h"
#include "dataProcessImmediate.h"
#include "dataProcessRegister.h"
//#include "dataProcess.h"


#define SIZE_OF_BUFFER 100
#define DELIMITERS " ,\n"
#define STRINGS_COUNT 10
#define STRINGS_SIZE 20

// typeArray[num of elems][length] - Contains all the types
// Needed for parsing and determining if a word is a label or not
char typeArray[33][5] = {"add", "adds", "sub", "subs", "cmp", "cmn",
                         "neg", "negs", "and", "ands", "bic", "bics",
                         "eor", "orr", "eon", "orn", "tst", "movk",
                         "movn", "movz", "mov", "mvn", "madd", "msub",
                         "mul", "mneg", "b", "b.", "br", "str",
                         "ldr", "nop", ".int"};

//nodeFunc funcArray[] = {arithmetic, arithmetic, arithmetic, arithmetic, cmp, cmn, neg,
//                    negs, arith_or_logic, arith_or_logic, arith_or_logic, arith_or_logic,
//                    arith_or_logic, arith_or_logic, arith_or_logic, arith_or_logic, tst,
//                    wideMove, wideMove, wideMove, mov, mvn, multiply, multiply, mul, mneg,
//                    unconditionalOffsetA, conditionalBranchesA, unconditionalRegisterA,
//                    singleDataTransfer, singleDataTransfer, nop, literal};




int main(int argc, char **argv) {
  assert(argc == 3);

  // Initialising the funcPtrTale, symbolTable and memory address count
  //funcPtrEntry funPtrTable = createMainFuncEntry();
  int symMaxSize = INITIAL_SYMBOLTABLE_SIZE;
  symbolEntry symTable[INITIAL_SYMBOLTABLE_SIZE];
  int symCount = 0;
  int memoryAddress = 0;
  char *token;   // Hold token from tokenizer
  char **argsCopy = createMallocedStrings(STRINGS_COUNT, STRINGS_SIZE); // Extract and store type and arguments from each line

  int argsCount = 0; // Keep count of arguments extracted from line


  // Open the source code for reading
  FILE *readFile = fopen(argv[1], "r");
  if (readFile == NULL) {
    printf("Failed to open the readFile.\n");
    return 1;
  }


  char buffer[SIZE_OF_BUFFER];
  Node prevNode = NULL;
  Node currNode;
  List list;


  // One Pass - Creates all the nodes and the symbol Table containing the labels
  while(fgets(buffer, sizeof(buffer), readFile) != NULL) {
    token = strtok(buffer, DELIMITERS);
    if (token == NULL || token[0] == ' ' || token[0] == '\n') { // Checks if empty line
      continue;
    } else if (labelCheck(token)) { // Checks if label
      symTable[symCount] = createSymEntry(token, memoryAddress);
      symCount++;
    } else {                             // Must be a instruction
      while (token != NULL) {
        strcpy(argsCopy[argsCount], token);
        argsCount++;
        token = strtok(NULL, DELIMITERS);
      }

      currNode = createNode(memoryAddress, argsCopy[0], argsCount -1, argsCopy + 1);
      if (prevNode == NULL) {
        prevNode = currNode;
        list = createListWithStart(currNode);
      } else {
        addNextNode(prevNode, currNode, list);
        prevNode = currNode;
      }
      memoryAddress = memoryAddress + 4;
      argsCount = 0;
    }
  }

  symbolTable mainSymTable = createSymTable(symCount, symTable);
  fclose(readFile);
  // By now our list contains all the nodes, mainSymTable contains all the labels and their memory address


  printSymTable(mainSymTable);
  printList(list);


  // Open the binary file for writing
  FILE *writeFile = fopen("file.bin", "wb");
  if (writeFile == NULL) {
    printf("Failed to open the writeFile.\n");
    return 1;
  }
  fclose(writeFile);
  return EXIT_SUCCESS;
}
