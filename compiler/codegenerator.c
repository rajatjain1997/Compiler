#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tree.h"
#include"quadruple.h"
#include"list.h"
#include"symboltable.h"
#include"codegenerator.h"
#include"token.h"

void writebase(FILE* fp) {
  fprintf(fp, "section .data\n");
  fprintf(fp, "stackbase db 0\n");
  fprintf(fp, "section .bss\n");
  fprintf(fp, "temp1\tRESB\t20\n");
  fprintf(fp, "temp2\tRESB\t20\n");
  fprintf(fp, "section .text\n");
  fprintf(fp, "global _start\n");
  fprintf(fp, "\tmov esp, stackbase\n");
  fprintf(fp, "mov ebp, esp\n");
  fprintf(fp, "def:\n");
  fprintf(fp, "\tpop edx\n");
  fprintf(fp, "\tpusher:\n");
  fprintf(fp, "\t\tpush cx\n");
  fprintf(fp, "\tloope pusher\n");
  fprintf(fp, "\tpush edx\n");
  fprintf(fp, "ret\n");
  fprintf(fp, "quit:\n");
  fprintf(fp, "\tmov bx, 0\n");
  fprintf(fp, "\tmov ax, 1\n");
  fprintf(fp, "\tint 80h\n");
}

void writeend(FILE* fp) {
  fprintf(fp, "call quit\n");
}

void convertToRegister(FILE* fp, Address* addr, char reg[]) {
  struct symbolTableEntry* tempentree;
  switch (addr->type) {
    case 0:
      tempentree = addr->address.entry;
      if(tempentree->value.identry->type->type==INT || tempentree->value.identry->type->type==REAL)
        fprintf(fp, "mov %s, [ebp + %d]\n", reg, tempentree->value.identry->offset);
      else {
        fprintf(fp, "mov %s, ebp\n", reg);
        fprintf(fp, "add %s, %d\n", reg, tempentree->value.identry->offset);
      }
    break;
    case INT:
      fprintf(fp, "mov %s, %d\n", reg, addr->address.integer);
    break;
  }
}

void convertToMemory(FILE* fp, Address* addr, char reg[]) {
  struct symbolTableEntry* tempentree;
  tempentree = addr->address.entry;
  if(tempentree->value.identry->type->type==INT || tempentree->value.identry->type->type==REAL)
    fprintf(fp, "mov [ebp + %d], %s\n", tempentree->value.identry->offset, reg);
}

void writeCode(FILE* fp, Quadruple* code, SymbolTable st) {
  Address *addr1, *addr2, *addr3;
  addr1 = code->op[0];
  addr2 = code->op[1];
  addr3 = code->op[2];
  switch(code->operator) {
    case OP_PLUS:
    break;
    case OP_MINUS:
    break;
    case OP_DIV:
    break;
    case OP_MUL:
    break;
    case OP_JLT:
      fprintf(fp, "jl %s\n", ((char*) addr1->address.entry));
    break;
    case OP_JLE:
      fprintf(fp, "jle %s\n", ((char*) addr1->address.entry));
    break;
    case OP_JEQ:
      fprintf(fp, "je %s\n", ((char*) addr1->address.entry));
    break;
    case OP_JGT:
      fprintf(fp, "jg %s\n", ((char*) addr1->address.entry));
    break;
    case OP_JGE:
      fprintf(fp, "jge %s\n", ((char*) addr1->address.entry));
    break;
    case OP_JNE:
      fprintf(fp, "jne %s\n", ((char*) addr1->address.entry));
    break;
    case OP_CMP:
      convertToRegister(fp, addr1, "eax");
      convertToRegister(fp, addr2, "ebx");
      fprintf(fp, "cmp eax, ebx\n");
    break;
    case OP_JMP:
      fprintf(fp, "jmp %s\n", ((char*) addr1->address.entry));
    break;
    case OP_LABEL:
      fprintf(fp, "%s:\n", ((char*) addr1->address.entry));
    break;
    case OP_MOV:
    //Remember to differentaiate between moves for strings and matrices.
    break;
    case OP_PUSH:
      fprintf(fp, "mov cx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->size);
      fprintf(fp, "call def\n");
    break;
    case OP_ADDRPLUS:
      convertToRegister(fp, addr1, "eax");
      convertToRegister(fp, addr2, "ebx");
      fprintf(fp, "add eax, ebx\n");
      convertToMemory(fp, addr3, "eax");
    break;
    case OP_READ:
      fprintf(fp, "mov edx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->size);
      fprintf(fp, "mov ecx, ebp\n");
      fprintf(fp, "add ecx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->offset);
      fprintf(fp, "mov ebx, 0\n");
      fprintf(fp, "mov eax, 3\n");
      fprintf(fp, "int 80h\n");
    break;
    case OP_PRINT:
      fprintf(fp, "mov edx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->size);
      fprintf(fp, "mov ecx, ebp\n");
      fprintf(fp, "add ecx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->offset);
      fprintf(fp, "mov ebx, 1\n");
      fprintf(fp, "mov eax, 4\n");
      fprintf(fp, "int 80h\n");
    break;
    default:
      fprintf(fp, ";Code Generation Skipped for operator %d", code->operator);
  }
}

void generateCode(char* filename, List code, SymbolTable st) {
  FILE* fp = fopen(filename, "w");
  int define = 0;
  writebase(fp);
  Element temp;
  temp = code->first;
  while(temp!=NULL) {
    if(temp->data.value.quadruple->operator==OP_DEFINE) {
      define = 1;
      while(define!=0) {
        if(temp->data.value.quadruple->operator==OP_RET) {
          define--;
        } else if(temp->data.value.quadruple->operator==OP_DEFINE) {
          define++;
        }
        temp = temp->next;
      }
    }
    writeCode(fp, temp->data.value.quadruple, st);
    temp = temp->next;
  }
  writeend(fp);
  fclose(fp);
}
