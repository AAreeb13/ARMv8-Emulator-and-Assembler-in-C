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

#endif //ARM_PROJECT_STRUCTURES_H
