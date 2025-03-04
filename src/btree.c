#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

bool key_greater_than(PairType type, Key key, Key than)
{
    if (type != INT && type != STR)
    {
        return 0;
    }
    if (type == STR)
    {
        return strcmp(key.string, than.string) > 0;
    }
    return key.integer > than.integer;
}
bool key_less_than(PairType type, Key key, Key than)
{
    if (type != INT && type != STR)
    {
        return 0;
    }
    if (type == STR)
    {
        return strcmp(key.string, than.string) < 0;
    }
    return key.integer < than.integer;
}
bool key_equal_to(PairType type, Key key, Key to)
{
    if (type != INT && type != STR)
    {
        return 0;
    }
    if (type == STR)
    {
        return strcmp(key.string, to.string) == 0;
    }
    return key.integer == to.integer;
}

BTreeNode *new_node(int num_pairs, bool is_leaf)
{
    BTreeNode *node = malloc(sizeof(BTreeNode));
    node->num_pairs = num_pairs;
    node->is_leaf = is_leaf;

    for (int i = 0; i < BTREE_ORDER; i++)
    {
        node->children[i] = nullptr;
    }

    return node;
}

void free_node(BTreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (!node->is_leaf)
    {
        for (int i = 0; i <= node->num_pairs; i++)
        {
            if (node->children[i] != NULL)
            {
                free_node(node->children[i]);
            }
        }
    }

    free(node);
}

void btree_split_child(BTreeNode *parent, int index)
{
    BTreeNode *child = parent->children[index];
    int mid = (MAX_PAIRS + 1) / 2;

    BTreeNode *new_child = new_node(mid, child->is_leaf);

    // Move half of keys from child to new_child
    for (int j = 0; j < new_child->num_pairs; j++)
    {
        new_child->pairs[j] = child->pairs[j + mid - 1];
    }

    // Move children if not a leaf
    if (!child->is_leaf)
    {
        for (int j = 0; j <= new_child->num_pairs; j++)
        {
            new_child->children[j] = child->children[j + mid - 1];
        }
    }
    else
    {
        child->next = new_child;
    }

    // Reduce number of keys in the original child
    child->num_pairs -= mid;

    // Shift parent's children to make room for new_child
    for (int j = parent->num_pairs; j >= index + 1; j--)
    {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = new_child;

    // Move parent's keys
    for (int j = parent->num_pairs - 1; j >= index; j--)
    {
        parent->pairs[j + 1] = parent->pairs[j];
    }
    parent->pairs[index].key_type = child->pairs[mid - 1].key_type; // Move the median key up to the parent
    parent->pairs[index].key = child->pairs[mid - 1].key;
    parent->num_pairs++;
}

void btree_insert_nonfull(BTreeNode *node, Pair pair)
{
    int index = node->num_pairs - 1;
    if (node->is_leaf)
    {
        while (index >= 0 && key_less_than(pair.key_type, pair.key, node->pairs[index].key))
        {
            node->pairs[index + 1] = node->pairs[index];
            index--;
        }
        node->pairs[index + 1] = pair;
        node->num_pairs++;
    }
    else
    {
        // Find the child to insert the key into
        while (index >= 0 && key_less_than(pair.key_type, pair.key, node->pairs[index].key))
        {
            index--;
        }
        index++;
        // if the child is full split it.
        if (node->children[index]->num_pairs == MAX_PAIRS)
        {
            btree_split_child(node, index);
            if (key_greater_than(pair.key_type, pair.key, node->pairs[index].key))
            {
                index++;
            }
        }
        btree_insert_nonfull(node->children[index], pair);
    }
}

void btree_insert(BTreeNode **root, Pair pair)
{
    if ((*root)->num_pairs == MAX_PAIRS)
    {
        BTreeNode *new_root = new_node(0, 0);
        new_root->children[0] = *root;
        btree_split_child(new_root, 0);
        *root = new_root;

        btree_insert_nonfull(*root, pair);
    }
    else
    {
        btree_insert_nonfull(*root, pair);
    }
}

bool btree_search(BTreeNode *root, Pair *pair)
{
    if (root == NULL)
    {
        return 0;
    }

    int i = 0;
    while (i < root->num_pairs && key_less_than(root->pairs[i].key_type, root->pairs[i].key, pair->key))
    {
        i++;
    }

    if (root->is_leaf)
    {
        if (i < root->num_pairs && key_equal_to(pair->key_type, pair->key, root->pairs[i].key))
        {
            pair->value_type = root->pairs[i].value_type;
            pair->value = root->pairs[i].value;
            return 1;
        }
        return 0;
    }

    if (i < root->num_pairs && key_equal_to(pair->key_type, pair->key, root->pairs[i].key))
    {
        return btree_search(root->children[i + 1], pair);
    }

    return btree_search(root->children[i], pair);
}
