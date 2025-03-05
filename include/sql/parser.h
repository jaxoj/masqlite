#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ParserStatement {
    Statement tokenized_statement;
    size_t pos;
} ParserStatement;

typedef struct Parser {
    ASTNode *ast;
    size_t ast_len;
    Statement *statements;
    size_t statements_len;
} Parser;

ParserStatement *new_parser_statement(Statement statement);

Token current_parser_statement_token(ParserStatement *statement);

Parser *new_parser(Statement *statements, size_t len);

void push_ast_node(Parser *parser, ASTNode node);

void parse(Parser *parser);

ASTNode parse_statement(ParserStatement *statement);

ASTNode parse_create(ParserStatement *statement);

ASTNode parse_database(ParserStatement *statement);

ASTNode parse_table(ParserStatement *statement);

#endif // PARSER_H
