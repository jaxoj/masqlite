#ifndef VM_H
#define VM_H

#include<stdint.h>
#include<stdbool.h>

#define MAX_REGISTERS 256
#define MAX_STACK_SIZE 1024
#define MAX_PROGRAM_SIZE 1024

typedef enum OperationCode{
    OP_NOP,
    OP_HALT,
    OP_LOAD,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_JMP,
    OP_JMP_IF_ZERO,
    OP_JMP_IF_NOT_ZERO,
    OP_CALL,
    OP_RET,
    OP_PUSH,
    OP_POP,
    OP_PRINT,
    OP_SELECT,
    OP_INSERT,
    OP_UPDATE,
    OP_DELETE
} OperationCode;

typedef struct Instruction {
    OperationCode operation_code;
    int32_t operand1;
    int32_t operand2;
    int32_t operand3;
} Instruction;

typedef struct VM {
    int32_t registers[MAX_REGISTERS];
    int32_t stack[MAX_STACK_SIZE];
    int32_t sp; // stack pointer
    int32_t ip; // instruction pointer
    bool halted;
    Instruction program[MAX_PROGRAM_SIZE];
} VM;

VM *new_vm();
void vm_load_program(VM *vm, Instruction *program, int program_size);
void vm_run(VM *vm);

#endif // VM_H