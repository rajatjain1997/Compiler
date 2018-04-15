#include<stdio.h>
#include<stdlib.h>
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
    break;
    case OP_JLE:
    break;
    case OP_JEQ:
    break;
    case OP_JGT:
    break;
    case OP_JGE:
    break;
    case OP_JNE:
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
    writeCode(fp, temp->data.value.quadruple, st);
    temp = temp->next;
  }
  writeend(fp);
}
