#include "vm.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void test_vm_load_program(void **state)
{
    (void)state;
    Instruction program = {.opcode = OP_ADD, .opr1 = 1, .opr2 = 2};
    VM *vm = new_vm();

    vm_load_program(vm, &program, 1);

    free_node(vm->tree);
    free(vm);
}

static void test_vm_run_NOP(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_NOP,
        },
        {
            .opcode = OP_HALT,
        },
    };

    vm_load_program(vm, program, 2);
    vm_run(vm);

    assert_int_equal(vm->ip, 1);

    free_node(vm->tree);
    free(vm);
}

static void test_run_LOAD(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_LOAD,
            .opr1 = 0,
            .opr2 = 555,
        },
        {.opcode = OP_HALT}};

    vm_load_program(vm, program, 2);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 555);

    free_node(vm->tree);
    free(vm);
}

static void test_run_STORE(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_LOAD,
            .opr1 = 0,
            .opr2 = 222,
        },
        {
            .opcode = OP_STORE,
            .opr1 = 0,
            .opr2 = 1,
        },
        {.opcode = OP_HALT}};

    vm_load_program(vm, program, 3);
    vm_run(vm);

    assert_int_equal(vm->registers[1], 222);

    free_node(vm->tree);
    free(vm);
}

static void test_run_ADD(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_LOAD,
            .opr1 = 0,
            .opr2 = 222,
        },
        {
            .opcode = OP_ADD,
            .opr1 = 1,
            .opr2 = 0,
            .opr3 = 3,
        },
        {
            .opcode = OP_HALT,
        },
    };

    vm_load_program(vm, program, 3);
    vm_run(vm);

    assert_int_equal(vm->registers[1], 225);

    free_node(vm->tree);
    free(vm);
}

static void test_run_JMP(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_JMP,
            .opr1 = 2,
        },
        {
            .opcode = OP_HALT,
        },
        {
            .opcode = OP_LOAD,
            .opr1 = 0,
            .opr2 = 222,
        },
        {
            .opcode = OP_HALT,
        },
    };

    vm_load_program(vm, program, 4);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 222);

    free_node(vm->tree);
    free(vm);
}

static void test_run_CALL_RET(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .opcode = OP_CALL,
            .opr1 = 2,
        },
        {
            .opcode = OP_HALT,
        },
        {
            .opcode = OP_LOAD,
            .opr1 = 1,
            .opr2 = 4,
        },
        {
            .opcode = OP_MUL,
            .opr1 = 0,
            .opr2 = 1,
            .opr3 = 4,
        },
        {
            .opcode = OP_RET,
        }
    };

    vm_load_program(vm, program, 5);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 16);

    free_node(vm->tree);
    free(vm);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vm_load_program),
        cmocka_unit_test(test_vm_run_NOP),
        cmocka_unit_test(test_run_LOAD),
        cmocka_unit_test(test_run_STORE),
        cmocka_unit_test(test_run_ADD),
        cmocka_unit_test(test_run_JMP),
        cmocka_unit_test(test_run_CALL_RET),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}