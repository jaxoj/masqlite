#include "sql/lexer.h"
#include "sql/keyword.h"
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

Statement current_statement(Lexer *lexer){
    return lexer->statements[lexer->statements_len - 1];
}

Token last_token(Lexer *lexer) {
    Statement current = current_statement(lexer);
    return current.tokens[current.len - 1];
}

void tokenize(Lexer *lexer) {
    while (lexer->input[lexer->pos] != '\0') {
        if (lexer->statements_len > 0) {
            Token last = last_token(lexer);
            if (last.type == TOKEN_UNKNOWN) {
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

void match_statement(Lexer *lexer) {
    if (match_keyword(lexer, CREATE_KEYWORD)) {
        push_token(lexer, (Token) {TOKEN_CREATE, CREATE_KEYWORD});
        match_create(lexer);
        return;
    }
    push_token(lexer, (Token) {TOKEN_UNKNOWN, nullptr});
}

void match_create(Lexer *lexer) {
    if (match_keyword(lexer, DATABASE_KEYWORD)) {
        push_token(lexer, (Token) {TOKEN_DATABASE, DATABASE_KEYWORD});
        match_database(lexer);
        return;
    }
    if (match_keyword(lexer, TABLE_KEYWORD)) {
        push_token(lexer, (Token) {TOKEN_TABLE, TABLE_KEYWORD});
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
    skip_whitespace(lexer);

    if (name.type == TOKEN_IDENTIFIER && lexer->input[lexer->pos] == '(') {
        lexer->pos++;
        match_columns(lexer);
    }
}

void match_columns(Lexer *lexer) {
    do {
        // check if the identifier name isn't a keyword
        // if so check if it's a constraint
        // if so break out of the loop, if not return a syntax error.
        Token col = match_identifier(lexer);
        if (!is_keyword(col.value)) {
            push_token(lexer, col);
            if (col.type == TOKEN_UNKNOWN) {
            return;
            }

            Token col_type = match_type(lexer);
            push_token(lexer, col_type);
            if (col_type.type == TOKEN_UNKNOWN) {
                return;
            }

            match_column_constraints(lexer);
            Token col_constraint = last_token(lexer);
            if (col_constraint.type == TOKEN_UNKNOWN) {
                return;
            }
        } else if (!is_constraint_keyword(col.value)) {
            push_token(lexer, col);
            return;
        } else {
            lexer->pos -= strlen(PRIMARY_KEYWORD);
            break;
        }
    } while (lexer->input[lexer->pos] == ',' && lexer->pos++);

    while (lexer->input[lexer->pos] != ')') {
        match_constraints(lexer);
        Token constraint = last_token(lexer);
        if (constraint.type == TOKEN_UNKNOWN) {
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

void match_column_constraints(Lexer *lexer) {
    skip_whitespace(lexer);
    while (lexer->input[lexer->pos] != ',' && lexer->input[lexer->pos] != ')') {
        Token col_constraint = match_constraint(lexer);
        push_token(lexer, col_constraint);
        if (col_constraint.type == TOKEN_UNKNOWN) {
            return;
        }
    }
}

void match_constraints(Lexer *lexer) {
    skip_whitespace(lexer);
    while (lexer->input[lexer->pos] != ',' && lexer->input[lexer->pos] != ')') {
        Token constraint = match_constraint(lexer);
        push_token(lexer, constraint);
        if (constraint.type == TOKEN_UNKNOWN) {
            return;
        }
        if (lexer->input[lexer->pos] == '(') {
            lexer->pos++;
            Token constraint_col = match_identifier(lexer);
            push_token(lexer, constraint_col);
            if (constraint_col.type == TOKEN_UNKNOWN) {
                return;
            }
            if (lexer->input[lexer->pos] == ')') {
                lexer->pos++;
                skip_whitespace(lexer);
            }
        }
    }
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

bool match_keyword(Lexer *lexer, const char *keyword) {
    skip_whitespace(lexer);

    size_t len = strlen(keyword);
    if (strncmp(&lexer->input[lexer->pos], keyword, len) == 0 && !isalnum(lexer->input[lexer->pos + len])) {
        lexer->pos += len;
        return 1;
    }
    return 0;
}

bool is_keyword(const char *str) {
    const char *keywords[] = SQL_KEYWORDS;
    for (size_t i = 0; i < sizeof(keywords) / sizeof(char *); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

bool is_constraint_keyword(const char *str){
    const char *constraints_keyword[] = SQL_CONSTRAINTS;
    for (size_t i = 0; i < sizeof(constraints_keyword)/sizeof(char*); i++)
    {
        if (strcmp(str, constraints_keyword[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
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

Token match_type(Lexer *lexer) {
    if (match_keyword(lexer, INT_KEYWORD)) {
        return (Token) {TOKEN_TYPE, INT_KEYWORD};
    }
    if (match_keyword(lexer, VARCHAR_KEYWORD)) {
        return (Token) {TOKEN_TYPE, VARCHAR_KEYWORD};
    }
    if (match_keyword(lexer, TEXT_KEYWORD)) {
        return (Token) {TOKEN_TYPE, TEXT_KEYWORD};
    }
    return (Token) {TOKEN_UNKNOWN, nullptr};
}

Token match_constraint(Lexer *lexer) {
    if (match_keyword(lexer, NOT_NULL_KEYWORD)) {
        return (Token) {TOKEN_NOT_NULL_CONSTRAINT, NOT_NULL_KEYWORD};
    }
    if (match_keyword(lexer, PRIMARY_KEY_KEYWORD)) {
        return (Token) {TOKEN_PRIMARY_KEY_CONSTRAINT, PRIMARY_KEY_KEYWORD};
    }
    if (match_keyword(lexer, AUTO_INCREMENT_KEYWORD)) {
        return (Token) {TOKEN_AUTO_INCREMENT_CONSTRAINT, AUTO_INCREMENT_KEYWORD};
    }
    return (Token) {TOKEN_UNKNOWN, nullptr};
}
