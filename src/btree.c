#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

BTreeNode *new_node(int num_keys, bool is_leaf)
{
    BTreeNode *node = malloc(sizeof(BTreeNode));
    node->num_keys = num_keys;
    node->is_leaf = is_leaf;

    node->keys = malloc((BTREE_ORDER - 1) * sizeof(int));
    node->children = malloc(BTREE_ORDER * sizeof(BTreeNode *));

    for (int i = 0; i < BTREE_ORDER; i++)
    {
        node->children[i] = NULL;
    }

    return node;
}

void free_node(BTreeNode *node)
{
    if (node == NULL)
    {
        return;
    }

    if (!node->is_leaf)
    {
        for (int i = 0; i <= node->num_keys; i++)
        {
            if (node->children[i] != NULL)
            {
                free_node(node->children[i]);
            }
        }
    }

    free(node->keys);
    free(node->children);
    free(node);
}

void btree_split_child(BTreeNode *parent, int i)
{
    BTreeNode *child = parent->children[i];
    BTreeNode *new_child = new_node((BTREE_ORDER - 1) / 2, child->is_leaf);

    // Move half of keys from child to new_child
    for (int j = 0; j < new_child->num_keys; j++)
    {
        new_child->keys[j] = child->keys[j + (BTREE_ORDER + 1) / 2];
    }

    // Move children if not a leaf
    if (!child->is_leaf)
    {
        for (int j = 0; j <= new_child->num_keys; j++)
        {
            new_child->children[j] = child->children[j + (BTREE_ORDER + 1) / 2];
        }
    }

    child->num_keys = (BTREE_ORDER - 1) / 2;

    // Shift parent's children to make room for new_child
    for (int j = parent->num_keys; j >= i + 1; j--)
    {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = new_child;

    // Move parent's keys
    for (int j = parent->num_keys - 1; j >= i; j--)
    {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[(BTREE_ORDER - 1) / 2];
    parent->num_keys++;
}

void btree_insert_nonfull(BTreeNode *node, int key)
{
    int index = node->num_keys - 1;
    if (node->is_leaf)
    {
        while (index >= 0 && key < node->keys[index])
        {
            node->keys[index + 1] = node->keys[index];
            index--;
        }
        node->keys[index + 1] = key;
        node->num_keys++;
    }
    else
    {
        // Find the child to insert the key into
        while (index >= 0 && key < node->keys[index])
        {
            index--;
        }
        index++;
        // if the child is full split it.
        if (node->children[index]->num_keys == MAX_KEYS)
        {
            btree_split_child(node, index);
            if (key > node->keys[index])
            {
                index++;
            }
        }
        btree_insert_nonfull(node->children[index], key);
    }
}

void btree_insert(BTreeNode **root, int key)
{
    if ((*root)->num_keys == MAX_KEYS)
    {
        BTreeNode *new_root = new_node(0, false);
        new_root->children[0] = *root;
        btree_split_child(new_root, 0);
        btree_insert_nonfull(new_root, key);
        *root = new_root;
    }
    else
    {
        btree_insert_nonfull(*root, key);
    }
}
