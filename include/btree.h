#ifndef BTREE_H
#define BTREE_H
#include <stdbool.h>

#define BTREE_ORDER 4
#define MAX_PAIRS (BTREE_ORDER - 1)
#define MIN_PAIRS (MAX_PAIRS / 2)

typedef struct Pair Pair;
typedef struct BTreeNode BTreeNode;

typedef enum
{
    INT,
    STR,
    POINTER,
} PairType;

typedef union
{
    int integer;
    char *string;
} Key;

typedef union
{
    char *column;
    BTreeNode *node;
} Value;

struct Pair
{
    PairType key_type, value_type;
    Key key;
    Value value;
};

#define INT_KEY [](int key) { return (Key){.integer = key} };
#define STR_KEY [](char *key) { return (Key){.string = key}; };
#define KEY_OF(key) _Generic((key), int: INT_KEY, char *: STR_KEY)(key)
bool key_greater_than(PairType type, Key key, Key than);
bool key_less_than(PairType type, Key key, Key than);
bool key_equal_to(PairType type, Key key, Key to);

struct BTreeNode
{
    Pair pairs[MAX_PAIRS];
    BTreeNode *children[BTREE_ORDER];
    BTreeNode *next;
    int num_pairs;
    bool is_leaf;
};

// Allocate memory for a new `BTreeNode` and return its address.
BTreeNode *new_node(int num_pairs, bool is_leaf);

// Free node with its children
void free_node(BTreeNode *node);

// Split the child of the given node once it reaches the maximum number of pairs.
void btree_split_child(BTreeNode *node, int index);

// insert to a node that didn't reach the maximum number of pairs yet.
void btree_insert_nonfull(BTreeNode *node, Pair pair);

void btree_insert(BTreeNode **root, Pair pair);

// searches for the given `key` and return whether it exsits in the list or not.
bool btree_search(BTreeNode *root, Pair *pair);

#endif
