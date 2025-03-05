#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sql/parser.h"

static void test_create_database_statement(void **state) {
    (void) state;
    Token tokens[] = {{TOKEN_CREATE,        "CREATE"},
                      {TOKEN_DATABASE,      "DATABASE"},
                      {TOKEN_IDENTIFIER,    "users"},
                      {TOKEN_END_STATEMENT, ";"},
    };
    Statement statements[] = {(Statement) {&tokens[0], 4}};
    Parser *parser = new_parser(statements, 1);

    parse(parser);

    assert_int_equal(parser->ast_len, 1);
    assert_int_equal(parser->ast[0].type, AST_CREATE_DATABASE);
    assert_string_equal(parser->ast[0].data.create_database.name, "users");
}

static void test_create_database_table_statement(void **state) {
    (void) state;
    Token tokens[] = {{TOKEN_CREATE,                    "CREATE"},
                      {TOKEN_TABLE,                     "TABLE"},
                      {TOKEN_IDENTIFIER,                "users"},
                      {TOKEN_START_COLUMN,              nullptr},
                      {TOKEN_IDENTIFIER,                "id"},
                      {TOKEN_TYPE,                      "INT"},
                      {TOKEN_NOT_NULL_CONSTRAINT,       "NOT NULL"},
                      {TOKEN_AUTO_INCREMENT_CONSTRAINT, "AUTO_INCREMENT"},
                      {TOKEN_END_COLUMN,                nullptr},
                      {TOKEN_START_COLUMN,              nullptr},
                      {TOKEN_IDENTIFIER,                "name"},
                      {TOKEN_TYPE,                      "VARCHAR"},
                      {TOKEN_NOT_NULL_CONSTRAINT,       "NOT NULL"},
                      {TOKEN_END_COLUMN,                nullptr},
                      {TOKEN_START_COLUMN,              nullptr},
                      {TOKEN_IDENTIFIER,                "age"},
                      {TOKEN_TYPE,                      "INT"},
                      {TOKEN_END_COLUMN,                nullptr},
                      {TOKEN_START_CONSTRAINT,          nullptr},
                      {TOKEN_PRIMARY_KEY_CONSTRAINT,    "PRIMARY KEY"},
                      {TOKEN_IDENTIFIER,                "id"},
                      {TOKEN_END_CONSTRAINT,            nullptr},
                      {TOKEN_END_STATEMENT,             ";"},
    };
    Statement statements[] = {(Statement) {&tokens[0], 15}};
    Parser *parser = new_parser(statements, 1);

    parse(parser);

    ASTNode node = parser->ast[0];
    assert_int_equal(parser->ast_len, 1);
    assert_int_equal(node.type, AST_CREATE_TABLE);
    assert_string_equal(node.data.create_table.name, "users");
    assert_int_equal(node.data.create_table.num_columns, 3);
    assert_int_equal(node.data.create_table.num_columns, 3);
    assert_true(node.data.create_table.columns[0].auto_increment);
    assert_true(node.data.create_table.columns[0].primary_key);
    assert_true(node.data.create_table.columns[0].not_null);
    assert_true(node.data.create_table.columns[1].not_null);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_create_database_statement),
            cmocka_unit_test(test_create_database_table_statement)
    };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
