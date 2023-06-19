
#ifndef SRC_DATATRANSFER_H
#define SRC_DATATRANSFER_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "structures.h"


extern uint32_t singleDataTransfer(Node);

extern uint32_t unsignedImmediateOffset(char **);
extern uint32_t zeroUnsignedOffset();
extern uint32_t postIndexed(char **);
extern uint32_t preIndexed(char **);
extern uint32_t registerOffset(char **);
extern void loadLiteral(uint32_t , char **);

extern uint32_t literal(Node);


#endif //SRC_DATATRANSFER_H

