#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 32
#define MEMORY_SIZE (1 << 21) // 2MB
#define MASK_32 ((UINT32_C(1) << 31) - 1)
#define MASK_64 ((UINT64_C(1) << 63) - 1)

#define NUM_GENREGS 31
// registers: X00 -> X30

// misc
uint8_t getBit(uint32_t, uint8_t);
uint32_t getSubWord(uint8_t, uint8_t, uint32_t);
uint64_t getSubWord64(uint8_t, uint8_t, uint64_t);
int64_t ror(int64_t, uint8_t, bool);
int64_t lsr(int64_t, uint8_t, bool);
int64_t lsl(int64_t, uint8_t, bool);
int64_t asr(int64_t, uint8_t, bool);

int64_t signExtend(int32_t, int);
//

// Data Processing (Immediate) functions           NEED TO UPDATE FLAGS!!!
void dataProcessImmediate(uint32_t);

void arithmetic(uint32_t, bool);
void add(uint8_t, uint64_t, uint8_t, bool);
void adds(uint8_t, uint64_t, uint8_t, bool);
void sub(uint8_t, uint64_t, uint8_t, bool);
void subReg(uint8_t, int64_t, uint8_t, bool);
void subs(uint8_t, uint64_t, uint8_t, bool);

void wideMove(uint32_t, bool);
void movn(uint64_t, uint8_t, bool);
void movz(uint64_t, uint8_t);
void movk(uint64_t, uint8_t, uint8_t, bool);
//

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

// Single Data Transfer functions
uint8_t singleDataTransfer(uint32_t);

uint64_t unsignedImmediateOffset(uint32_t, uint8_t);
uint64_t preIndexed(uint32_t);
uint64_t postIndexed(uint32_t);
uint64_t registerOffset(uint32_t);
uint64_t loadLiteral(uint32_t);

void load(uint64_t, uint8_t, uint8_t);
void store(uint64_t, uint8_t, uint8_t);

// Branch Instructions functions
void branchInstructions(uint32_t);
void unconditionalOffset(int64_t);
void unconditionalRegister(uint8_t);
void conditionalBranches(int64_t, uint8_t);




struct Reg {
    char name[4];
    uint64_t value; // stands for Xn -> in binary
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

///////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    FILE *file;
    // Make sure that there is a second parameter(binary file)
    if (argc != 3) {
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

    initialiseGeneralReg();

    // no. words
    int numWords = fileSize / sizeof(uint32_t);

    // process each word
    uint32_t word;
    while (true) {
        word = *(uint32_t * )(memory + PC.value );
        if (word == 0x8a000000) {
            break;
        } else if (PC.value / 4 >= numWords) {
            break;
        } else if (getSubWord(26, 28, word) == 4) { //0b100x
            dataProcessImmediate(word);
        } else if (getSubWord(25, 27, word) == 5) { //0bx101
            dataProcessRegister(word);
        } else if ((getBit(word, 27) == 1) && (getBit(word, 25) == 0))  { //0bx1x0
            singleDataTransfer(word);
        } else if (getSubWord(26, 28, word) == 5) { //0b101x
            // Branches
            branchInstructions(word);
            PC.value -= 4;
        }
        PC.value += 4;
    }

    cpuState(argv[2]);
    free(memory);
    return 0;
}
///////////////////////////////////////////////////////////////////

// extends N-bit number to 64 bits
int64_t signExtend(int32_t simmN, int N) {
    int64_t result;
    if (simmN & (1 << (N - 1))) {
        // If the sign bit is set (negative number)
        // Perform sign extension with 1s
        result = (int64_t)simmN | (0xFFFFFFFFFFFFFFFFULL << N);
    } else {
        // If the sign bit is not set (positive number)
        // Perform sign extension with 0s
        result = (int64_t)simmN & ((1ULL << N) - 1);
    }
    return result;
}

// @param i - ensure it's between 0 and 31 inclusive.
uint8_t getBit(uint32_t word, uint8_t i) { return (word >> i) & 1; }

// accesses subsection of instruction between indexes start to end, inclusively.
// PRE: end>=start
uint32_t getSubWord(uint8_t start, uint8_t end, uint32_t word) {
    int width = end - start + 1;
    uint32_t subword = (word >> start) & ((1 << width) - 1);
    return subword;
}

uint64_t getSubWord64(uint8_t start, uint8_t end, uint64_t data) {
    int width = end - start + 1;
    uint64_t subword64 = (data >> start) & ((1 << width) - 1);
    return subword64;
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
        int64_t firstBit = getSubWord64(0, 0, result) ? UINT64_C(1) << 63 : 0;
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
        uint8_t bit = getSubWord64(63, 63, value);
        value = value >> shiftAmount;
        if (bit) {
            value += bit << (64 - shiftAmount);
        }
        return value;
    }
}

int64_t lsl(int64_t value, uint8_t shiftAmount, bool Xn) {
    if (!Xn) {
        uint32_t val = value;
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
        if (!(rd == 31)) {
            add(rn, Op_2, rd, Xn);
        }
    } else if (opc == 1) /*0b01*/ {
        adds(rn, Op_2, rd, Xn);
    } else if (opc == 2) /*0b10*/ {
        if (!(rd == 31)) {
            sub(rn, Op_2, rd, Xn);
        }
    } else /*0b11*/ {
        subs(rn, Op_2, rd, Xn);
    }
}

void add(uint8_t rn, uint64_t Op_2, uint8_t rd, bool Xn) {
    genRegisters[rd].value = Xn ? genRegisters[rn].value + (Op_2)
                                : (((int32_t)genRegisters[rn].value) + Op_2) & 0xFFFFFFFFLL;
}


void adds(uint8_t rn, uint64_t Op_2, uint8_t rd, bool Xn) {
    if (!Xn) {
        uint32_t res;
        uint32_t rn_val = genRegisters[rn].value;
        uint32_t Op_val = Op_2;
        pState.C = __builtin_uadd_overflow(rn_val, Op_val, &res);
        pState.V = __builtin_add_overflow(rn_val, Op_val, &res);
        pState.Z = !res;
        pState.N = (res >> 31) & 1;
        if (!(rd == 31)) {
            genRegisters[rd].value = res;
        }
    } else {
        uint64_t res;
        uint64_t rn_val = genRegisters[rn].value;
        uint64_t Op_val = Op_2;
        pState.C = __builtin_uaddl_overflow(rn_val, Op_val, &res);
        pState.V = __builtin_saddl_overflow(rn_val, Op_val, (int64_t *)&res);
        pState.Z = !res;
        pState.N = (res >> 63) & 1;
        if (!(rd == 31)) {
            genRegisters[rd].value = res;
        }
    }
}

void sub(uint8_t rn, uint64_t Op_2, uint8_t rd, bool Xn) {
    genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
                                : ((((int32_t)genRegisters[rn].value) - Op_2) & 0xFFFFFFFFLL);
}

void subReg(uint8_t rn, int64_t Op_2, uint8_t rd, bool Xn) {
    genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
                                : ((((int32_t)genRegisters[rn].value) - Op_2) & 0xFFFFFFFFLL);
}

void subs(uint8_t rn, uint64_t Op_2, uint8_t rd, bool Xn) {
    genRegisters[rd].value = Xn ? genRegisters[rn].value - Op_2
                                : ((int32_t)genRegisters[rn].value) - Op_2;
    if (!Xn) {
        uint32_t res;
        uint32_t rn_val = genRegisters[rn].value;
        uint32_t Op_val = Op_2;
        pState.C = !__builtin_usub_overflow(rn_val, Op_val, &res);
        pState.V = __builtin_ssub_overflow(rn_val, Op_val, (int32_t *)&res);
        pState.Z = !res;
        pState.N = (res >> 31) & 1;
        if (!(rd == 31)) {
            genRegisters[rd].value = res;
        }
    } else {
        uint64_t res;
        uint64_t rn_val = genRegisters[rn].value;
        uint64_t Op_val = Op_2;
        pState.C = !__builtin_usubl_overflow(rn_val, Op_val, &res);
        pState.V = __builtin_ssubl_overflow(rn_val, Op_val, (int64_t *)&res);
        pState.Z = !res;
        pState.N = (res >> 63) & 1;
        if (!(rd == 31)) {
            genRegisters[rd].value = res;
        }
    }
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
    uint8_t sf = getBit(word, 31);
    if (M == 0 && (getBit(word, 24) == 1)) {
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
            Op_2 = asr(genRegisters[rm].value, operand, Xn);
            break;
    }

    switch (opc) {
        case 0: /*0b00*/
            add(rn, Op_2, rd, Xn);
            break;
        case 1: /*0b01*/
            adds(rn, Op_2, rd, Xn);
            break;
        case 2: /*0b10*/
            sub(rn, Op_2, rd, Xn);
            break;
        default:
            subs(rn, Op_2, rd, Xn); /*0b11*/
            break;
    }
}

void logical(uint32_t word, bool Xn) {
    uint8_t rd = getSubWord(0, 4, word);
    if (rd == 31)
        return;
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
    pState.N = Xn ? getSubWord64(63, 63, result) : getBit(result, 31);
    pState.Z = result ? pState.Z : 1;
    pState.C = 0;
    pState.V = 0;
    return result;
}

int64_t bics(int64_t rn, uint64_t Op_2, bool Xn) {
    int64_t result = bic(rn, Op_2, Xn);
    pState.N = Xn ? getSubWord64(63, 63, result) : getBit(result, 31);
    pState.Z = result ? pState.Z : 1;
    pState.C = 0;
    pState.V = 0;
    return result;
}

void multiply(uint32_t word, bool Xn) {
    //  assert(getSubWord(29, 30, word) == 0);
    uint8_t rd = getSubWord(0, 4, word);
    if (rd == 31)
        return;
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

///STORING AND LOADING
uint8_t singleDataTransfer(uint32_t word){
    //size of the load
    uint8_t sf = getBit(word, 30);
    uint8_t rt_index =  getSubWord(0, 4, word); // target register address.
    int8_t l = getBit(word, 22); // 1 if load, 0 if store, else invald address.
    uint64_t address;
////
    if (getBit(word,31) == 1){ //Single Data Transfer
        address =
                (getBit(word,24) == 1) ? unsignedImmediateOffset(word,sf) : //Unsigned Immediate Offset or...
                ((getBit(word,21)==1) ? registerOffset(word) : //...Register Offset or...
                 ((getBit(word,11) == 1) ? preIndexed(word) : postIndexed(word)));//...Pre or Post Index
    }else {                     // Load Literal
        address = loadLiteral(word);
        l = 1;
    }
////

    //must be within necessary range
    if (address >= (1<<21) || address <= 0){l=-1;printf("Address out of range\n");}

    if (l==1){
        //LOADING TO REGISTER OR STORING IN MAIN MEMORY
        load(address,rt_index,sf);
    }else if (l==0) {
        store(address,rt_index,sf);
    }else {
        return EXIT_FAILURE;// if invalid address
    }
    return EXIT_SUCCESS;
} //Pre U = 1, sf = 1 or 0
uint64_t unsignedImmediateOffset(uint32_t word,uint8_t sf){
    uint8_t xn_index = getSubWord(5,9,word); //register address
    uint64_t xn = genRegisters[xn_index].value;
    uint64_t imm12 = getSubWord(10,12,word);
    uint64_t uoffset = (sf == 1) ? imm12 << 3 : imm12 << 2;


    uint64_t address = uoffset + xn;
    return address;
}
uint64_t preIndexed(uint32_t word){
    int64_t simm9 =  getSubWord(12,20,word);
    int64_t signedBit = simm9 & (1<<8); // access value at final position
    simm9 = simm9 - signedBit - signedBit;
    uint8_t xn_index = getSubWord(5,9,word); //register address
    genRegisters[xn_index].value += simm9;


    uint64_t address = genRegisters[xn_index].value;

    return address;
}
uint64_t postIndexed(uint32_t word){
    int64_t simm9 = (int64_t) getSubWord(12,20,word);
    int64_t signedBit = simm9 & (1<<8);
    simm9 = simm9 - signedBit - signedBit;
    int64_t xn = genRegisters[getSubWord(5,9,word)].value;//old register value
    genRegisters[getSubWord(5,9,word)].value +=  simm9; //new register value

    uint64_t address = xn;
    return address;
}
uint64_t registerOffset(uint32_t word){
    uint8_t xn_index = getSubWord(5,9,word);
    uint64_t xn = genRegisters[xn_index].value;
    uint8_t xm_index = getSubWord(16,20,word);
    uint64_t xm = genRegisters[xm_index].value;

    uint64_t address = xn + xm;
    return address;
}

int64_t signExtendTo64(int64_t value,uint8_t length) {
    // Extract the sign bit (11th bit)
    int64_t signBit = (value >> (length-1)) & 1;

    // Perform sign extension if the sign bit is 1
    if (signBit == 1) {
        value |= (~0ULL << length); // Fill the higher bits with 1's
    }
    return value;
}

uint64_t loadLiteral(uint32_t word){
    int64_t simm19 = (int64_t) getSubWord(5,23,word);
    int64_t simm19Times4 = simm19 << 2;
    int64_t signExtended = signExtendTo64(simm19Times4,21);

    uint64_t address = (uint64_t) (signExtended + PC.value);
    return address;
}

//Pre: sf = 1 or 0 , address is non-negative and less than 2^21
void load(uint64_t address,uint8_t rt_index,uint8_t sf){
    int n = (sf==1) ? 8:4; //decides whether registers are 64bit version or 32bit version
    uint64_t loadedValue = 0;
    for (int i=0;i<n;i++){
        loadedValue += (memory[address+i]<<i*8);
    }
    genRegisters[rt_index].value = loadedValue;
}
//Pre sf = 1 or 0 , address is non-negative and less than 2^21
void store(uint64_t address,uint8_t rt_index,uint8_t sf){
    int n = (sf==1) ? 8:4;//decides whether registers are 64bit version or 32bit version
    for (int i = 0; i<n;i++){
        memory[address+i] = (uint8_t) getSubWord64(i*8,i*8+7,genRegisters[rt_index].value);//
    }
}

// Branch Instructions functions
void branchInstructions(uint32_t word) {
    uint8_t test = getSubWord(29, 31, word);
    // does reach this
    if (test == 0) { // 0b000
        // branch1 tests this
        unconditionalOffset(signExtend(getSubWord(0, 25, word), 26));
    } else if (test == 6) { // 0b110
        unconditionalRegister(getSubWord(5, 9, word));
    } else if (test == 2) { // 0b010
        conditionalBranches(signExtend(getSubWord(5, 23, word), 19), getSubWord(0, 3, word));
    } else {
        printf("NO");
    }
}

// signed offset
void unconditionalOffset(int64_t simm26) {
    PC.value = (simm26 << 2);
}

// address stored in register
void unconditionalRegister(uint8_t xn) {
    if (xn != 0b11111) {
        PC.value = genRegisters[xn].value;
    }
}
void conditionalBranches(int64_t simm19, uint8_t cond) {
    int64_t offset = simm19 << 2;
    if (cond == 0 && pState.Z == 1) { // EQ - 0b0000
        PC.value += signExtend(offset, 19);
    } else if (cond == 1 && pState.Z == 0) { // NE - 0b0001
        PC.value += signExtend(offset, 19);
    } else if (cond == 10 && pState.N == pState.V) { // GE - 0b1010
        PC.value += signExtend(offset, 19);
    } else if (cond == 11 && pState.N != pState.V) { // LT - 0b1011
        PC.value += signExtend(offset, 19);
    } else if (cond == 12 && pState.Z == 0 && pState.N == pState.V) { // GT - 0b1100
        PC.value += signExtend(offset, 19);
    } else if (cond == 13 && !(pState.Z == 0 && pState.N == pState.V)) { // LE - 0b1101
        PC.value += signExtend(offset, 19);
    } else if (cond == 14) { // AL - 0b1110
        PC.value += signExtend(offset, 19);
    }
}