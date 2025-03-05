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
    TOKEN_START_COLUMN,
    TOKEN_END_COLUMN,
    TOKEN_START_CONSTRAINT,
    TOKEN_END_CONSTRAINT,
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

/**
 * Creates a new lexer instance for the given SQL input string.
 */
Lexer *new_lexer(const char *input);

/**
 * Adds a new SQL statement to the lexer's list of statements.
 */
void add_statement(Lexer *lexer);

/**
 * Pushes a token to the current statement in the lexer.
 */
void push_token(Lexer *lexer, Token token);

/**
 * Returns the current statement being processed by the lexer.
 */
Statement current_statement(Lexer *lexer);

/**
 * Returns the last token in the current statement.
 */
Token last_token(Lexer *lexer);

/**
 * Tokenizes the SQL input and populates the lexer's statements.
 */
void tokenize(Lexer *lexer);

/**
 * Skips whitespace characters in the input string.
 */
void skip_whitespace(Lexer *lexer);

/**
 * Matches the start of an SQL statement and determines its type.
 */
void match_statement(Lexer *lexer);

/**
 * Matches a CREATE statement and determines whether it's CREATE TABLE or CREATE DATABASE.
 */
void match_create(Lexer *lexer);

/**
 * Matches and processes a CREATE DATABASE statement.
 */
void match_database(Lexer *lexer);

/**
 * Matches and processes a CREATE TABLE statement.
 */
void match_table(Lexer *lexer);

/**
 * Matches and processes column definitions within a CREATE TABLE statement.
 */
void match_columns(Lexer *lexer);

/**
 * Matches and processes column-level constraints (e.g., NOT NULL, PRIMARY KEY, etc.).
 */
void match_column_constraints(Lexer *lexer);

/**
 * Matches and processes table-level constraints.
 */
void match_table_constraints(Lexer *lexer);

/**
 * Matches the end of an SQL statement (i.e., a semicolon).
 */
void match_end_of_statement(Lexer *lexer);

/**
 * Checks if the current input matches a specific SQL keyword.
 */
bool match_keyword(Lexer *lexer, const char *keyword);

/**
 * Determines if a given string is a recognized SQL keyword.
 */
bool is_keyword(const char* str);

/**
 * Determines if a given string is a recognized SQL constraint keyword.
 */
bool is_constraint_keyword(const char* str);

/**
 * Matches an identifier (such as a table or column name) in the SQL input.
 */
Token match_identifier(Lexer *lexer);

/**
 * Matches a data type (such as INT, VARCHAR, TEXT) in the SQL input.
 */
Token match_type(Lexer *lexer);

/**
 * Matches a constraint (such as NOT NULL, PRIMARY KEY, AUTO_INCREMENT) in the SQL input.
 */
Token match_constraint(Lexer *lexer);

#endif // LEXER_H
