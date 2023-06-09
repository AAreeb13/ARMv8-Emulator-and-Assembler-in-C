#ifndef DATAPROCESSIMMEDIATE_H 
#define DATAPROCESSIMMEDIATE_H

#include <stdint.h>
#include <stdbool.h>

extern void dataProcessImmediate(uint32_t);

extern void arithmetic(uint32_t, bool);
extern void add(uint8_t, uint32_t, uint8_t, bool);
extern void adds(uint8_t, uint32_t, uint8_t, bool);
extern void sub(uint8_t, uint32_t, uint8_t, bool);
extern void subs(uint8_t, uint32_t, uint8_t, bool);

extern void wideMove(uint32_t, bool);
extern void movn(uint64_t, uint8_t, bool);
extern void movz(uint64_t, uint8_t);
extern void movk(uint64_t, uint8_t, uint8_t, bool);

#endif