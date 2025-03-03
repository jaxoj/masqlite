#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sql/lexer.h"

static void test_create_database_statement(void **state) {
    (void) state;
    const Token expected_tokens[] = {{TOKEN_CREATE,        "CREATE"},
                                     {TOKEN_DATABASE,      "DATABASE"},
                                     {TOKEN_IDENTIFIER,    "users"},
                                     {TOKEN_END_STATEMENT, ";"}};
    Lexer *lexer = new_lexer("CREATE DATABASE users;\nCREATE users;");

    tokenize(lexer);

    Statement s1 = lexer->statements[0];
    assert_int_equal((int) s1.len, 4);

    for (size_t i = 0; i < 4; i++) {
        assert_int_equal(s1.tokens[i].type, expected_tokens[i].type);
        assert_string_equal(s1.tokens[i].value, expected_tokens[i].value);
    }

    Statement s2 = lexer->statements[1];
    assert_int_equal((int) s2.len, 2);
    assert_int_equal(s2.tokens[lexer->statements[1].len - 1].type, TOKEN_UNKNOWN);
}

int main(void) {
    static const struct CMUnitTest tests[] =
            {
                    cmocka_unit_test(test_create_database_statement),
            };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
