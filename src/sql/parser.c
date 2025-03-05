#include "sql/parser.h"
#include "sql/lexer.h"
#include "sql/keyword.h"
#include <stdlib.h>
#include <string.h>

ParserStatement *new_parser_statement(Statement statement) {
    ParserStatement *s = malloc(sizeof(ParserStatement));
    s->tokenized_statement = statement;
    s->pos = 0;
    return s;
}

Token current_parser_statement_token(ParserStatement *statement) {
    return statement->tokenized_statement.tokens[statement->pos];
}

Parser *new_parser(Statement *statements, size_t len) {
    Parser *parser = malloc(sizeof(Parser));
    parser->statements = statements;
    parser->statements_len = len;
    parser->ast = nullptr;
    parser->ast_len = 0;
    return parser;
}

void push_ast_node(Parser *parser, ASTNode node) {
    ASTNode *new = realloc(parser->ast, (parser->ast_len + 1) * sizeof(ASTNode));
    if (new == nullptr) {
        perror("AST List allocation failed");
        return;
    }
    parser->ast = new;
    parser->ast[parser->ast_len] = node;
    parser->ast_len++;
}

void parse(Parser *parser) {
    for (size_t i = 0; i < parser->statements_len; i++) {
        ParserStatement *statement = new_parser_statement(parser->statements[i]);
        ASTNode statement_ast = parse_statement(statement);
        push_ast_node(parser, statement_ast);
    }
}

ASTNode parse_statement(ParserStatement *statement) {
    Token current = current_parser_statement_token(statement);
    switch (current.type) {
        case TOKEN_CREATE:
            statement->pos++;
            return parse_create(statement);
        default:
            return (ASTNode) {.type = AST_UNKNOWN};
    }
}

ASTNode parse_create(ParserStatement *statement) {
    Token current = current_parser_statement_token(statement);
    if (current.type == TOKEN_DATABASE) {
        statement->pos++;
        return parse_database(statement);
    }
    if (current.type == TOKEN_TABLE) {
        statement->pos++;
        return parse_table(statement);
    }
    return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected keyword 'Database' or 'Table'."}}};
}

ASTNode parse_database(ParserStatement *statement) {
    Token current = current_parser_statement_token(statement);
    if (current.type == TOKEN_IDENTIFIER) {
        return (ASTNode) {AST_CREATE_DATABASE, (ASTData) {.create_database = (CreateDatabaseData) {current.value}}};
    }
    return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected a database identifier."}}};
}

ASTNode parse_table(ParserStatement *statement) {
    Token current = current_parser_statement_token(statement);
    if (current.type != TOKEN_IDENTIFIER) {
        return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected a table identifier."}}};
    }
    CreateTableData data = {.name = current.value, .columns = nullptr};
    statement->pos++;
    current = current_parser_statement_token(statement);

    Column *columns = nullptr;
    size_t num_columns = 0;

    // parse the columns
    while (current.type == TOKEN_START_COLUMN) {
        statement->pos++;
        current = current_parser_statement_token(statement);

        // parse the column name.
        Column column = {.name = current.value};
        statement->pos++;
        current = current_parser_statement_token(statement);

        // parse the column type.
        if (current.type != TOKEN_TYPE) {
            return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected a column type."}}};
        }
        column.type = current.value;
        statement->pos++;
        current = current_parser_statement_token(statement);

        // parse column level constraints
        while (current.type != TOKEN_END_COLUMN) {
            if (current.type == TOKEN_UNKNOWN) {
                return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected a column constraint."}}};
            }
            if (strcmp(current.value, NOT_NULL_KEYWORD) == 0) {
                column.not_null = 1;
            }
            if (strcmp(current.value, AUTO_INCREMENT_KEYWORD) == 0) {
                column.auto_increment = 1;
            }
            if (strcmp(current.value, PRIMARY_KEY_KEYWORD) == 0) {
                column.primary_key = 1;
            }
            statement->pos++;
            current = current_parser_statement_token(statement);
        }

        columns = realloc(columns, (num_columns + 1) * sizeof(Column));
        if (!columns) {
            return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Memory allocation failed for columns."}}};
        }
        columns[num_columns++] = column;

        statement->pos++;
        current = current_parser_statement_token(statement);
    }

    // parse table level constraints
    while (current.type == TOKEN_START_CONSTRAINT) {
        statement->pos++;
        current = current_parser_statement_token(statement);
        if (current.type == TOKEN_UNKNOWN) {
            return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected table column constraints."}}};
        }

        const char *constraint_type = current.value;
        statement->pos++;
        current = current_parser_statement_token(statement);

        if (current.type != TOKEN_IDENTIFIER) {
            return (ASTNode) {AST_UNKNOWN, (ASTData) {.error = (ErrorData) {"Expected constraint identifier."}}};
        }
        const char *column_name = current.value;

        // search for the column and add the constraints.
        for (size_t i = 0; i < num_columns; i++) {
            if (strcmp(column_name, columns[i].name) == 0) {
                if (strcmp(constraint_type, NOT_NULL_KEYWORD) == 0) {
                    columns[i].not_null = 1;
                }
                if (strcmp(constraint_type, AUTO_INCREMENT_KEYWORD) == 0) {
                    columns[i].auto_increment = 1;
                }
                if (strcmp(constraint_type, PRIMARY_KEY_KEYWORD) == 0) {
                    columns[i].primary_key = 1;
                }
            }
        }

        statement->pos += 2; // skip TOKEN_END_CONSTRAINT
    }

    data.columns = columns;
    data.num_columns = num_columns;
    return (ASTNode) {AST_CREATE_TABLE, (ASTData) {.create_table = data}};
}
