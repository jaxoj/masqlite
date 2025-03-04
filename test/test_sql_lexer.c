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
                                     {TOKEN_END_STATEMENT, ";"},
    };
    Lexer *lexer = new_lexer("CREATE DATABASE users;\nCREATE users;");

    tokenize(lexer);

    Statement s1 = lexer->statements[0];
    assert_int_equal((int) s1.len, (int) (sizeof(expected_tokens) / sizeof(Token)));

    for (size_t i = 0; i < s1.len; i++) {
        assert_int_equal(s1.tokens[i].type, expected_tokens[i].type);
        assert_string_equal(s1.tokens[i].value, expected_tokens[i].value);
    }

    Statement s2 = lexer->statements[1];
    assert_int_equal((int) s2.len, 2);
    assert_int_equal(s2.tokens[lexer->statements[1].len - 1].type, TOKEN_UNKNOWN);
}

static void test_create_table_statement(void **state) {
    (void) state;
    const Token expected_tokens[] = {{TOKEN_CREATE,                    "CREATE"},
                                     {TOKEN_TABLE,                     "TABLE"},
                                     {TOKEN_IDENTIFIER,                "users"},
                                     {TOKEN_IDENTIFIER,                "id"},
                                     {TOKEN_TYPE,                      "INT"},
                                     {TOKEN_NOT_NULL_CONSTRAINT,       "NOT NULL"},
                                     {TOKEN_AUTO_INCREMENT_CONSTRAINT, "AUTO_INCREMENT"},
                                     {TOKEN_IDENTIFIER,                "name"},
                                     {TOKEN_TYPE,                      "VARCHAR"},
                                     {TOKEN_NOT_NULL_CONSTRAINT,       "NOT NULL"},
                                     {TOKEN_IDENTIFIER,                "age"},
                                     {TOKEN_TYPE,                      "INT"},
                                     {TOKEN_PRIMARY_KEY_CONSTRAINT,    "PRIMARY KEY"},
                                     {TOKEN_IDENTIFIER,                "id"},
                                     {TOKEN_END_STATEMENT,             ";"},
                                     {TOKEN_EOF,                       "\0"},
    };
    Lexer *lexer = new_lexer(
            "CREATE TABLE users (\nid INT NOT NULL AUTO_INCREMENT,\nname VARCHAR NOT NULL,\nage INT,\nPRIMARY KEY(id)\t\n);"
    );

    tokenize(lexer);

    Statement s = lexer->statements[0];
    assert_int_equal(s.len, (int) (sizeof(expected_tokens) / sizeof(Token)));

    for (size_t i = 0; i < s.len; i++) {
        assert_int_equal(s.tokens[i].type, expected_tokens[i].type);
        if (i < s.len - 1) {
            assert_string_equal(s.tokens[i].value, expected_tokens[i].value);
        }
    }
}
int main(void) {
    static const struct CMUnitTest tests[] =
            {
                    cmocka_unit_test(test_create_database_statement),
                    cmocka_unit_test(test_create_table_statement),
            };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
