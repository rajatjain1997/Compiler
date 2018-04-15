#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"quadruple.h"
#include"list.h"
#include"symboltable.h"

void writebase(FILE* fp) {
  fprintf(fp, "section .data\n");
  fprintf(fp, "stackbase db 0\n");
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
  fprintf(fp, "\tint 80h");
}

void writeend(FILE* fp) {
  fprintf(fp, "call quit\n");
}

void convertToRegister(FILE* fp, Address* addr, char reg[]) {
  struct symbolTableEntry* tempentree;
  switch (address->type) {
    case 0:
      tempentree = addr->address.entry;
      if(tempentree->value.identry->type==INT || tempentree->value.identry->type==REAL)
        fprintf(fp, "mov %s, [ebp + %d]", reg, tempentree->value.identry->offset);
      else {
        fprintf(fp, "mov %s, ebp", reg);
        fprintf(fp, "add %s, %d", reg, tempentree->value.identry->offset);
      }
    break;
    case INT:
      fprintf(fp, "mov %s, %d", reg, addr->address.integer);
    break;
  }
}

void writeCode(FILE* fp, Quadruple* code, SymbolTable st) {
  Address *addr1, *addr2, *addr3;
  addr1 = code->op[0]->address;
  addr2 = code->op[1]->address;
  addr3 = code->op[2]->address;
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
      fprintf(fp, "jl %s", ((char*) addr1.entry));
    break;
    case OP_JLE:
      fprintf(fp, "jle %s", ((char*) addr1.entry));
    break;
    case OP_JEQ:
      fprintf(fp, "je %s", ((char*) addr1.entry));
    break;
    case OP_JGT:
      fprintf(fp, "jg %s", ((char*) addr1.entry));
    break;
    case OP_JGE:
      fprintf(fp, "jge %s", ((char*) addr1.entry));
    break;
    case OP_JNE:
      fprintf(fp, "jne %s", ((char*) addr1.entry));
    break;
    case OP_CMP:
    break;
    case OP_JMP:
      fprintf(fp, "jmp %s", ((char*) addr1.entry));
    break;
    case OP_LABEL:
      fprintf(fp, "%s:\n", ((char*) addr1.entry));
    break;
    case OP_MOV:
    //Remember to differentaiate between moves for strings and matrices.
    break;
    case OP_PUSH:
      fprintf(fp, "mov cx, %d\n", ((struct symbolTableEntry*) addr1.entry)->value.identry->size);
      fprintf(fp, "call def");
    break;
    case OP_ADDRPLUS:
    break;
    case OP_READ:
    break;
    case OP_PRINT:
    break;
    default:
      fprintf(fp, ";Code Generation Skipped for operator %d", code->operator);
  }
}

void generateCode(FILE* fp, List code, SymbolTable st) {
  writebase(fp);
  Element temp;
  temp = code->first;
  while(temp!=NULL) {
    if(temp->data.value.quadruple->operator==OP_DEFINE) {
      while(temp->data.value.quadruple->operator!=OP_RET) {
        temp = temp->next;
      }
    }
    writeCode(fp, temp->data.value.quadruple, st);
    temp = temp->next;
  }
  writeend(fp);
}
