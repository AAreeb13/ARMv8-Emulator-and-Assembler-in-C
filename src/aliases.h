#ifndef SRC_ALIASES_H
#define SRC_ALIASES_H
#include <stdint.h>
#include "structures.h"
#include "utils.h"
#include "dataProcessRegister.h"
#include "dataProcessImmediate.h"

extern uint32_t cmp(Node node);
extern uint32_t cmn(Node node);
extern uint32_t neg(Node node);
extern uint32_t negs(Node node);
extern uint32_t tst(Node node);
extern uint32_t mvn(Node node);
extern uint32_t mov(Node node);
extern uint32_t mul(Node node);
extern uint32_t mneg(Node node);

#endif //SRC_ALIASES_H
