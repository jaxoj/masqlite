#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

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
    int mid = (BTREE_ORDER - 1) / 2;

    BTreeNode *new_child = new_node(BTREE_ORDER / 2 - 1, child->is_leaf);

    // Move half of keys from child to new_child
    for (int j = 0; j < new_child->num_keys; j++)
    {
        new_child->keys[j] = child->keys[j + mid + 1];
    }

    // Move children if not a leaf
    if (!child->is_leaf)
    {
        for (int j = 0; j <= new_child->num_keys; j++)
        {
            new_child->children[j] = child->children[j + mid + 1];
        }
    }

    // Reduce number of keys in the original child
    child->num_keys = mid;

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
    parent->keys[i] = child->keys[mid]; // Move the median key up to the parent
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

bool btree_search(BTreeNode *root, int key)
{
    if (root == NULL)
    {
        return false;
    }
    
    int i = 0;
    while (i < root->num_keys && root->keys[i] < key)
    {
        i++;
    }

    if (i < root->num_keys && root->keys[i] == key)
    {
        return true;
    }

    if (root->is_leaf)
    {
        return false;
    }

    return btree_search(root->children[i], key);
}

int btree_find_key_index(BTreeNode *node, int key)
{
    int index = 0;
    while (index < node->num_keys && node->keys[index] < key)
    {
        index++;
    }
    return index;
}

int btree_delete(BTreeNode **root, int key)
{
    BTreeNode *ref = *root;
    if (root == NULL || !btree_search(ref, key))
    {
        return -1;
    }

    btree_remove(*root, key);

    if (ref->num_keys == 0)
    {
        BTreeNode *tmp = *root;
        if (!ref->is_leaf)
        {
            *root = ref->children[0];
        }
        else
        {
            *root = NULL;
        }
        free(tmp);
    }

    return 0;
}

void btree_remove(BTreeNode *node, int key)
{
    int index = btree_find_key_index(node, key);

    if (index < node->num_keys && key == node->keys[index])
    {
        if (node->is_leaf)
        {
            btree_remove_from_leaf(node, index);
        }
        else
        {
            btree_remove_from_non_leaf(node, index);
        }
    }
    else
    {
        if (node->is_leaf)
        {
            return;
        }

        // Check if the key is in the subtree rooted with the last child
        int flag = ((index == node->num_keys) ? 1 : 0);

        // If the child where the key is supposed to exist has less than the minimum keys, fill that child
        if (node->children[index]->num_keys < MIN_KEYS + 1)
        {
            btree_fill(node, index);
        }

        // If the last child has been merged, it must have merged with the previous child, so we recurse on the (index-1)th child
        if (flag && index > node->num_keys)
        {
            btree_remove(node->children[index - 1], key);
        }
        else
        {
            btree_remove(node->children[index], key);
        }
    }
}

void btree_remove_from_leaf(BTreeNode *node, int index)
{
    for (int i = index; i < node->num_keys - 1; i++)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->num_keys--;
}

void btree_remove_from_non_leaf(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index + 1];

    if (child->num_keys >= MIN_KEYS)
    {
        if (child->is_leaf)
        {
            node->keys[index] = child->keys[child->num_keys - 1];
            btree_delete(&child, node->keys[index]);
        }
        else
        {
            int pred = btree_find_predecessor(child, index);
            node->keys[index] = pred;
            btree_delete(&child, pred);
        }
    }
    else if (sibling->num_keys >= MIN_KEYS)
    {
        if (sibling->is_leaf)
        {
            node->keys[index] = sibling->keys[0];
            btree_delete(&sibling, node->keys[index]);
        }
        else
        {

            int succ = btree_find_successor(sibling, index);
            node->keys[index] = succ;
            btree_delete(&sibling, succ);
        }
    }
    else
    {
        btree_merge_nodes(node, index);
        btree_delete(&child, node->keys[index]);
    }
}

int btree_find_predecessor(BTreeNode *node, int index)
{
    BTreeNode *current = node->children[index];
    while (!current->is_leaf)
    {
        current = current->children[current->num_keys];
    }
    return current->keys[current->num_keys - 1];
}

int btree_find_successor(BTreeNode *node, int index)
{
    BTreeNode *current = node->children[index + 1];
    while (!current->is_leaf)
    {
        current = current->children[0];
    }
    return current->keys[0];
}

void btree_merge_nodes(BTreeNode *parent, int index)
{
    BTreeNode *child = parent->children[index];
    BTreeNode *sibling = parent->children[index + 1];

    // Move the key from parent to child
    child->keys[MIN_KEYS] = parent->keys[index];

    // Move keys from sibling to child
    for (int i = 0; i < sibling->num_keys; i++)
    {
        child->keys[MIN_KEYS + 1 + i] = sibling->keys[i];
    }

    // Move children from sibling to child
    if (!child->is_leaf)
    {
        for (int i = 0; i <= sibling->num_keys; i++)
        {
            child->children[MIN_KEYS + 1 + i] = sibling->children[i];
        }
    }

    // Update the number of keys in the child
    child->num_keys += sibling->num_keys + 1;

    // Move keys and children to the parent to fill the gap
    for (int i = index + 1; i < parent->num_keys; i++)
    {
        parent->keys[i - 1] = parent->keys[i];
    }
    for (int i = index + 2; i <= parent->num_keys; i++)
    {
        parent->children[i - 1] = parent->children[i];
    }

    parent->num_keys--;

    free(sibling);
}

void btree_fill(BTreeNode *node, int index)
{
    if (index != 0 && node->children[index - 1]->num_keys >= MIN_KEYS + 1)
    {
        btree_borrow_from_prev(node, index);
    }
    else if (index != node->num_keys && node->children[index + 1]->num_keys >= MIN_KEYS + 1)
    {
        btree_borrow_from_next(node, index);
    }
    else
    {
        if (index != node->num_keys)
        {
            btree_merge_nodes(node, index);
        }
        else
        {
            btree_merge_nodes(node, index - 1);
        }
    }
}

void btree_borrow_from_prev(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index - 1];

    // Move all keys and children in the child node one step forward
    for (int i = child->num_keys - 1; i >= 0; i--)
    {
        child->keys[i + 1] = child->keys[i];
    }
    if (!child->is_leaf)
    {
        for (int i = child->num_keys; i >= 0; i--)
        {
            child->children[i + 1] = child->children[i];
        }
    }

    // Move the key from the parent to the child
    child->keys[0] = node->keys[index - 1];

    if (!child->is_leaf)
    {
        child->children[0] = sibling->children[sibling->num_keys];
    }

    // Move the key from the sibling to the parent
    node->keys[index - 1] = sibling->keys[sibling->num_keys - 1];

    child->num_keys++;
    sibling->num_keys--;
}

void btree_borrow_from_next(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index + 1];

    // Move the key from the parent to the child
    child->keys[child->num_keys] = node->keys[index];

    if (!child->is_leaf)
    {
        child->children[child->num_keys + 1] = sibling->children[0];
    }

    // Move the first key from the sibling to the parent
    node->keys[index] = sibling->keys[0];

    // Move all keys and children in the sibling node one step backward
    for (int i = 1; i < sibling->num_keys; ++i)
    {
        sibling->keys[i - 1] = sibling->keys[i];
    }
    if (!sibling->is_leaf)
    {
        for (int i = 1; i <= sibling->num_keys; ++i)
        {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->num_keys++;
    sibling->num_keys--;
}