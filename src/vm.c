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
    vm->tree = new_node(0, 1);
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
        Instruction inst = vm->program[vm->ip];
        switch (inst.opcode)
        {
        /**
         * OPERATION: OP_NOP
         * just skips to the next instruction.
         * PARAMS:
         * REGISTERS:
         */
        case OP_NOP:
            vm->ip++;
            break;

        /**
         * OPERATION: OP_HALT
         * Halts the virtual machine.
         * PARAMS:
         * REGISTERS:
         */
        case OP_HALT:
            vm->halted = 1;
            break;

        /**
         * OPERATION: OP_LOAD
         * Loads a value into a register.
         * PARAMS: opr1 (register), opr2 (value)
         * REGISTERS: modifies opr1
         */
        case OP_LOAD:
            vm->registers[inst.opr1] = inst.opr2;
            vm->ip++;
            break;

        /**
         * OPERATION: OP_STORE
         * Stores the value of one register into another.
         * PARAMS: opr1 (source register), opr2 (destination register)
         * REGISTERS: modifies opr2
         */
        case OP_STORE:
            // Assuming operand2 is the address in memory
            vm->registers[inst.opr2] = vm->registers[inst.opr1];
            vm->ip++;
            break;

        /**
         * OPERATION: OP_ADD
         * Adds the value of two registers and stores the result in a register.
         * PARAMS: opr1 (destination register), opr2 (source register), opr3 (source register)
         * REGISTERS: modifies opr1
         */
        case OP_ADD:
            vm->registers[inst.opr1] = vm->registers[inst.opr2] + inst.opr3;
            vm->ip++;
            break;

        /**
         * OPERATION: OP_SUB
         * Subtracts the value of one register from another and stores the result in a register.
         * PARAMS: opr1 (destination register), opr2 (source register), opr3 (source register)
         * REGISTERS: modifies opr1
         */
        case OP_SUB:
            vm->registers[inst.opr1] = vm->registers[inst.opr2] - inst.opr3;
            vm->ip++;
            break;

        /**
         * OPERATION: OP_MUL
         * Multiplies the value of two registers and stores the result in a register.
         * PARAMS: opr1 (destination register), opr2 (source register), opr3 (source register)
         * REGISTERS: modifies opr1
         */
        case OP_MUL:
            vm->registers[inst.opr1] = vm->registers[inst.opr2] * inst.opr3;
            vm->ip++;
            break;

        /**
         * OPERATION: OP_DIV
         * Divides the value of one register by another and stores the result in a register.
         * PARAMS: opr1 (destination register), opr2 (source register), opr3 (source register)
         * REGISTERS: modifies opr1
         */
        case OP_DIV:
            vm->registers[inst.opr1] = vm->registers[inst.opr2] / inst.opr3;
            vm->ip++;
            break;

        /**
         * OPERATION: OP_JMP
         * Jumps to the specified instruction pointer.
         * PARAMS: opr1 (instruction pointer)
         * REGISTERS:
         */
        case OP_JMP:
            vm->ip = inst.opr1;
            break;

        /**
         * OPERATION: OP_JMP_IF_NOT_ZERO
         * Jumps to the specified instruction pointer if the value in a register is not zero.
         * PARAMS: opr1 (instruction pointer), opr2 (register)
         * REGISTERS:
         */
        case OP_JMP_IF_NOT_ZERO:
        {

            if (vm->registers[inst.opr2])
            {
                vm->ip = inst.opr1;
            }
            else
            {
                vm->ip++;
            }
            break;
        }

        /**
         * OPERATION: OP_JMP_IF_ZERO
         * Jumps to the specified instruction pointer if the value in a register is zero.
         * PARAMS: opr1 (instruction pointer), opr2 (register)
         * REGISTERS:
         */
        case OP_JMP_IF_ZERO:
        {

            if (!vm->registers[inst.opr2])
            {
                vm->ip = inst.opr1;
            }
            else
            {
                vm->ip++;
            }
            break;
        }

        /**
         * OPERATION: OP_CALL
         * Calls a function by jumping to its entry point and saving the current instruction pointer on the stack.
         * PARAMS: opr1 (function entry point)
         * REGISTERS:
         */
        case OP_CALL:
            // Save the curret vm intraction in the stack
            vm->stack[++vm->sp] = vm->ip;
            // Jump to the function's entry point
            vm->ip = inst.opr1;
            break;

        /**
         * OPERATION: OP_RET
         * Returns from a function by restoring the instruction pointer from the stack.
         * PARAMS:
         * REGISTERS:
         */
        case OP_RET:
            // Return value expexted to be in regiter 0
            // Restore the inst pointer from the stack
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