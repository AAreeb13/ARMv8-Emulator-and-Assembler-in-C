
#ifndef SRC_DATATRANSFER_H
#define SRC_DATATRANSFER_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "structures.h"
#include "assemble.h"


extern void giveSymTableTransfer(symbolTable table);
extern uint32_t singleDataTransfer(Node);
extern uint32_t unsignedImmediateOffset(Node);
extern uint32_t zeroUnsignedOffset();
extern uint32_t postIndexed(Node);
extern uint32_t preIndexed(Node);
extern uint32_t registerOffset(Node);
extern void loadLiteral(uint32_t *, Node);
extern uint32_t literal(Node);


#endif //SRC_DATATRANSFER_H

