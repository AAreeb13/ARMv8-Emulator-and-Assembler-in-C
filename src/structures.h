//
// Created by iu22 on 13/06/23.
//

#ifndef ARM_PROJECT_STRUCTURES_H
#define ARM_PROJECT_STRUCTURES_H
#include <stdint.h>

#define MAX_LABEL_SIZE 50
#define INITIAL_SYMBOL_SIZE 100


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

struct symbolEntry {
    char label[MAX_LABEL_SIZE];
    uint32_t memoryAddress;
};

typedef struct symbolEntry *symbolEntry;

struct symbolTable {
    int max_size;
    int count;
    symbolEntry table[INITIAL_SYMBOL_SIZE];
};

typedef struct symbolTable *symbolTable;



// Node creation, addition and printing
extern Node createNode(uint32_t memoryAddress, const char* type, const char** args, int num);
extern Node addNodeToNode(Node currNode, Node addNode, List list);
extern void printNode(Node node);


// List creation
extern List createList(Node startNode, Node endNode, int count);
extern List createListWithStart(Node startNode);
extern List createListWithBoth(Node startNode, Node endNode);

// Freeing functions
extern void freeSymbolEntry(symbolEntry symEntry);
extern void freeSymbolTable(symbolTable symTable);
extern void freeList(List list);
extern void freeNode(Node node);




#endif //ARM_PROJECT_STRUCTURES_H
