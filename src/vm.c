#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

VM *new_vm()
{
    VM *vm = malloc(sizeof(VM));

    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        vm->registers[i] = 0;
    }
    for (int i = 0; i < MAX_STACK_SIZE; i++)
    {
        vm->stack[i] = 0;
    }
    vm->sp = -1;
    vm->ip = 0;
    vm->halted = 0;
    return vm;
}

void vm_load_program(VM *vm, Instruction *program, int program_size)
{
    if (program_size > MAX_PROGRAM_SIZE)
    {
        fprintf(stderr, "Program size exceeded the maximum allowed size\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < program_size; i++)
    {
        vm->program[i] = program[i];
    }
    vm->ip = 0;
}

void vm_run(VM *vm)
{
    while (!vm->halted && vm->ip < MAX_PROGRAM_SIZE)
    {
        Instruction instruction = vm->program[vm->ip];
        switch (instruction.operation_code)
        {
        case OP_NOP:
            vm->ip++;
            break;
        case OP_HALT:
            vm->halted = 1;
            break;
        case OP_LOAD:
            vm->registers[instruction.operand1] = instruction.operand2;
            vm->ip++;
            break;
        case OP_STORE:
            // Assuming operand2 is the address in memory
            vm->registers[instruction.operand2] = vm->registers[instruction.operand1];
            vm->ip++;
            break;
        case OP_ADD:
            vm->registers[instruction.operand1] = vm->registers[instruction.operand2] + instruction.operand3;
            vm->ip++;
            break;
        case OP_SUB:
            vm->registers[instruction.operand1] = vm->registers[instruction.operand2] - instruction.operand3;
            vm->ip++;
            break;
        case OP_MUL:
            vm->registers[instruction.operand1] = vm->registers[instruction.operand2] * instruction.operand3;
            vm->ip++;
            break;
        case OP_DIV:
            vm->registers[instruction.operand1] = vm->registers[instruction.operand2] / instruction.operand3;
            vm->ip++;
            break;
        case OP_JMP:
            vm->ip = instruction.operand1;
            break;
        case OP_CALL:
            // Save the curret vm intraction in the stack
            vm->stack[++vm->sp] = vm->ip;
            // Jump to the function's entry point
            vm->ip = instruction.operand1;
            break;
        case OP_RET:
            // Return value expexted to be in regiter 0
            // Restore the instruction pointer from the stack
            if (vm->sp >= 0)
            {
                vm->ip = vm->stack[vm->sp--];
                vm->ip++;
            }
            else
            {
                fprintf(stderr, "Stack underflow on RET\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            break;
        }
    }
    if (!vm->halted && vm->ip == MAX_PROGRAM_SIZE)
    {
        fprintf(stderr, "VM exceeded the program size\n");
        exit(EXIT_FAILURE);
    }
}