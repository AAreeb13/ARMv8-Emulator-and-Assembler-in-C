#include <stdlib.h>

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}


#include <stdio.h>
#include <stdint.h>

#define WORD_SIZE 4 // 32 bits (4 bytes)

int main() {
    FILE *file = fopen("binary_file.bin", "rb"); // Open binary file for reading

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    fseek(file, 0, SEEK_SET); // Seek to the beginning of the file

    uint8_t buffer[WORD_SIZE]; // Buffer to hold a word (32-bit instruction)

    while (fread(buffer, WORD_SIZE, 1, file) == 1) {
        // Process each word in the buffer
        // You can access the individual bytes of the word using buffer[0], buffer[1], buffer[2], buffer[3]
        // Do whatever you need to do with the word (e.g., print it, analyze it, etc.)
    }

    fclose(file); // Close the file

    return 0;
}

