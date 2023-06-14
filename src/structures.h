//
// Created by iu22 on 13/06/23.
//

#ifndef ARM_PROJECT_STRUCTURES_H
#define ARM_PROJECT_STRUCTURES_H
#include <stdint.h>

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


// Freeing functions
extern void freeList(List list);
extern void freeNode(Node node);

// List creation
extern List createList(Node startNode, Node endNode, uint8_t count);
extern List createListWithStart(Node startNode);
extern List createListWithBoth(Node startNode, Node endNode);

// Node creation
extern Node createNode(uint8_t memoryAddress, const char* type, const char** args, uint8_t num);





#endif //ARM_PROJECT_STRUCTURES_H
