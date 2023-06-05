#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 32
#define MEMORY_SIZE (1 << 21) // 2MB
// 2^31 - 1
#define MASK_32 2147483647
// 2^63 - 1
#define MASK_64 9223372036854775807

#define NUM_GENREGS 31
// registers: X00 -> X30

// misc
uint8_t getBit(uint32_t, uint8_t);
void printByte(unsigned char);
uint32_t getSubWord(uint8_t, uint8_t, uint32_t);
int power(int, int);
int64_t ror(int64_t, uint8_t, bool);
int64_t lsr(int64_t, uint8_t, bool);
int64_t lsl(int64_t, uint8_t, bool);
int64_t asr(int64_t, uint8_t, bool);

// Data Processing (Immediate) functions           NEED TO UPDATE FLAGS!!!
void dataProcessImmediate(uint32_t);

void arithmetic(uint32_t, bool);
void add(uint8_t, unsigned int, uint8_t, bool);
void addc(uint8_t, unsigned int, uint8_t, bool);
void sub(uint8_t, unsigned int, uint8_t, bool);
void subc(uint8_t, unsigned int, uint8_t, bool);

void wideMove(uint32_t, bool);
void movn(uint64_t, uint8_t, bool);
void movz(uint64_t, uint8_t);
void movk(uint64_t, uint8_t, uint8_t, bool);

// Data Processing (Register) functions            NEED TO TAKE INTO ACC ZR!!!
void dataProcessRegister(uint32_t);

void arithmeticReg(uint32_t, bool);

void logical(uint32_t, bool);
int64_t and (int64_t, uint64_t, bool);
int64_t bic(int64_t, uint64_t, bool);
int64_t orr(int64_t, uint64_t, bool);
int64_t orn(int64_t, uint64_t, bool);
int64_t eor(int64_t, uint64_t, bool);
int64_t eon(int64_t, uint64_t, bool);
int64_t ands(int64_t, uint64_t, bool);
int64_t bics(int64_t, uint64_t, bool);

void multiply(uint32_t, bool);

struct Reg {
  char name[4];
  int64_t value; // stands for Xn -> in binary
};

// array of general registers + zero register
struct Reg genRegisters[NUM_GENREGS + 1];

// zero register
const struct Reg ZR = {"ZR", 0};

void initialiseGeneralReg() {
  for (int i = 0; i < NUM_GENREGS; i++) {
    sprintf(genRegisters[i].name, "X%02d", i);
    genRegisters[i].value = 0;
  }
  genRegisters[NUM_GENREGS] = ZR;
}

// create PC structure
struct Reg PC = {"PC", 0};

// create PState structure
struct PState {
  bool N; // last result (lr) was -ive
  bool Z; // lr was 0
  bool C; // lr caused a bit to be carried out
  bool V; // lr caused overflow
};

struct PState pState = {false, true, false, false};

// main memory
uint8_t *memory;

int main(int argc, char **argv) {
  FILE *file;
  // Make sure that there is a second parameter(binary file)
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  // Memory is given 2MB
  memory = (uint8_t *)calloc(MEMORY_SIZE, 1);
  if (memory == NULL) {
    printf("Memory allocation failed.\n");
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

  // Read file into memory
  size_t bytesRead = fread(memory, 1, fileSize, file);
  if (bytesRead != fileSize) {
    printf("Failed to read file.\n");
    free(memory);
    fclose(file);
    return EXIT_FAILURE;
  }

  // Close file
  fclose(file);

  // no. words
  int numWords = fileSize / sizeof(uint32_t);

  // process each word

  for (int i = 0; i < numWords; i++) {
    uint32_t word = *(uint32_t *)(memory + i * sizeof(uint32_t));
    if (getSubWord(26, 28, word) == 4) /*0b100x*/ {
      dataProcessImmediate(word);
    } else if (getSubWord(25, 27, word) == 5) /*0bx101*/ {
      dataProcessRegister(word);
    } else if ((getBit(word, 27) == 1) && (getBit(word, 25) == 0)) /*0bx1x0*/ {
      // Loads and Stores
    } else if (getSubWord(26, 28, word) == 5) /*0b101x*/ {
      // Branches
    }
  }
  // // Print byte in hex
  // for (long i = 0; i < fileSize; i++) {
  //    printf("%08x ", memory[i]);
  // }
  free(memory);
  return 0;
}

// @param i - ensure it's between 0 and 31 inclusive.
uint8_t getBit(uint32_t word, uint8_t i) { return (word >> i) & 1; }

// @param byte - for passing in memory[i].
void printByte(unsigned char byte) {
  for (int j = 7; j >= 0; j--) {
    uint8_t mask = 1 << j;
    uint8_t bit = (byte & mask) ? 1 : 0;
    printf("%d", bit);
  }
}

// accesses subsection of instruction between indexes start to end, inclusively.
// PRE: end>=start
uint32_t getSubWord(uint8_t start, uint8_t end, uint32_t word) {
  int width = end - start + 1;
  unsigned int subword = (word >> start) & (power(2, width) - 1);
  return subword;
}

int power(int base, int exponent) {
  if (exponent == 0) {
    return 1;
  } else if (exponent % 2 == 0) {
    int temp = power(base, exponent / 2);
    return temp * temp;
  } else {
    return base * power(base, exponent - 1);
  }
}

int64_t ror(int64_t value, uint8_t shiftAmount, bool Xn) {
  if (!Xn) {
    shiftAmount %= 32;
    int32_t result = value;
    for (int i = 0; i < shiftAmount; i++) {
      int32_t firstBit = getBit(result, 0) ? 1 << 31 : 0;
      result = ((result >> 1) & MASK_32) + (firstBit);
    }
    return result;
  }
  shiftAmount %= 64;
  int64_t result = value;
  for (int i = 0; i < shiftAmount; i++) {
    int64_t firstBit = getBit(result, 0) ? (MASK_64 + 1) : 0;
    result = ((result >> 1) & MASK_64) + (firstBit);
  }
  return result;
}

int64_t lsr(int64_t value, uint8_t shiftAmount, bool Xn) {
  if (!Xn) {
    int32_t val = value;
    uint8_t bit = getBit(value, 31);
    val = val >> shiftAmount;
    if (bit) {
      val += bit << (32 - shiftAmount);
    }
    return val;
  } else {
    uint8_t bit = getBit(value, 63);
    value = value >> shiftAmount;
    if (bit) {
      value += bit << (64 - shiftAmount);
    }
    return value;
  }
}

int64_t lsl(int64_t value, uint8_t shiftAmount, bool Xn) {
  if (!Xn) {
    int32_t val = value;
    return val << shiftAmount;
  }
  return value << shiftAmount;
}

int64_t asr(int64_t value, uint8_t shiftAmount, bool Xn) {
  if (!Xn) {
    int32_t val = value;
    return val >> shiftAmount;
  }
  return value >> shiftAmount;
}

void dataProcessImmediate(uint32_t word) {
  uint8_t opi = getSubWord(23, 25, word);
  uint8_t sf = getBit(word, 31);
  if (opi == 2) /*0b010*/ {
    if (sf == 1) {
      arithmetic(word, 1);
    } else {
      arithmetic(word, 0);
    }
  } else /*opi = 0b101*/ {
    if (sf == 1) {
      wideMove(word, 1);
    } else {
      wideMove(word, 0);
    }
  }
}

void arithmetic(uint32_t word, bool Xn) {
  uint8_t sh = getBit(word, 22);
  uint32_t Op_2;

  if (sh == 0) {
    Op_2 = getSubWord(10, 21, word);
  } else {
    Op_2 = getSubWord(10, 21, word) << 12;
  }

  uint8_t opc = getSubWord(29, 30, word);
  uint8_t rd = getSubWord(0, 4, word);
  uint8_t rn = getSubWord(5, 9, word);

  if (opc == 0) /*0b00*/ {
    add(rn, Op_2, rd, Xn);
  } else if (opc == 1) /*0b01*/ {
    addc(rn, Op_2, rd, Xn);
  } else if (opc == 2) /*0b10*/ {
    sub(rn, Op_2, rd, Xn);
  } else /*0b11*/ {
    subc(rn, Op_2, rd, Xn);
  }
}

void add(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value + Op_2
      : ((int32_t) genRegisters[rn].value) + Op_2;
}
void addc(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value + Op_2
      : ((int32_t) genRegisters[rn].value) + Op_2;
  // UPDATE FLAGS!!!
}
void sub(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
      : ((int32_t) genRegisters[rn].value) - Op_2;
}

void subc(uint8_t rn, uint32_t Op_2, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
      : ((int32_t) genRegisters[rn].value) - Op_2;
  // UPDATE FLAGS!!!
}

void wideMove(uint32_t word, bool Xn) {
  uint8_t hw = getSubWord(21, 21, word);
  uint64_t Op = getSubWord(5, 20, word) << (hw * 16);
  uint8_t opc = getSubWord(29, 30, word);
  uint8_t rd = getSubWord(0, 4, word);
  switch (opc) {
    case 0: // 0b00
      movn(Op, rd, Xn);
      break;
    case 2: // 0b10
      movz(Op, rd);
      break;
    case 3: // 0b10
      movk(Op, rd, hw, Xn);
      break;
  }
}

void movn(uint64_t Op, uint8_t rd, bool Xn) {
  genRegisters[rd].value = Xn ? ~Op : (int32_t)(~Op);
}

void movz(uint64_t Op, uint8_t rd) { genRegisters[rd].value = Op; }

void movk(uint64_t Op, uint8_t rd, uint8_t hw, bool Xn) {
  int64_t val = genRegisters[rd].value;
  int64_t lower = getSubWord(0, (hw * 16) - 1, val);
  int64_t upper = getSubWord((hw + 1) * 16, 63, val) << ((hw + 1) * 16);
  int64_t result = lower + Op + upper;
  genRegisters[rd].value = Xn ? result : (int32_t)result;
}

void dataProcessRegister(uint32_t word) {
  uint8_t M = getBit(word, 28);
  uint8_t opr = getSubWord(21, 24, word);
  uint8_t sf = getBit(word, 31);
  if (M == 0 && (getBit(opr, 3) == 1)) {
    arithmeticReg(word, sf);
  } else if (M == 1) {
    multiply(word, sf);
  } else {
    logical(word, sf);
  }
}

void arithmeticReg(uint32_t word, bool Xn) {
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t rd = getSubWord(0, 4, word);
  uint8_t operand = getSubWord(10, 15, word);
  uint8_t shift = getSubWord(22, 23, word);
  uint8_t opc = getSubWord(29, 30, word);
  uint64_t Op_2;
  switch (shift) {
    case 0: /*0b00*/
      Op_2 = lsl(genRegisters[rm].value, operand, Xn);
      break;
    case 1: /*0b01*/
      Op_2 = lsr(genRegisters[rm].value, operand, Xn);
      break;
    default: /*0b10*/
      Op_2 = lsl(genRegisters[rm].value, operand, Xn);
      break;
  }

  switch (opc) {
    case 0: /*0b00*/
      add(rn, Op_2, rd, Xn);
      break;
    case 1: /*0b01*/
      addc(rn, Op_2, rd, Xn);
      break;
    case 2: /*0b10*/
      sub(rn, Op_2, rd, Xn);
      break;
    default:
      subc(rn, Op_2, rd, Xn); /*0b11*/
      break;
  }
}

void logical(uint32_t word, bool Xn) {
  uint8_t rd = getSubWord(0, 4, word);
  if (rd == 31) return;
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t operand = getSubWord(10, 15, word);
  uint8_t shift = getSubWord(22, 23, word);
  uint8_t opc = getSubWord(29, 30, word);
  uint8_t N = getBit(word, 21);
  uint64_t Op_2;
  switch (shift) {
    case 0: /*0b00*/
      Op_2 = lsl(genRegisters[rm].value, operand, Xn);
      break;
    case 1: /*0b01*/
      Op_2 = lsr(genRegisters[rm].value, operand, Xn);
      break;
    case 2: /*0b10*/
      Op_2 = asr(genRegisters[rm].value, operand, Xn);
      break;
    default: /*0b11*/
      Op_2 = ror(genRegisters[rm].value, operand, Xn);
      break;
  }

  switch (opc) {
    case 0: /*0b00*/
      genRegisters[rd].value = N ? bic(rn, Op_2, Xn) : and(rn, Op_2, Xn);
      break;
    case 1: /*0b01*/
      genRegisters[rd].value = N ? orn(rn, Op_2, Xn) : orr(rn, Op_2, Xn);
      break;
    case 2: /*0b10*/
      genRegisters[rd].value = N ? eon(rn, Op_2, Xn) : eor(rn, Op_2, Xn);
      break;
    default: /*0b11*/
      genRegisters[rd].value = N ? bics(rn, Op_2, Xn) : ands(rn, Op_2, Xn);
      break;
  }
}

int64_t and (int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 & Op_32;
  }
  return genRegisters[rn].value & Op_2;
}

int64_t bic(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 & (~Op_32);
  }
  return genRegisters[rn].value & (~Op_2);
}

int64_t orr(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 | Op_32;
  }
  return genRegisters[rn].value | Op_2;
}

int64_t orn(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 | (~Op_32);
  }
  return genRegisters[rn].value | (~Op_2);
}

int64_t eor(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 ^ Op_32;
  }
  return genRegisters[rn].value ^ Op_2;
}

int64_t eon(int64_t rn, uint64_t Op_2, bool Xn) {
  if (!Xn) {
    int32_t Op_32 = Op_2;
    int32_t rn_32 = genRegisters[rn].value;
    return rn_32 ^ (~Op_32);
  }
  return genRegisters[rn].value ^ (~Op_2);
}

int64_t ands(int64_t rn, uint64_t Op_2, bool Xn) {
  int64_t result = and(rn, Op_2, Xn);
  pState.N = Xn ? getBit(result, 63) : getBit(result, 31);
  pState.Z = result ? pState.Z : 1;
  pState.C = 0;
  pState.V = 0;
  return result;
}

int64_t bics(int64_t rn, uint64_t Op_2, bool Xn) {
  int64_t result = bic(rn, Op_2, Xn);
  pState.N = Xn ? getBit(result, 63) : getBit(result, 31);
  pState.Z = result ? pState.Z : 1;
  pState.C = 0;
  pState.V = 0;
  return result;
}

void multiply(uint32_t word, bool Xn) {
  assert(getSubWord(29, 30, word) == 0);
  uint8_t rd = getSubWord(0, 4, word);
  if (rd == 31) return;
  uint8_t rm = getSubWord(16, 20, word);
  uint8_t rn = getSubWord(5, 9, word);
  uint8_t ra = getSubWord(10, 14, word);
  uint8_t x = getBit(word, 15);
  if (!Xn) {
    uint32_t rm_32 = genRegisters[rm].value;
    uint32_t rn_32 = genRegisters[rn].value;
    uint32_t ra_32 = genRegisters[rn].value;
    uint32_t result = x ? ra_32 - (rn_32 * rm_32) : ra_32 + (rn_32 * rm_32);
    genRegisters[rd].value = result;
  } else {
    genRegisters[rd].value =
        x ? genRegisters[ra].value -
            (genRegisters[rn].value * genRegisters[rm].value)
          : genRegisters[ra].value +
            (genRegisters[rn].value * genRegisters[rm].value);
  }
}