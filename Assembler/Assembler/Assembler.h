#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>

// Data Types
typedef struct {
    char name[50];
    int place;
} Label;

typedef struct {
    char opcode[51];
    char rd[51];
    char rs[51];
    char rt[51];
    char imm[51];
} Instruction;

// Function Prototypes
int firstRun(Label* labels, FILE* file, int* numLabels);
void secondRun(Label* labels, Instruction* instructions, FILE* asmFile, FILE* outFile, int numLabels, int numOfLines);
char* opcodeToHex(char* opcode);
char regToHex(char* regName);
char* immToHex(char* numToReturn, const char* immField, Label* labels, int numLabels);  // const-correctness
void lowercaseWord(char* str);
void wordInstruction(Instruction inst, FILE* outFile, int PC);
int parseNumber(const char* str);

#endif // ASSEMBLER_H
