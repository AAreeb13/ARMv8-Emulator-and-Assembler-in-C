#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MEMORY_SIZE (1 << 21) // 2MB

uint8_t *memory;

uint8_t getBit(uint32_t word, uint8_t i) { return (word >> i) & 1; }

void printWord(uint32_t word) {
    printf(" ");
    for (int i = 31; i > -1; i--) {
        printf("%d  ", getBit(word, i));
    }
    printf("\n");
}

int main(int argc, char **argv) {
    FILE *file;

    if (argc != 2) {
        return EXIT_FAILURE;
    }

    memory = (uint8_t *)calloc(MEMORY_SIZE, 1);
    if (memory == NULL) {
        printf("Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Opening file failed.\n");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    size_t bytesRead = fread(memory, 1, fileSize, file);
    if (bytesRead != fileSize) {
        printf("Failed to read file.\n");
        free(memory);
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);

    int numWords = fileSize / sizeof(uint32_t);
    for (int i = 31; i >= 11; i--) {
        printf("%d ", i);
    }
    for (int i = 10; i >= 0; i--) {
        printf("%d  ", i);
    }
    printf("\n\n");
    for (int i = 0; i < numWords; i++) {
        uint32_t word = *(uint32_t *)(memory + i * sizeof(uint32_t));
        printWord(word);
    }

    free(memory);
    return 0;
}
