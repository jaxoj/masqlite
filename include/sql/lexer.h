#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TokenType{
    TOKEN_CREATE,
    TOKEN_TABLE,
    TOKEN_DATABASE,
    TOKEN_IDENTIFIER,
    TOKEN_TYPE,
    TOKEN_NOT_NULL_CONSTRAINT,
    TOKEN_PRIMARY_KEY_CONSTRAINT,
    TOKEN_AUTO_INCREMENT_CONSTRAINT,
    TOKEN_END_STATEMENT,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef struct Token{
    TokenType type;
    char *value;
} Token;

typedef struct Statement{
    Token *tokens;
    size_t len;
} Statement;

typedef struct Lexer{
    const char *input;
    size_t pos;
    Statement *statements;
    size_t statements_len;
} Lexer;

Lexer *new_lexer(const char *input);

void add_statement(Lexer *lexer);

void push_token(Lexer *lexer, Token token);

Statement current_statement(Lexer *lexer);

Token last_token(Lexer *lexer);

void tokenize(Lexer *lexer);

void skip_whitespace(Lexer *lexer);

void match_statement(Lexer *lexer);

void match_create(Lexer *lexer);

void match_database(Lexer *lexer);

void match_table(Lexer *lexer);

void match_columns(Lexer *lexer);

void match_column_constraints(Lexer *lexer);

void match_constraints(Lexer *lexer);

void match_end_of_statement(Lexer *lexer);

bool match_keyword(Lexer *lexer, const char *keyword);

bool is_keyword(const char* str);

bool is_constraint_keyword(const char* str);

Token match_identifier(Lexer *lexer);

Token match_type(Lexer *lexer);

Token match_constraint(Lexer *lexer);

#endif // LEXER_H
