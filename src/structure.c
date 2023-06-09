#include "structure.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binaryFileWriter(uint8_t *memory, char *arg1) {
    FILE *file;
    file = fopen(arg1, "rb");
    if (file == NULL) {
        printf("Opening file failed.\n");
        return;
    }
    // File size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Read file into memory
    size_t bytesRead = fread(memory, 1, fileSize, file);
    if (bytesRead != fileSize) {
        printf("Failed to read file.\n");
        free(memory);
        fclose(file);
        return;
    }
}

void initialiseGeneralReg() {
    // array of general registers + zero register
    struct Reg genRegisters[NUM_GENREGS + 1];
    for (int i = 0; i < NUM_GENREGS; i++) {
        sprintf(genRegisters[i].name, "X%02d", i);
        genRegisters[i].value = 0;
    }
    genRegisters[NUM_GENREGS] = ZR;
}

uint8_t *allocateMemory() {
    uint8_t *memory = (uint8_t *)calloc(MEMORY_SIZE, sizeof(uint8_t));
    if (memory == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    return memory;
}

// outputs state of CPU into given output file
int cpuState(char *output_file) {

    FILE *out = fopen(output_file, "w");
    if (out == NULL) {
        fprintf(stderr, "Can’t create output.out\n");
        exit(1);
    }

    // REGISTERS
    fputs("Registers:", out);
    putc('\n', out);

    // general
    for (int i = 0; i < NUM_GENREGS; i++) {
        char temp[27];
        sprintf(temp, "%.3s    = %016lx\n", genRegisters[i].name,
                genRegisters[i].value);
        fputs(temp, out);
    }

    // special registers - PC & PSTATE
    char temp[27];
    sprintf(temp, "%.2s     = %016lx\n", PC.name, PC.value);
    fputs(temp, out);

    sprintf(temp, "PSTATE : %.1s%.1s%.1s%.1s\n", pState.N ? "N" : "-",
            pState.Z ? "Z" : "-", pState.C ? "C" : "-", pState.V ? "V" : "-");
    fputs(temp, out);

    // Non-zero memory
    fputs("Non-zero memory:\n", out);
    for (int i = 0; i < MEMORY_SIZE; i += 4) {
        uint32_t word = (memory[i + 3] << 24) + (memory[i + 2] << 16) +
                        (memory[i + 1] << 8) + memory[i];
        char temp[23];
        sprintf(temp, "0x%08X : %08x\n", i, word);
        if (word != 0) { // if non zero
            fputs(temp, out);
        }
        if (word == 0x8a000000) { // reached the end
            break;
        }
    }

    // error checking
    if (ferror(out)) {
        fprintf(stderr, "Error occurred writing to output.txt\n");
        exit(1);
    }
    fclose(out);

    return 0;
}