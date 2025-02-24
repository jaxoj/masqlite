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
    Instruction program = {.operation_code = OP_ADD, .operand1 = 1, .operand2 = 2};
    VM *vm = new_vm();

    vm_load_program(vm, &program, 1);

    free(vm);
}

static void test_vm_run_NOP(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_NOP,
        },
        {
            .operation_code = OP_HALT,
        },
    };

    vm_load_program(vm, program, 2);
    vm_run(vm);

    assert_int_equal(vm->ip, 1);

    free(vm);
}

static void test_run_LOAD(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_LOAD,
            .operand1 = 0,
            .operand2 = 555,
        },
        {.operation_code = OP_HALT}};

    vm_load_program(vm, program, 2);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 555);

    free(vm);
}

static void test_run_STORE(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_LOAD,
            .operand1 = 0,
            .operand2 = 222,
        },
        {
            .operation_code = OP_STORE,
            .operand1 = 0,
            .operand2 = 1,
        },
        {.operation_code = OP_HALT}};

    vm_load_program(vm, program, 3);
    vm_run(vm);

    assert_int_equal(vm->registers[1], 222);

    free(vm);
}

static void test_run_ADD(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_LOAD,
            .operand1 = 0,
            .operand2 = 222,
        },
        {
            .operation_code = OP_ADD,
            .operand1 = 1,
            .operand2 = 0,
            .operand3 = 3,
        },
        {
            .operation_code = OP_HALT,
        },
    };

    vm_load_program(vm, program, 3);
    vm_run(vm);

    assert_int_equal(vm->registers[1], 225);

    free(vm);
}

static void test_run_JMP(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_JMP,
            .operand1 = 2,
        },
        {
            .operation_code = OP_HALT,
        },
        {
            .operation_code = OP_LOAD,
            .operand1 = 0,
            .operand2 = 222,
        },
        {
            .operation_code = OP_HALT,
        },
    };

    vm_load_program(vm, program, 4);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 222);

    free(vm);
}

static void test_run_CALL_RET(void **state)
{
    (void)state;
    VM *vm = new_vm();
    Instruction program[] = {
        {
            .operation_code = OP_CALL,
            .operand1 = 2,
        },
        {
            .operation_code = OP_HALT,
        },
        {
            .operation_code = OP_LOAD,
            .operand1 = 1,
            .operand2 = 4,
        },
        {
            .operation_code = OP_MUL,
            .operand1 = 0,
            .operand2 = 1,
            .operand3 = 4,
        },
        {
            .operation_code = OP_RET,
        }
    };

    vm_load_program(vm, program, 5);
    vm_run(vm);

    assert_int_equal(vm->registers[0], 16);

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