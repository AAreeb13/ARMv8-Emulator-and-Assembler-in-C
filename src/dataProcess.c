#include "dataProcess.h"

// determines whether @param str is a register
static bool isReg(char *str) {
  return str[0] == 'x' || str[0] == 'w';
}

// calls the register or immediate version of arithmetic instructions
uint32_t arithmetic(Node node) {
  return isReg(node -> args[2]) ? arith_or_logic(node) : arithmeticImm(node);
}