#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "storage_engine.h"

static void test_open_database(void **state)
{
    (void)state;
    assert_int_equal(open_database(), 0);
}

static void test_write_page(void **state)
{
    (void) state;
    Page page;
    const char str[] = "Hello, this is a simple string.";
    memcpy(page.data, str, strlen(str) + 1);

    assert_int_equal(write_page(1, &page), 0);
}

static void test_read_page(void **state) {
    (void) state;
    Page page;
    const char str[] = "Hello, this is a simple string.";

    assert_int_equal(read_page(1, &page), 0);
    assert_string_equal((char *)page.data, str);
}

static void test_close_database(void **state)
{
    (void)state;
    assert_int_equal(close_database(), 0);
    assert_int_equal(close_database(), -1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_open_database),
        cmocka_unit_test(test_write_page),
        cmocka_unit_test(test_read_page),
        cmocka_unit_test(test_close_database),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}