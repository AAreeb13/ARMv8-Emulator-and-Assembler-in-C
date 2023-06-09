// Contains functions for Load and Store instructions
#ifndef STOREANDLOAD_H
#define STOREANDLOAD_H


#include <stdint.h>

extern uint8_t singleDataTransfer(uint32_t);

extern uint64_t unsignedImmediateOffset(uint32_t, uint8_t);
extern uint64_t preIndexed(uint32_t);
extern uint64_t postIndexed(uint32_t);
extern uint64_t registerOffset(uint32_t);
extern int64_t signExtendTo64(int64_t value, uint8_t length);
extern uint64_t loadLiteral(uint32_t);
extern void load(uint64_t, uint8_t, uint8_t);
extern void store(uint64_t, uint8_t, uint8_t);

#endif