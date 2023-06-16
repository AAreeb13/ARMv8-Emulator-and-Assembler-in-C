#include "aliases.h"

uint32_t cmp(Node node) {
  changeType(node, "subs");
  insertArg(node, "rzr", 0);
  return arithmeticImm(node);
}

uint32_t cmn(Node node) {
  changeType(node, "adds");
  insertArg(node, "rzr", 0);
  return arithmeticImm(node);
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
  changeType(node, strdup("madd"));
  insertArg(node, strdup("rzr"), 3);
  return multiply(node);
}

uint32_t mneg(Node node) {
  changeType(node, strdup("msub"));
  insertArg(node, strdup("rzr"), 3);
  return multiply(node);
}