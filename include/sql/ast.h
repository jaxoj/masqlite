#ifndef AST_H
#define AST_H

typedef enum ASTNodeType {
    AST_CREATE_DATABASE,
    AST_CREATE_TABLE,
    AST_UNKNOWN
} ASTNodeType;

typedef struct Column {
    char *name;
    char *type;
    bool not_null;
    bool primary_key;
    bool auto_increment;
} Column;

typedef struct CreateDatabaseData {
    char *name;
} CreateDatabaseData;

typedef struct CreateTableData {
    char* name;
    Column *columns;
    size_t num_columns;
} CreateTableData;

typedef struct ErrorData {
    char *error;
} ErrorData;

typedef union ASTData {
    CreateDatabaseData create_database;
    CreateTableData create_table;
    ErrorData error;
} ASTData;

typedef struct ASTNode {
    ASTNodeType type;
    ASTData data;
} ASTNode;

#endif //AST_H
