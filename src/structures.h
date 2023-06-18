//
// Created by iu22 on 13/06/23.
//

#ifndef ARM_PROJECT_STRUCTURES_H
#define ARM_PROJECT_STRUCTURES_H
#include <stdint.h>

#define MAX_LABEL_SIZE 50
#define INITIAL_SYMBOLTABLE_SIZE 100
#define FUNC_TABLE_SIZE 50

// Node and List
typedef struct Node *Node;
struct Node{
    uint32_t memoryAddress;
    char *type;
    char **args;
    int num;
    Node next;
};


struct List{
    int count;
    Node first;
    Node last;
};
typedef struct List *List;


// Symbol Entry and Table
struct symbolEntry {
    char label[MAX_LABEL_SIZE];
    uint32_t memoryAddress;
};
typedef struct symbolEntry *symbolEntry;

struct symbolTable {
    int max_size;
    int count;
    symbolEntry table[INITIAL_SYMBOLTABLE_SIZE];
};
typedef struct symbolTable *symbolTable;


// Function pointers and table

typedef uint32_t (*nodeFunc)(Node);

struct funcPtrEntry {
    char *name;
    nodeFunc func;
};
typedef struct funcPtrEntry *funcPtrEntry;

struct funcPtrTable {
    int max_size;
    int count;
    funcPtrEntry table[FUNC_TABLE_SIZE]
};
typedef struct funcPtrTable *funcPtrTable;

// Symbol Entries and Table creation and functions
extern symbolEntry createSymEntry(char *label, uint32_t memoryAddress);
extern symbolTable createSymTable(int max_size, int count, symbolEntry *table);
extern uint32_t getAddress(symbolTable symtable, char *label);

// Function Pointer Entries and Table creation and functions
extern funcPtrEntry createFuncEntry(char *name, nodeFunc func);
extern funcPtrTable createFuncTable(int max_size, int count, funcPtrEntry table[]);


// Node creation, addition and printing
extern Node createNode(uint32_t memoryAddress, const char* type, int num, const char** args);
extern Node addNodeToNode(Node currNode, Node addNode, List list);
extern void printNode(Node node);


// List creation
extern List createList(Node startNode, Node endNode, int count);
extern List createListWithStart(Node startNode);
extern List createListWithBoth(Node startNode, Node endNode);

// Freeing functions
extern void freeSymbolEntry(symbolEntry symEntry); //
extern void freeSymbolTable(symbolTable symTable);  //
extern void freeFuncPtrEntry(funcPtrEntry entry); //
extern void freeFuncPtrTable(funcPtrTable funTable); //
extern void freeNode(Node node); //
extern void freeList(List list); //



#endif //ARM_PROJECT_STRUCTURES_H
