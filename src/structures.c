//
// Created by iu22 on 13/06/23.
//
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "structures.h"
#define MAX_SYMBOLS 100


symbolEntry createEntry(char *label, uint32_t memoryAddress) {
  symbolEntry newEntry = malloc(sizeof(struct symbolEntry));
  if (newEntry == NULL) {
    printf("Malloc failed when allocating newEntry");
    return NULL;
  }

  strcpy(newEntry->label, label);
  newEntry->memoryAddress = memoryAddress;

  return newEntry;
}

symbolTable createSymTable(int max_size, int count, symbolEntry *table) {
  symbolTable newTable = malloc(sizeof(struct symbolTable));
  if (newTable == NULL) {
    printf("Malloc failed when allocating newTable");
    return NULL;
  }

  newTable->max_size = max_size;
  newTable->count = count;
  for (int i = 0; i < count; i++) {
    newTable->table[i] = table[i];
  }

  return newTable;
}


uint32_t getAddress(symbolTable symtable, char *label) {
  for (int i = 0; i < (symtable->count); i++) {
    if (strcmp(symtable->table[i]->label, label) == 0) {
      return symtable->table[i]->memoryAddress;
    }
  }
  printf("Label not found!");
  return -1;
}


Node createNode(uint32_t memoryAddress, const char* type, int num, const char** args) {
  Node newNode = malloc(sizeof(struct Node));
  if (newNode == NULL) {
    printf("Malloc failed when allocating newNode");
    return NULL;
  }

  newNode->memoryAddress = memoryAddress;

  // Allocate memory for the type string and copy the content
  newNode->type = malloc(strlen(type) + 1);
  if (newNode->type == NULL) {
    printf("Malloc failed when allocating for type");
    free(newNode);
    return NULL;
  }
  strcpy(newNode->type, type);

  // Allocate memory for the args array
  newNode->args = malloc(sizeof(char *) * num);
  if (newNode->args == NULL) {
    printf("Malloc failed when allocating args list");
    free(newNode->type);
    free(newNode);
    return NULL;
  }

  // Allocate memory for each args element and copy string
  for (int i = 0; i < num; i++) {
    newNode->args[i] = malloc(strlen(args[i]) + 1);

    if (newNode->args[i] == NULL) {
      printf("Malloc failed when allocating arg[%d]", i);
      // To free all the args before
      for (int j = 0; j < i; j++) {
        free(newNode->args[j]);
      }
      free(newNode->args);
      free(newNode->type);
      free(newNode);
      return NULL;
    }

    strcpy(newNode->args[i], args[i]);
  }




  newNode->num = num;
  newNode->next = NULL;

  return newNode;
}


Node addNodeToNode(Node currNode, Node addNode, List list) {
  currNode->next = addNode;
  list->count ++;
  return currNode;
}

Node createAlisCopyNode(Node node, int n, char *arg) {
  //Creating the array with extra element at index n
  char *arrayOfArgs[node->num + 1];
  int j = 0;
  for (int i = 0; i < node->num + 1; i++) {
    if (i == n) {
      arrayOfArgs[i] = malloc(sizeof(strlen(arg) + 1));
      strcpy(arrayOfArgs[i], arg);
    }
    arrayOfArgs[i] = malloc(sizeof(strlen(node->arg[j]) + 1));
    strcpy(arrayOfArgs[i], node->arg[j]);
    j++;
  }
  
  //Creating copyNode with new string inserted
  Node copyNode = createNode(node->memoryAddress,
  node->type,node->num + 1, arrayOfArgs);
  copyNode->next = node->next;

  
  return copyNode;
}


void printNode(Node node) {
  char builder[100];
  strcpy(builder, node->type);
  for (int i = 0; i < node->num; i++) {
    strcat(builder, (node->args)[i]);
  }
  printf("%s\n", builder);
}

/*
 * Creates a new linked list with the specified start and end nodes.

 * Parameters:
   - startNode: A pointer to the Node structure representing the first node in the list.
   - endNode: A pointer to the Node structure representing the last node in the list.
   - count: The number of nodes in the list.

 * Returns:
   - A pointer to the newly created List structure representing the linked list, or NULL if memory allocation fails.

 * Preconditions:
   - startNode and endNode must be valid pointers to Node structures.
   - count must be a positive value.
*/
List createList(Node startNode, Node endNode, int count) {
  List list = malloc(sizeof(struct List));
  if (list == NULL) {
    printf("Malloc failed when allocating list");
    return NULL;
  }

  list->first = startNode;
  list->last = endNode;
  list->count = count;

  return list;
}

// Similar to createList 
List createListWithStart(Node startNode) {
  return createList(startNode, NULL, 0);
}

List createListWithBoth(Node startNode, Node endNode) {
  return createList(startNode, endNode, -1);
}


// Free Functions Below


void freeSymbolEntry(symbolEntry symEntry) {
  free(symEntry);
}

/*
 * Frees the memory allocated for a symbol table and its entries.
 * Parameters:
   - symTable: A pointer to the symbolTable structure representing the symbol Table to be freed.
 * Preconditions:
   - The list must be a valid pointer to a non-empty symbol Table structure.
*/
void freeSymbolTable(symbolTable symTable) {
  for (int i = 0; i < symTable->count; i++) { // Free each of the symbolEntries
    freeSymbolEntry(symTable->table[i]);
  }
  free(symTable);
}


/*
 * Frees the memory allocated for a linked list and its nodes.
 * Parameters:
   - list: A pointer to the List structure representing the linked list to be freed.
 * Preconditions:
   - The list must be a valid pointer to a non-empty List structure.
*/
void freeList(List list) {
  assert (list->count != 0);
  Node node = list->first;
  Node nextNode = list->first;
  for (int i = 0; i < (list->count); i++) {
    nextNode = node->next;
    freeNode(node);
  }
  free(list);
}


/*
 * Frees the memory allocated for a single node of the linked list.
 * Parameters:
   - node: A pointer to the Node structure representing the node to be freed.
 * Preconditions:
   - The node must be a valid pointer to a Node structure.
*/
void freeNode(Node node) {
  free(node->memoryAddress);
  free(node->type);
  free(node->next);
  // Free each of the strings stored in args
  for (int i = 0; i < node->num; i++){
    free((node->args)[i]);
  }
  free(node->num);
}






