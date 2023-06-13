//
// Created by iu22 on 13/06/23.
//

#include "structures.h"

struct Node{
    uint8 memoryAddress;
    char *type;
    char *args[];
    uint8 num;
    Node next;
};

typedef Node *Node;

