#include <stdio.h>
#include <stdlib.h>
#define WORD_SIZE 32

int main(int argc, char **argv) {
    FILE *file;
    unsigned char *buffer;

    if (argc != 2) {
        return EXIT_FAILURE;
    }
    
    // Open file
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Opening file failed.\n");
        return EXIT_FAILURE;
    }
    
    // File size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    
    // Allocate memory for buffer
    buffer = (unsigned char*)malloc(fileSize);
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Read file into buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        printf("Failed to read file.\n");
        free(buffer);
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Close file
    fclose(file);

    // no. words
    int numWords = fileSize / sizeof(unsigned int);
    
    // process each word
    for (int i = 0; i < numWords; i++) {
      unsigned int word = *(unsigned int *)(buffer + i * sizeof(unsigned int));
      unsigned char bitArray[WORD_SIZE];
      
      for (int j = 0; j < WORD_SIZE; j++) {
        bitArray[j] = (word >> j) & 1;
      }
    }
    // Print byte in hex
    //for (long i = 0; i < fileSize; i++) {
    //    printf("%08x ", buffer[i]); 
    //}

    // Print in binary
    // for (long i = 0; i < fileSize; i++) {
        
    //     for (int j = 7; j >= 0; j--) {
    //         unsigned char mask = 1 << j;
    //         unsigned char bit = (buffer[i] & mask) ? 1 : 0;
    //         printf("%d", bit);
    //     }
    //     printf(" ");
    // }
    // printf("\n");
    // printf("%ld\n", fileSize / 4);
    // printf("%ld\n", fileSize % 4);
    
    
    free(buffer);
    
    return 0;
}

