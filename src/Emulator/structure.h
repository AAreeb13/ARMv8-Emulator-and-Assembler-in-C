// Represents structure of Memory and Register
#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdint.h>
#include <stdbool.h>

#define WORD_SIZE 32
#define MEMORY_SIZE (1 << 21) // 2MB
#define NUM_GENREGS 31
// registers: X00 -> X30

//BinaryFile opener and writer
extern void binaryFileWriter(uint8_t *memory, char *arg1);

struct Reg {
    char name[4];
    int64_t value; // stands for Xn -> in binary
};

// create PState structure
struct PState {
    bool N; // last result (lr) was -ive
    bool Z; // lr was 0
    bool C; // lr caused a bit to be carried out
    bool V; // lr caused overflow
};

extern struct Reg PC;
extern const struct Reg ZR;
extern struct PState pState;

extern struct Reg genRegisters[NUM_GENREGS + 1];

extern void initialiseGeneralReg();

// main memory
extern uint8_t *memory;
extern uint8_t *allocateMemory();

//CPU state printer
extern int cpuState(char *output_file);

#endif

