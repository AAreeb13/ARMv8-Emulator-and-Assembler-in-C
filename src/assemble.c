#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "structures.h"
#include "dataProcessImmediate.h"
#include "dataProcessRegister.h"
#include "dataProcess.h"
#include "aliases.h"
#include "branching.h"
#include "dataTransfer.h"

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

nodeFunc funcArray[] = {arithmetic, arithmetic, arithmetic, arithmetic, cmp, cmn, neg,
                    negs, arith_or_logic, arith_or_logic, arith_or_logic, arith_or_logic,
                    arith_or_logic, arith_or_logic, arith_or_logic, arith_or_logic, tst,
                    wideMove, wideMove, wideMove, mov, mvn, multiply, multiply, mul, mneg,
                    unconditionalOffsetA, conditionalBranchesA, unconditionalRegisterA,
                    singleDataTransfer, singleDataTransfer, nop, literal};



int main(int argc, char **argv) {
  assert(argc == 3);

  // Phase One: Initialising the funcPtrTale, symbolTable and memory address count
  funcPtrTable mainFunTable = createMainFuncTable();
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

  // Initialising the list and nodes
  Node prevNode = NULL;
  Node currNode;
  List list;


  // Phase Two: One Pass - Creates all the nodes and the symbol Table containing the labels
  while(fgets(buffer, sizeof(buffer), readFile) != NULL) {
    token = strtok(buffer, DELIMITERS);
    if (token == NULL || token[0] == ' ' || token[0] == '\n') { // Checks if empty line
      continue;
    } else if (labelCheck(token)) { // Checks if label, then adds to symTable
      symTable[symCount] = createSymEntry(token, memoryAddress);
      symCount++;
    } else {                             // Must be an instruction
      while (token != NULL) {
        strcpy(argsCopy[argsCount], token);
        argsCount++;
        token = strtok(NULL, DELIMITERS);
      }
      // argsCopy[0] stores the type, so count is 1 less
      currNode = createNode(memoryAddress, argsCopy[0], argsCount -1, argsCopy + 1);
      if (prevNode == NULL) { // We are processing our first Node, so no previous node
        prevNode = currNode;
        list = createListWithStart(currNode);
      } else { // Processing node after the first node, so connect previous node to this one
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



  // Phase Three : Process each node and write to file
  // Open the binary file for writing
  FILE *writeFile = fopen(argv[2], "wb");
  if (writeFile == NULL) {
    printf("Failed to open the writeFile.\n");
    return 1;
  }

  currNode = list->first;
  nodeFunc func;
  uint32_t word;
  for (int i = 0; i < list->count; i++) {
    assert (currNode != NULL);
    func = getFuncPtr(currNode->type, mainFunTable);
    word = func(currNode);
    size_t numBytesWritten = fwrite(&word, sizeof(uint32_t), 1, writeFile);
    if (numBytesWritten != 1) {
      printf("Error occurred while writing to the file.\n");
      fclose(writeFile);
      return 1;
    }
    currNode = currNode->next;
  }
  fclose(writeFile);

  //Phase Four: Free all structures and mallocs
  freeList(list);
  freeFuncPtrTable(mainFunTable);
  freeSymbolTable(mainSymTable);
  freeMallocedStrings(argsCopy, STRINGS_COUNT);

  return EXIT_SUCCESS;
}


