//
// Created by aaa1322 on 14/06/23.
//

#ifndef SRC_DATATRANSFER_H
#define SRC_DATATRANSFER_H

#include <stdlib.h>
#include "structure.h"
typedef struct Node *Node;

struct Node{
    uint8_t memoryAddress;
    char *type;
    uint8_t num;
    char **args;
    Node next;
};
uint32_t singleDataTransfer(Node);
uint32_t zeroUnsignedOffset();



#endif //SRC_DATATRANSFER_H
