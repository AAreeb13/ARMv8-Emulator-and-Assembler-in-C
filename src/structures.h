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





#endif //ARM_PROJECT_STRUCTURES_H
