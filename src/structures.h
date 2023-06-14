//
// Created by iu22 on 13/06/23.
//

#ifndef ARM_PROJECT_STRUCTURES_H
#define ARM_PROJECT_STRUCTURES_H

struct Node{
    uint8 memoryAddress;
    char *type;
    char *args[];
    uint8 num;
    Node next;
};

typedef Node *Node;





#endif //ARM_PROJECT_STRUCTURES_H
