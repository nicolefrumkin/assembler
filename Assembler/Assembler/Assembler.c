#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h> 
#include "Assembler.h"

int main(int argc, char *argv[]) {
	Label labels[4096];
	Instruction *instructions[4096];
	int numLabels = 0;

	FILE *asmFile = fopen(argv[1], "r");
	if (asmFile == NULL) {
		fprintf(stderr, "Failed to open file\n");
		return 1;
	}

	int numOfLines = firstRun(labels, asmFile, &numLabels); // saves labels and counts PC

	FILE* outFile = fopen(argv[2], "w");

	if (outFile == NULL) {
		fprintf(stderr, "Failed to create file\n");
		return 1;
	}

	rewind(asmFile); //returns file pointer to the beginning

	secondRun(labels, instructions, asmFile, outFile, numLabels, numOfLines);

	fclose(asmFile);
	fclose(outFile);

	return 0;
}


int firstRun(Label* labels, FILE* file, int* numLabels) {
	int PC = 0;
	char line[500];
	char firstWord[50];
	char secondWord[50];
	int j = 0;

	while (fgets(line, sizeof(line), file) != NULL) {

		if (sscanf(line, "%s%s", firstWord, secondWord) < 1) {
			continue;
		}
		// Check if line is a comment
		if (firstWord[0] == "#") {
			continue;
		}
		// Check if the first word ends with ':'
		int len = strlen(firstWord);
		if (len > 0 && firstWord[len - 1] == ':') {
			firstWord[len - 1] = '\0'; // Remove the ':' from the end
			// Copy the label name and set its place
			strncpy((labels)[*numLabels].name, firstWord, sizeof((labels)[*numLabels].name) - 1);
			(labels)[*numLabels].name[sizeof((labels)[*numLabels].name) - 1] = '\0'; // Ensure null-termination
			(labels)[*numLabels].place = PC;
			(*numLabels)++; // Increment the count of labels
			if (strlen(secondWord) == 0) { // Check if secondWord is empty
				PC--;
			}
		}

		//checks if a line has imm field, if it does, it increments PC by 2
		char* token;
		int tokenIndex = 0;
		char tokens[5][50] = { "", "", "", "", "" };
		token = strtok(line, " ,\t\n");
		while (token != NULL && tokenIndex < 5) {
			strncpy(tokens[tokenIndex], token, sizeof(tokens[tokenIndex]) - 1);
			tokens[tokenIndex][sizeof(tokens[tokenIndex]) - 1] = '\0';
			tokenIndex++;
			token = strtok(NULL, " ,\t\n");
		}

		// Check if any token is $imm
		int foundImm = 0;
		for (int i = 0; i < tokenIndex; i++) {
			if (strcmp(tokens[i], "$imm") == 0) {
				foundImm = 1;
				break;
			}
		}

		// Increment PC based on the presence of $imm
		if (foundImm) {
			PC+=2; // Increment PC by 2 if $imm is found
		}
		else {
			PC++;
		}
	}
	return PC;
}

void secondRun(Label* labels, Instruction* instructions, FILE* asmFile, FILE* outFile,int numLabels, int numOfLines) {
	int PC = 1;
	int instIdx = 0;
	char line[500];

	while (fgets(line, sizeof(line), asmFile) != NULL) {
		char* token;
		int tokenIndex = 0;
		Instruction inst = { "", "", "", "", "" };

		token = strtok(line, " ,\t\n");
		// Skip the label if the first token ends with ':'
		if (token != NULL && strchr(token, ':') != NULL) {
			token = strtok(NULL, " ,\t\n");  // Move to the next token
		}
		// Skip comment lines
		if (token != NULL && token[0]=="#") {
			continue;  // Move to the next token
		}

		while (token != NULL) {
			switch (tokenIndex) {
			case 0: // Opcode
				strncpy(inst.opcode, token, sizeof(inst.opcode) - 1);
				inst.opcode[sizeof(inst.opcode) - 1] = '\0';
				break;
			case 1: // rd
				strncpy(inst.rd, token, sizeof(inst.rd) - 1);
				inst.rd[sizeof(inst.rd) - 1] = '\0';
				break;
			case 2: // rs
				strncpy(inst.rs, token, sizeof(inst.rs) - 1);
				inst.rs[sizeof(inst.rs) - 1] = '\0';
				break;
			case 3: // rt
				strncpy(inst.rt, token, sizeof(inst.rt) - 1);
				inst.rt[sizeof(inst.rt) - 1] = '\0';
				break;
			case 4: // imm
				strncpy(inst.imm, token, sizeof(inst.imm) - 1);
				inst.imm[sizeof(inst.imm) - 1] = '\0';
				break;
			}

			tokenIndex++;
			token = strtok(NULL, " ,\t\n");
		}

		// Store the instruction in the instructions array
		instructions[instIdx++] = inst;

		// Increment the PC for each instruction processed
		PC++;
	}
		// Write instructions to outFile
		PC--;
		char numToReturn[6];
		int i = 0;
		while (PC > 0) {
			Instruction inst = instructions[i];
			if (strcmp(inst.opcode,".word")==0) {
				wordInstruction(inst, outFile, numOfLines);
			}
			else if (opcodeToHex(inst.opcode) != NULL) {
				fprintf(outFile, "%s", opcodeToHex(inst.opcode));
				fprintf(outFile, "%c", regToHex(inst.rd));
				fprintf(outFile, "%c", regToHex(inst.rs));
				fprintf(outFile, "%c\n", regToHex(inst.rt));
				if (strcmp(inst.rd, "$imm") == 0 || strcmp(inst.rs, "$imm") == 0 || strcmp(inst.rt, "$imm") == 0) {
					fprintf(outFile, "%s\n", immToHex(numToReturn, inst.imm, labels,numLabels));
				}
			}
			fflush(outFile);

			i++;
			PC--;
		}
}

char* opcodeToHex(char* opcode) {
	lowercaseWord(opcode);
	if (strcmp(opcode, "add") == 0) return "00";
	else if (strcmp(opcode, "sub") == 0) return "01";
	else  if (strcmp(opcode, "mul") == 0) return "02";
	else if (strcmp(opcode, "and") == 0) return "03";
	else if (strcmp(opcode, "or") == 0) return "04";
	else if (strcmp(opcode, "xor") == 0) return "05";
	else if (strcmp(opcode, "sll") == 0) return "06";
	else if (strcmp(opcode, "sra") == 0) return "07";
	else if (strcmp(opcode, "srl") == 0) return "08";
	else if (strcmp(opcode, "beq") == 0) return "09";
	else if (strcmp(opcode, "bne") == 0) return "0A";
	else if (strcmp(opcode, "blt") == 0) return "0B";
	else if (strcmp(opcode, "bgt") == 0) return "0C";
	else if (strcmp(opcode, "ble") == 0) return "0D";
	else if (strcmp(opcode, "bge") == 0) return "0E";
	else if (strcmp(opcode, "jal") == 0) return "0F";
	else if (strcmp(opcode, "lw") == 0) return "10";
	else if (strcmp(opcode, "sw") == 0) return "11";
	else if (strcmp(opcode, "reti") == 0) return "12";
	else if (strcmp(opcode, "in") == 0) return "13";
	else if (strcmp(opcode, "out") == 0) return "14";
	else if (strcmp(opcode, "halt") == 0) return "15";
	else return NULL;
}

char regToHex(char* regName) {
	if (strcmp(regName, "$zero") == 0) return '0';
	else if (strcmp(regName, "$imm") == 0) return '1';
	else if (strcmp(regName, "$v0") == 0) return '2';
	else if (strcmp(regName, "$a0") == 0) return '3';
	else if (strcmp(regName, "$a1") == 0) return '4';
	else if (strcmp(regName, "$a2") == 0) return '5';
	else if (strcmp(regName, "$a3") == 0) return '6';
	else if (strcmp(regName, "$t0") == 0) return '7';
	else if (strcmp(regName, "$t1") == 0) return '8';
	else if (strcmp(regName, "$t2") == 0) return '9';
	else if (strcmp(regName, "$s0") == 0) return 'A';
	else if (strcmp(regName, "$s1") == 0) return 'B';
	else if (strcmp(regName, "$s2") == 0) return 'C';
	else if (strcmp(regName, "$gp") == 0) return 'D';
	else if (strcmp(regName, "$sp") == 0) return 'E';
	else if (strcmp(regName, "$ra") == 0) return 'F';
	else return NULL;
}

char* immToHex(char* numToReturn, const char* immField, Label* labels, int numLabels) {
	// Check if the number is decimal
	int isDecimal = 1;
	int isNegative = 0;

	for (int i = 0; immField[i] != '\0'; i++) {
		if (immField[i] == '-') {
			isNegative = 1;
		}
		else if (!isdigit(immField[i])) {
			isDecimal = 0;
			break;
		}
	}

	if (isDecimal) {
		int decimalNumber = atoi(immField); // Convert string to integer
		if (decimalNumber < 0) {
			decimalNumber = (1 << 20) + decimalNumber; // Convert negative number to 20-bit two's complement
		}
		sprintf(numToReturn, "%05X", decimalNumber);
	}
	// Convert hexadecimal to 5 digits hexadecimal
	else if (immField[0] == '0' && (immField[1] == 'x' || immField[1] == 'X')) {
		int hexNumber;
		sscanf(immField, "%x", &hexNumber); // Scan as hexadecimal integer
		sprintf(numToReturn, "%05X", hexNumber);
	}
	// Convert label to hexadecimal number
	else {
		for (int i = 0; i < numLabels; i++) {
			if (strcmp(labels[i].name, immField) == 0) {
				sprintf(numToReturn, "%05X", labels[i].place);
				break;
			}
		}
	}
	return numToReturn;
}

void wordInstruction(Instruction inst, FILE* outFile, int PC) {
	int currentLines = PC;
	int lineNumber = parseNumber(inst.rd);
	int number = parseNumber(inst.rs);

	if (currentLines < lineNumber) {
		// Write missing lines with 00000 if necessary
		while (currentLines <= lineNumber) {
			fprintf(outFile, "00000\n"); // Note the newline character here
			currentLines++;
			fflush(outFile); // Flushes the buffer and writes to the file
		}
		fseek(outFile, lineNumber*7, SEEK_SET);  // Each line will be 6 characters: 5 hex digits + newline
		fprintf(outFile, "%05X\n", number);
	}
	else {
		// Move the file pointer to the specified position
		long position = lineNumber * 7;
		fseek(outFile, position, SEEK_SET);

		// Write the number at the specified line number
		fprintf(outFile, "%05X", number); // Note the newline character here
		currentLines = lineNumber; // Update the current line counter
		fflush(outFile);
	}
}


int parseNumber(const char* str) {
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		return strtol(str, NULL, 16);  // Convert hexadecimal string to integer
	}
	return atoi(str);  // Convert decimal string to integer
}

void lowercaseWord(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		str[i] = tolower(str[i]); // Convert each character to lowercase
	}
}