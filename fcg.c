/**
 * @author  Mukund Shah
 * @note Lab 9: Illustrate Target Code Generation for a given expression.
 * @file fcg.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE 100

// Structure to represent a three-address code instruction
typedef struct {
  char op[2]; // +, -, *, =
  char arg1[10];
  char arg2[10];
  char result[10];
} Instruction;

// Function to generate target code (x86 assembly) from three-address code
void generate_target_code(Instruction *code, int count) {
  printf("section .text\n");
  printf("global _start\n\n");
  printf("_start:\n");

  for (int i = 0; i < count; i++) {
    Instruction instr = code[i];

    switch (instr.op[0]) { // Access the first character of op
    case '+':
      printf("    mov eax, [%s]\n", instr.arg1);
      printf("    add eax, [%s]\n", instr.arg2);
      printf("    mov [%s], eax\n", instr.result);
      break;
    case '-':
      printf("    mov eax, [%s]\n", instr.arg1);
      printf("    sub eax, [%s]\n", instr.arg2);
      printf("    mov [%s], eax\n", instr.result);
      break;
    case '*':
      printf("    mov eax, [%s]\n", instr.arg1);
      printf("    imul dword [%s]\n", instr.arg2);
      printf("    mov [%s], eax\n", instr.result);
      break;
    case '=':
      printf("    mov eax, [%s]\n", instr.arg1);
      printf("    mov [%s], eax\n", instr.result);
      break;
    }
  }

  // Exit syscall
  printf("    mov eax, 1\n");
  printf("    xor ebx, ebx\n");
  printf("    int 0x80\n");

  printf("\nsection .data\n");
  for (int i = 0; i < count; i++) {
    Instruction instr = code[i];
    if (instr.op[0] == '=') {
      printf("    %s dd 0\n", instr.result);
    }
  }

  printf("\nsection .bss\n");
  for (int i = 0; i < count; i++) {
    Instruction instr = code[i];
    if (instr.op[0] != '=') {
      printf("    %s resd 1\n", instr.result);
    }
  }

  printf("\n");

  return;
}

int main() {
  Instruction code[MAX_CODE];
  int count = 0;

  // Example: generate three-address code for the expression: a = b + c * d
  strcpy(code[count].op, "=");
  strcpy(code[count].arg1, "b");
  strcpy(code[count].arg2, "");
  strcpy(code[count].result, "t1");
  count++;

  strcpy(code[count].op, "=");
  strcpy(code[count].arg1, "c");
  strcpy(code[count].arg2, "");
  strcpy(code[count].result, "t2");
  count++;

  strcpy(code[count].op, "=");
  strcpy(code[count].arg1, "d");
  strcpy(code[count].arg2, "");
  strcpy(code[count].result, "t3");
  count++;

  strcpy(code[count].op, "*");
  strcpy(code[count].arg1, "t2");
  strcpy(code[count].arg2, "t3");
  strcpy(code[count].result, "t4");
  count++;

  strcpy(code[count].op, "+");
  strcpy(code[count].arg1, "t1");
  strcpy(code[count].arg2, "t4");
  strcpy(code[count].result, "a");
  count++;

  // Generate target code
  generate_target_code(code, count);

  return 0;
}
