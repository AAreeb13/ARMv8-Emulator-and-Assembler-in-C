//Contains helper and shift functions
#ifndef MISC_H
#define MISC_H
#include <stdint.h>
#include <stdbool.h>


extern uint64_t getSubWord64(uint8_t, uint8_t, uint64_t);
extern int64_t signExtend(int32_t, int);
extern uint8_t getBit(uint32_t, uint8_t);
extern uint32_t getSubWord(uint8_t, uint8_t, uint32_t);
extern int64_t ror(int64_t, uint8_t, bool);
extern int64_t lsr(int64_t, uint8_t, bool);
extern int64_t lsl(int64_t, uint8_t, bool);
extern int64_t asr(int64_t, uint8_t, bool);

#endif