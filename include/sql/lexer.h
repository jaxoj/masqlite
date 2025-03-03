#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOKEN_CREATE,
    TOKEN_TABLE,
    TOKEN_DATABASE,
    TOKEN_IDENTIFIER,
    TOKEN_TYPE,
    TOKEN_PRIMARY,
    TOKEN_END_STATEMENT,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef struct {
    Token *tokens;
    size_t len;
} Statement;

typedef struct {
    const char *input;
    size_t pos;
    Statement *statements;
    size_t statements_len;
} Lexer;

Lexer *new_lexer(const char *input);

void add_statement(Lexer *lexer);

void push_token(Lexer *lexer, Token token);

void tokenize(Lexer *lexer);

void skip_whitespace(Lexer *lexer);

void match_statement(Lexer *lexer);

void match_create(Lexer *lexer);

void match_database(Lexer *lexer);

void match_table(Lexer *lexer);

void match_columns(Lexer *lexer);

void match_end_of_statement(Lexer *lexer);

bool match_keyword(Lexer *lexer, const char *keyword);

Token match_identifier(Lexer *lexer);

Token match_type(Lexer *lexer);

#endif // LEXER_H
