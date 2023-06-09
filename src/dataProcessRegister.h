#ifndef DATAPROCESSREGISTER_H
#define DATAPROCESSREGISTER_H

#include <stdint.h>


extern void dataProcessRegister(uint32_t);

extern void arithmeticReg(uint32_t, bool);

extern void logical(uint32_t, bool);
extern int64_t and(int64_t, uint64_t, bool);
extern int64_t bic(int64_t, uint64_t, bool);
extern int64_t orr(int64_t, uint64_t, bool);
extern int64_t orn(int64_t, uint64_t, bool);
extern int64_t eor(int64_t, uint64_t, bool);
extern int64_t eon(int64_t, uint64_t, bool);
extern int64_t ands(int64_t, uint64_t, bool);
extern int64_t bics(int64_t, uint64_t, bool);

extern void multiply(uint32_t, bool);

#endif