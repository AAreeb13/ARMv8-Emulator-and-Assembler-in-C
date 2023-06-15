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
    uint8_t memoryAddress;
    char *type;
    char **args;
    uint8_t num;
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
    uint8_t memoryAddress;
};

typedef struct symbolEntry *symbolEntry;

struct symbolTable {
    int size;
    int count;
    symbolEntry table[INITIAL_SYMBOL_SIZE];
};

typedef struct symbolTable *symbolTable;



// Node creation, addition and printing
extern Node createNode(uint8_t memoryAddress, const char* type, const char** args, uint8_t num);
extern Node addNode(Node currNode, Node addNode, List list);
extern char *printNode(Node node, char *representation);


// List creation
extern List createList(Node startNode, Node endNode, uint8_t count);
extern List createListWithStart(Node startNode);
extern List createListWithBoth(Node startNode, Node endNode);

// Freeing functions
extern void freeList(List list);
extern void freeNode(Node node);




#endif //ARM_PROJECT_STRUCTURES_H
