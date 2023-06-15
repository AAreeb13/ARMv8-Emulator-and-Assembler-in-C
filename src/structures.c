#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "structures.h"


void freeList(List list) {
    assert (list->count != 0);
    Node node = list->first;
    Node nextNode = list->first;
    for (int i = 0; i < (list->count); i++) {
        nextNode = node->next;
        free(node);
    }
    free(list);

}

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

List createList(Node startNode, Node endNode, uint8_t count) {
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


List createListWithStart(Node startNode) {
    return createList(startNode, NULL, 0);
}

List createListWithBoth(Node startNode, Node endNode) {
    return createList(startNode, endNode, NULL);
}


Node createNode(uint8_t memoryAddress, const char* type, const char** args, uint8_t num) {
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


Node addNode(Node currNode, Node addNode, List list) {
    currNode->next = addNode;
    list->count ++;
    return currNode;
}

char *printNode(Node node, char *representation) {
    char builder[100];
    strcpy(builder, node->type);
    for (int i = 0; i < node->count; i++) {
        strcat(builder, (node->args)[i]);
    }
    strcpy(representation, builder);
    return representation;
}
