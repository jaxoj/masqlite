#ifndef BTREE_H
#define BTREE_H
#include <stdbool.h>

#define BTREE_ORDER 4
#define MAX_KEYS BTREE_ORDER -1

typedef struct BTreeNode
{
    int *keys;
    struct BTreeNode **children;
    int num_keys;
    bool is_leaf;
} BTreeNode;

BTreeNode* new_node(int num_keys, bool is_leaf);
void free_node(BTreeNode* nodee);
void btree_split_child(BTreeNode *node, int index);
void btree_insert_nonfull(BTreeNode *node, int key);
void btree_insert(BTreeNode **root, int key);
int btree_search(BTreeNode *root);
void btree_delete(BTreeNode *root);

#endif