#ifndef BTREE_H
#define BTREE_H
#include <stdbool.h>

#define BTREE_ORDER 4
#define MAX_KEYS (BTREE_ORDER - 1)
#define MIN_KEYS (MAX_KEYS / 2)

typedef struct BTreeNode
{
    int keys[MAX_KEYS];
    struct BTreeNode *children[BTREE_ORDER];
    int num_keys;
    bool is_leaf;
} BTreeNode;

// Allocate memory for a new `BTreeNode` and return its address.
BTreeNode *new_node(int num_keys, bool is_leaf);

// Free node with its children
void free_node(BTreeNode *nodee);

// Split the child of the given node once it reaches the maximum number of keys.
void btree_split_child(BTreeNode *node, int i);

// insert to a node which didn't reach the maximum number of keys yet.
void btree_insert_nonfull(BTreeNode *node, int key);

void btree_insert(BTreeNode **root, int key);

// searches for the given `key` and return whether it exsits in the list or not.
bool btree_search(BTreeNode *root, int key);

// return and integer index of the key in keys list.
// otherwise the returned interger will be either greater than the number of keys in the node,
// or the key of index will be greater than the given key.
int btree_find_key_index(BTreeNode *node, int key);

// it only removes the index key from the list of keys.
// because the node is supposed to be a lead there is no children to handle.
void btree_remove_from_leaf(BTreeNode *node, int index);

// finds the most right key in a tree rooted with the child of position `index` in `node`.
int btree_find_predecessor(BTreeNode *node, int index);

// finds the most left key in a tree rooted with the child of position `index` + 1 in `node`.
int btree_find_successor(BTreeNode *node, int index);

// merges the key of position `index` and keys of node of position `index` + 1 into child of position `index`
void btree_merge_nodes(BTreeNode *node, int index);

// unline the `btree_remove_from_leaf` this one has some children and borrowing handling.
void btree_remove_from_non_leaf(BTreeNode *node, int index);

void btree_borrow_from_prev(BTreeNode *node, int index);

void btree_borrow_from_next(BTreeNode *node, int index);

void btree_fill(BTreeNode *node, int index);

void btree_remove(BTreeNode *node, int key);

// return 0 if the give `key` is successfully deleted otherwise return -1.
int btree_delete(BTreeNode **root, int key);

#endif