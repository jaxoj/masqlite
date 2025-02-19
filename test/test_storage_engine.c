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
    (void)state;
    Page page;
    strcpy((char *)page.data, "Hello, this is a simple string.");

    assert_int_equal(write_page(999, &page), 0);
}

static void test_read_page(void **state)
{
    (void)state;
    Page page;
    const char str[] = "Hello, this is a simple string.";

    assert_int_equal(read_page(999, &page), 0);
    assert_string_equal((char *)page.data, str);
}

static void test_allocate_page(void **state)
{
    (void)state;
    Page page, page_read;
    strcpy((char *)page.data, "My name is Monsef");
    int page_number = allocate_page(&page);
    assert_int_equal(page_number, 999);
    assert_int_equal(read_page(page_number, &page_read), 0);
    assert_string_equal((char *)page_read.data, (char *)page.data);
}

static void test_free_page(void **state)
{
    (void)state;

    assert_int_equal(free_page(999), 0);
    assert_int_equal(free_page(999), -1);
}

static void test_read_page_with_cache(void **state)
{
    (void) state;
    Page page;
    assert_int_equal(read_page_with_cache(999, &page), 0);
    assert_non_null(page.data);
    assert_string_equal((char *)page.data, "My name is Monsef");
}

static void test_write_page_with_cache(void **state)
{
    (void) state;
    Page page;
    strcpy((char *)page.data, "Hello everyone");

    assert_int_equal(write_page_with_cache(999, &page), 0);
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
        cmocka_unit_test(test_allocate_page),
        cmocka_unit_test(test_free_page),
        cmocka_unit_test(test_read_page_with_cache),
        cmocka_unit_test(test_write_page_with_cache),
        cmocka_unit_test(test_close_database),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}