#include "aliases.h"

// inserts @param arg into @param index in @param node->args
static void insertArg(Node node, char *arg, int index) {
  node -> num++;
  node -> args = realloc(node -> args, sizeof(char *) * node -> num);
  for (int i = (node -> num) - 1; i > index; i--) {
    int len = strlen(node -> args[i - 1]) + 1;
    node -> args[i] = realloc(node -> args[i], sizeof(char) * len);
    strcpy(node -> args[i], node -> args[i - 1]);
  }
  node -> args[index] = realloc(node -> args[index], sizeof(char) * (strlen(arg) + 1));
  strcpy(node -> args[index], arg);
}

// changes the type of @param node->type to @param type
static void changeType(Node node, char *type) {
  int len = strlen(type) + 1;
  node -> type = realloc(node -> type, len);
  strcpy(node -> type, type);
}

uint32_t cmp(Node node) {
  changeType(node, "subs");
  insertArg(node, "rzr", 0);
  return arithmetic(node);
}

uint32_t cmn(Node node) {
  changeType(node, "adds");
  insertArg(node, "rzr", 0);
  return arithmetic(node);
}

uint32_t neg(Node node) {
  changeType(node, "sub");
  insertArg(node, "rzr", 1);
  return arithmeticImm(node);
}

uint32_t negs(Node node) {
  changeType(node, "subs");
  insertArg(node, "rzr", 1);
  return arithmeticImm(node);
}

uint32_t tst(Node node) {
  changeType(node, strdup("ands"));
  insertArg(node, strdup("rzr"), 0);
  return arith_or_logic(node);
}

uint32_t mvn(Node node) {
  changeType(node, strdup("orn"));
  insertArg(node, strdup("rzr"), 1);
  return arith_or_logic(node);
}

uint32_t mov(Node node) {
  changeType(node, strdup("orr"));
  insertArg(node, strdup("rzr"), 1);
  return arith_or_logic(node);
}

uint32_t mul(Node node) {
//  printNode(node);
  changeType(node, strdup("madd"));
//  printNode(node);
  insertArg(node, strdup("rzr"), 3);
//  printNode(node);
  return multiply(node);
}

uint32_t mneg(Node node) {
  changeType(node, strdup("msub"));
  insertArg(node, strdup("rzr"), 3);
  return multiply(node);
}

uint32_t nop(Node node) {
  return 0b11010101000000110010000000011111;
}