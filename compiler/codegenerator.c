#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tree.h"
#include"quadruple.h"
#include"list.h"
#include"symboltable.h"
#include"codegenerator.h"
#include"token.h"
#include"symbol.h"
#include"semantic.h"

void writebase(FILE* fp) {
  fprintf(fp, "%%include \'functions.asm\'\n");
  fprintf(fp, "section .bss\n");
  fprintf(fp, "temp1\tRESB\t20\n");
  fprintf(fp, "temp2\tRESB\t20\n");
  fprintf(fp, "tempmat1\tRESW\t100\n");
  fprintf(fp, "tempmat2\tRESW\t100\n");
  fprintf(fp, "stackbase RESB 1\n");
  fprintf(fp, "section .text\n");
  fprintf(fp, "global _start\n");
  fprintf(fp, "_start:\n");
  fprintf(fp, "mov ebp, stackbase\n");
}

void writeend(FILE* fp) {
  fprintf(fp, "call quit\n");
}

int getMatrixElement(Tree matrix, int i) {
  int column = i/(matrix->children->size);
  int row = i%(matrix->children->size);
  matrix = extractChildNumber(matrix, row+1);
  matrix = extractChildNumber(matrix, column+1);
  return getToken(extractSymbol(matrix))->value.integer;
}

int fetchSize(Address* addr) {
  switch(addr->type) {
    case 0:
      return ((struct symbolTableEntry*)addr->address.entry)->value.identry->size;
    case STRING:
      return ((String*)addr->address.entry)->size;
    case MATRIX:
      return ((Tree)addr->address.entry)->children->size * extractChildNumber(((Tree)addr->address.entry), 1)->children->size;
  }
}

void convertToRegister(FILE* fp, Address* addr, char reg[]) {
  struct symbolTableEntry* tempentree;
  char tempstring[20]; int i;
  static int stringtoggle = 0, matrixtoggle = 0; 
  Tree temptree;
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
    case STRING:
      if(!stringtoggle) {
        strcpy(tempstring, "temp1");
        stringtoggle++;
      } else {
        strcpy(tempstring, "temp2");
        stringtoggle--;
      }
      fprintf(fp, "mov %s, %s\n", reg, tempstring);
      for(i=0; i<((String*)addr->address.entry)->size-1;i++) {
        fprintf(fp, "mov dx, \'%c\'\n", ((String*)addr->address.entry)->value[i]);
        fprintf(fp, "mov [%s + %d], dx\n", reg, i);
      }
      fprintf(fp, "mov dx, 0Ah\n");
      fprintf(fp, "mov [%s + %d], dx\n", reg, i);
      break;
    case MATRIX:
      if(!matrixtoggle) {
        strcpy(tempstring, "tempmat1");
        matrixtoggle++;
      } else {
        strcpy(tempstring, "tempmat2");
        matrixtoggle--;
      }
      temptree = addr->address.entry;
      fprintf(fp, "mov %s, %s\n", reg, tempstring);
      for(i=0; i<temptree->children->size*extractChildNumber(temptree, 1)->children->size; i++) {
        fprintf(fp, "mov dx, %d\n", getMatrixElement(temptree, i));
        fprintf(fp, "mov [%s + %i], dx\n", reg, sizeLookup(INT)*i);
      }
      break;
  }
}

void convertToMemory(FILE* fp, Address* addr, char reg[]) {
  struct symbolTableEntry* tempentree;
  tempentree = addr->address.entry;
  if(tempentree->value.identry->type->type==INT || tempentree->value.identry->type->type==REAL)
    fprintf(fp, "mov [ebp + %d], %s\n", tempentree->value.identry->offset, reg);
}

void stringify(FILE* fp, Address* addr) {
  switch(((struct symbolTableEntry*) addr->address.entry)->value.identry->type->type) {
    case INT:
      fprintf(fp, "mov eax, 0\n");
      convertToRegister(fp, addr, "ax");
      fprintf(fp, "call iprintLF\n");
    break;
    case REAL:
    break;
    case STRING:
      convertToRegister(fp, addr, "ecx");
      fprintf(fp, "mov edx, %d\n", ((struct symbolTableEntry*) addr->address.entry)->value.identry->type->columns);
      fprintf(fp, "mov ebx, 1\n");
      fprintf(fp, "mov eax, 4\n");
      fprintf(fp, "int 80h\n");
    break;
    case MATRIX:
      convertToRegister(fp, addr, "eax");
      fprintf(fp, "mov edx, %d\n", ((struct symbolTableEntry*) addr->address.entry)->value.identry->size/sizeLookup(INT));
      fprintf(fp, "call mprintLF\n");
    break;
  }
}


void writeCode(FILE* fp, Quadruple* code, SymbolTable st) {
  Address *addr1, *addr2, *addr3;
  addr1 = code->op[0];
  addr2 = code->op[1];
  addr3 = code->op[2];
  switch(code->operator) {
    case OP_PLUS:
      switch(((struct symbolTableEntry*)addr3->address.entry)->value.identry->type->type) {
        case INT:
          convertToRegister(fp, addr1, "ax");
          convertToRegister(fp, addr2, "bx");
          fprintf(fp, "add ax, bx\n");
          convertToMemory(fp, addr3, "ax");
        break;
        case REAL:
        break;
        case STRING:
          convertToRegister(fp, addr1, "esi");
          convertToRegister(fp, addr3, "edi");
          fprintf(fp, "mov dl, %d\n", fetchSize(addr1));
          fprintf(fp, "call strcpy\n");
          fprintf(fp, "sub edi, 1\n");
          convertToRegister(fp, addr2, "esi");
          fprintf(fp, "mov dl, %d\n", fetchSize(addr2));
          fprintf(fp, "call strcpy\n");
        break;
        case MATRIX:
          convertToRegister(fp, addr1, "esi");
          convertToRegister(fp, addr2, "eax");
          convertToRegister(fp, addr3, "edi");
          fprintf(fp, "mov dl, %d\n", ((struct symbolTableEntry*) addr3->address.entry)->value.identry->size/sizeLookup(INT));
          fprintf(fp, "call matadd\n");
        break;
      }
    break;
    case OP_MINUS:
      switch(((struct symbolTableEntry*)addr3->address.entry)->value.identry->type->type) {
        case INT:
        convertToRegister(fp, addr1, "ax");
        convertToRegister(fp, addr2, "bx");
          fprintf(fp, "sub ax, bx\n");
          convertToMemory(fp, addr3, "ax");
        break;
        case REAL:
        break;
        case MATRIX:
        convertToRegister(fp, addr1, "esi");
        convertToRegister(fp, addr2, "eax");
        convertToRegister(fp, addr3, "edi");
        fprintf(fp, "mov dl, %d\n", ((struct symbolTableEntry*) addr3->address.entry)->value.identry->size/sizeLookup(INT));
        fprintf(fp, "call matsub\n");
        break;
      }
    break;
    case OP_DIV:
      switch(((struct symbolTableEntry*)addr3->address.entry)->value.identry->type->type) {
        case INT:
        convertToRegister(fp, addr1, "ax");
        convertToRegister(fp, addr2, "bx");
        fprintf(fp, "idiv bx\n");
        convertToMemory(fp, addr3, "ax");
        break;
        case REAL:
        break;
      }
    break;
    case OP_MUL:
      switch(((struct symbolTableEntry*)addr3->address.entry)->value.identry->type->type) {
        case INT:
          convertToRegister(fp, addr1, "ax");
          convertToRegister(fp, addr2, "bx");
          fprintf(fp, "imul bx\n");
          convertToMemory(fp, addr3, "ax");
        break;
        case REAL:
        break;
      }
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
      switch(((struct symbolTableEntry*)addr1->address.entry)->value.identry->type->type) {
        case INT:
          convertToRegister(fp, addr1, "ax");
          convertToRegister(fp, addr2, "bx");
          fprintf(fp, "cmp ax, bx\n");
        break;
        case REAL:
        break;
      }
    break;
    case OP_JMP:
      fprintf(fp, "jmp %s\n", ((char*) addr1->address.entry));
    break;
    case OP_LABEL:
      fprintf(fp, "%s:\n", ((char*) addr1->address.entry));
    break;
    case OP_MOV:
      switch(((struct symbolTableEntry*)addr1->address.entry)->value.identry->type->type) {
        case INT:
          convertToRegister(fp, addr2, "bx");
          convertToMemory(fp, addr1, "bx");
        break;
        case REAL:
        break;
        case MATRIX:
          convertToRegister(fp, addr1, "edi");
          convertToRegister(fp, addr2, "esi");
          fprintf(fp, "mov dl, %d\n", ((struct symbolTableEntry*)addr1->address.entry)->value.identry->size);
          fprintf(fp, "call strcpy\n");
        break;
        case STRING:
          convertToRegister(fp, addr1, "edi");
          convertToRegister(fp, addr2, "esi");
          fprintf(fp, "mov dl, %d\n", ((struct symbolTableEntry*)addr1->address.entry)->value.identry->size);
          fprintf(fp, "call strcpy\n");
        break;
      }
    break;
    case OP_PUSH:
      // fprintf(fp, "mov cx, %d\n", ((struct symbolTableEntry*) addr1->address.entry)->value.identry->size);
      // fprintf(fp, "call def\n");
    break;
    case OP_ADDRPLUS:
      fprintf(fp, "mov eax, 0\n");
      convertToRegister(fp, addr1, "ax");
      convertToRegister(fp, addr2, "ebx");
      fprintf(fp, "add ebx, eax\n");
      fprintf(fp, "mov ax, [ebx]\n");
      convertToMemory(fp, addr3, "ax");
    break;
    case OP_READ:
      fprintf(fp, "mov edx, %d\n", 10);
      fprintf(fp, "mov ecx, temp1\n");
      fprintf(fp, "mov ebx, 0\n");
      fprintf(fp, "mov eax, 3\n");
      fprintf(fp, "int 80h\n");
      switch (((struct symbolTableEntry*) addr1->address.entry)->value.identry->type->type) {
        case INT:
          fprintf(fp, "mov eax, ecx\n");
          fprintf(fp, "call atoi\n");
          convertToMemory(fp, addr1, "eax");
        break;
        case REAL:
        break;
      }
    break;
    case OP_PRINT:
      stringify(fp, addr1);
    break;
    default:
      fprintf(fp, ";Code Generation Skipped for operator %d\n", code->operator);
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
