#include "sql/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer *new_lexer(const char *input) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->pos = 0;
    lexer->statements = nullptr;
    lexer->statements_len = 0;
    return lexer;
}

void add_statement(Lexer *lexer) {
    Statement *new = realloc(lexer->statements, (lexer->statements_len + 1) * sizeof(Statement));
    if (new == nullptr) {
        perror("statements list reallocation failed");
        return;
    }
    lexer->statements = new;
    lexer->statements[lexer->statements_len].tokens = nullptr;
    lexer->statements[lexer->statements_len].len = 0;
    lexer->statements_len++;
}

void push_token(Lexer *lexer, Token token) {

    Statement *statement = &lexer->statements[lexer->statements_len - 1];
    Token *new = realloc(statement->tokens, (statement->len + 1) * sizeof(Token));
    if (new == nullptr) {
        perror("tokens list reallocation failed");
        return;
    }
    statement->tokens = new;
    statement->tokens[statement->len] = token;
    statement->len++;
}

void tokenize(Lexer *lexer) {
    while (lexer->input[lexer->pos] != '\0') {
        if (lexer->statements_len > 0) {
            Statement last = lexer->statements[lexer->statements_len - 1];
            if (last.tokens[last.len - 1].type == TOKEN_UNKNOWN) {
                return;
            }
        }

        skip_whitespace(lexer);
        add_statement(lexer);
        match_statement(lexer);
        skip_whitespace(lexer);
    }
    push_token(lexer, (Token) {TOKEN_EOF, nullptr});
}

void skip_whitespace(Lexer *lexer) {
    while (lexer->input[lexer->pos] == ' ' || lexer->input[lexer->pos] == '\t' || lexer->input[lexer->pos] == '\n') {
        lexer->pos++;
    }
}

bool match_keyword(Lexer *lexer, const char *keyword) {
    skip_whitespace(lexer);

    size_t len = strlen(keyword);
    if (strncmp(&lexer->input[lexer->pos], keyword, len) == 0 && !isalnum(lexer->input[lexer->pos + len])) {
        lexer->pos += len;
        return 1;
    }
    return 0;
}

void match_statement(Lexer *lexer) {
    if (match_keyword(lexer, "CREATE")) {
        push_token(lexer, (Token) {TOKEN_CREATE, "CREATE"});
        match_create(lexer);
        return;
    }
    push_token(lexer, (Token) {TOKEN_UNKNOWN, nullptr});
}

void match_create(Lexer *lexer) {
    if (match_keyword(lexer, "DATABASE")) {
        push_token(lexer, (Token) {TOKEN_DATABASE, "DATABASE"});
        match_database(lexer);
        return;
    }
    if (match_keyword(lexer, "TABLE")) {
        push_token(lexer, (Token) {TOKEN_TABLE, "TABLE"});
        match_table(lexer);
        return;
    }
    push_token(lexer, (Token) {TOKEN_UNKNOWN, nullptr});
}

void match_database(Lexer *lexer) {
    Token name = match_identifier(lexer);
    push_token(lexer, name);
    if (name.type == TOKEN_IDENTIFIER) {
        match_end_of_statement(lexer);
    }
}

void match_table(Lexer *lexer) {
    Token name = match_identifier(lexer);
    push_token(lexer, name);
    if (name.type == TOKEN_IDENTIFIER && lexer->input[lexer->pos] == '(') {
        lexer->pos++;
        match_columns(lexer);
    }
}

void match_columns(Lexer *lexer) {
    Token col = match_identifier(lexer);
    push_token(lexer, col);
    if (col.type != TOKEN_IDENTIFIER) {
        return;
    }

    Token col_type = match_type(lexer);
    push_token(lexer, col_type);
    if (col_type.type != TOKEN_TYPE) {
        return;
    }

    while (lexer->input[lexer->pos] == ',') {
        lexer->pos++;
        col = match_identifier(lexer);
        push_token(lexer, col);
        if (col.type != TOKEN_IDENTIFIER) {
            return;
        }

        col_type = match_type(lexer);
        push_token(lexer, col_type);
        if (col_type.type != TOKEN_TYPE) {
            return;
        }
    }

    if (lexer->input[lexer->pos] == ')') {
        lexer->pos++;
        match_end_of_statement(lexer);
        return;
    }
    push_token(lexer, (Token) {TOKEN_UNKNOWN, nullptr});
}

Token match_type(Lexer *lexer) {
    if (match_keyword(lexer, "INT")) {
        return (Token) {TOKEN_TYPE, "INT"};
    }
    if (match_keyword(lexer, "VARCHAR")) {
        return (Token) {TOKEN_TYPE, "VARCHAR"};
    }
    if (match_keyword(lexer, "TEXT")) {
        return (Token) {TOKEN_TYPE, "TEXT"};
    }
    return (Token) {TOKEN_UNKNOWN, nullptr};
}

Token match_identifier(Lexer *lexer) {
    skip_whitespace(lexer);

    size_t start = lexer->pos;
    while (isalnum(lexer->input[lexer->pos]) || lexer->input[lexer->pos] == '_') {
        lexer->pos++;
    }

    if (start == lexer->pos) {
        return (Token) {TOKEN_EOF, nullptr}; // no identifier
    }

    size_t len = lexer->pos - start;
    char *identifier = strndup(&lexer->input[start], len);
    return (Token) {TOKEN_IDENTIFIER, identifier};
}

void match_end_of_statement(Lexer *lexer) {
    skip_whitespace(lexer);
    if (lexer->input[lexer->pos] == ';') {
        push_token(lexer, (Token) {TOKEN_END_STATEMENT, ";"});
        lexer->pos++;
        return;
    }
    push_token(lexer, (Token) {TOKEN_UNKNOWN, nullptr});
}
