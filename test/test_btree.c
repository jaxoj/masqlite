#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "btree.h"

static void test_btree_insert_nonfull_leaf(void **state)
{
    (void)state;
    BTreeNode *node = new_node(1, true);
    node->keys[0] = 5;
    btree_insert_nonfull(node, 10);

    assert_int_equal(node->num_keys, 2);
    assert_int_equal(node->keys[1], 10);

    free_node(node);
}

static void test_btree_insert_nonfull_nonleaf(void **state)
{
    (void)state;
    BTreeNode *root = new_node(1, false);
    root->keys[0] = 20;
    root->children[0] = new_node(1, true);
    root->children[0]->keys[0] = 10;
    root->children[1] = new_node(1, true);
    root->children[1]->keys[0] = 30;

    btree_insert_nonfull(root, 25);

    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 20);
    assert_int_equal(root->children[0]->num_keys, 1);
    assert_int_equal(root->children[0]->keys[0], 10);
    assert_int_equal(root->children[1]->num_keys, 2);
    assert_int_equal(root->children[1]->keys[0], 25);
    assert_int_equal(root->children[1]->keys[1], 30);

    free_node(root);
}

static void test_btree_split_child(void **state)
{
    (void)state;

    // Parent with 1 key (10), will split the child
    BTreeNode *parent = new_node(1, false);
    parent->keys[0] = 10;

    // Child that will be split
    parent->children[0] = new_node(3, true);
    parent->children[0]->keys[0] = 5;
    parent->children[0]->keys[1] = 8;
    parent->children[0]->keys[2] = 9;

    btree_split_child(parent, 0);

    // Parent should now have an extra key
    assert_int_equal(parent->num_keys, 2);
    assert_int_equal(parent->keys[0], 8);
    assert_int_equal(parent->keys[1], 10);

    // Left child should have only one key
    assert_int_equal(parent->children[0]->num_keys, 1);
    assert_int_equal(parent->children[0]->keys[0], 5);

    // Right child should have one key
    assert_int_equal(parent->children[1]->num_keys, 1);
    assert_int_equal(parent->children[1]->keys[0], 9);

    free_node(parent);
}

static void test_insert_into_empty_tree(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);

    btree_insert(&root, 10);
    btree_insert(&root, 20);

    assert_int_equal(root->num_keys, 2);
    assert_int_equal(root->keys[0], 10);
    assert_int_equal(root->keys[1], 20);

    // Free memory
    free_node(root);
}

static void test_insert_causing_split(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true); // Start with an empty tree

    // Insert keys that will cause a split
    btree_insert(&root, 10);
    btree_insert(&root, 20);
    btree_insert(&root, 5);
    btree_insert(&root, 30);
    btree_insert(&root, 25);

    // After splitting, the tree should be balanced correctly
    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 10); // The middle key moves up

    // Left child should contain [5]
    assert_int_equal(root->children[0]->num_keys, 1);
    assert_int_equal(root->children[0]->keys[0], 5);

    // Right child should contain [25, 30]
    assert_int_equal(root->children[1]->num_keys, 3);
    assert_int_equal(root->children[1]->keys[0], 20);
    assert_int_equal(root->children[1]->keys[1], 25);
    assert_int_equal(root->children[1]->keys[2], 30);

    free_node(root);
}

static void test_btree_search(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);

    btree_insert(&root, 10);
    btree_insert(&root, 20);
    btree_insert(&root, 25);
    btree_insert(&root, 15);
    btree_insert(&root, 6);

    assert_true(btree_search(root, 10));
    assert_false(btree_search(root, 5));
    assert_false(btree_search(root, 16));
    assert_true(btree_search(root, 15));
    assert_true(btree_search(root, 6));

    free_node(root);
}

static void setup_btree_for_delete(BTreeNode **root)
{
    //      10, 20
    // 5,6,7    12,17   30
    btree_insert(root, 10);
    btree_insert(root, 20);
    btree_insert(root, 5);
    btree_insert(root, 6);
    btree_insert(root, 12);
    btree_insert(root, 30);
    btree_insert(root, 7);
    btree_insert(root, 17);
}

static void test_btree_find_key_index(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_int_equal(btree_find_key_index(root, 20), 1);
    assert_int_equal(btree_find_key_index(root, 30), 2);
    assert_int_equal(btree_find_key_index(root, 10), 0);
    assert_int_equal(btree_find_key_index(root, 7), 0);

    free_node(root);
}

static void test_btree_find_predecessor(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_int_equal(btree_find_predecessor(root, 0), 7);
    assert_int_equal(btree_find_predecessor(root, 1), 17);

    free_node(root);
}

static void test_btree_find_successor(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_int_equal(btree_find_successor(root, 0), 12);
    assert_int_equal(btree_find_successor(root, 1), 30);
    free_node(root);
}

static void test_btree_merge_nodes(void **state)
{
    (void) state;
    BTreeNode *root = new_node(0, true);
    btree_insert(&root, 20);
    root->is_leaf = false;

    BTreeNode *child = new_node(0, true);
    btree_insert(&child, 10);

    BTreeNode *sibling = new_node(0, true);
    btree_insert(&sibling, 25);

    root->children[0] = child;
    root->children[1] = sibling;

    btree_merge_nodes(root, 0);

    assert_int_equal(child->num_keys, 3);
    assert_int_equal(child->keys[0], 10);
    assert_int_equal(child->keys[1], 20);
    assert_int_equal(child->keys[2], 25);

    free_node(root);
}

static void test_btree_borrow_from_prev(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    btree_insert(&root, 20);
    root->is_leaf = false;

    BTreeNode *child = new_node(0, true);
    BTreeNode *sibling = new_node(2, true);
    sibling->keys[0] = 15;
    sibling->keys[1] = 17;

    root->children[0] = sibling;
    root->children[1] = child;

    btree_borrow_from_prev(root, 1);

    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 17);
    assert_int_equal(child->num_keys, 1);
    assert_int_equal(child->keys[0], 20);
    assert_int_equal(sibling->num_keys, 1);
    assert_int_equal(sibling->keys[0], 15);

    free_node(root);
}

static void test_btree_borrow_from_next(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    btree_insert(&root, 10);
    root->is_leaf = false;

    BTreeNode *child = new_node(0, true);
    BTreeNode *sibling = new_node(2, true);
    sibling->keys[0] = 15;
    sibling->keys[1] = 20;

    root->children[0] = child;
    root->children[1] = sibling;

    btree_borrow_from_next(root, 0);

    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 15);
    assert_int_equal(child->num_keys, 1);
    assert_int_equal(child->keys[0], 10);
    assert_int_equal(sibling->num_keys, 1);
    assert_int_equal(sibling->keys[0], 20);
    
    free_node(root);
}

static void test_btree_fill(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, true);
    btree_insert(&root, 20);
    root->is_leaf = false;

    BTreeNode *child = new_node(0, true);
    BTreeNode *sibling = new_node(2, true);
    sibling->keys[0] = 25;
    sibling->keys[1] = 30;

    root->children[0] = child; 
    root->children[1] = sibling;

    btree_fill(root, 0);

    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 25);
    assert_int_equal(child->num_keys, 1);
    assert_int_equal(child->keys[0], 20);
    assert_int_equal(sibling->num_keys, 1);
    assert_int_equal(sibling->keys[0], 30);

    free_node(root);
}

static void test_btree_delete_from_leaf(void **state)
{
    (void)state;

    BTreeNode *root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_true(btree_search(root, 6));
    btree_delete(&root->children[0], 6);
    assert_false(btree_search(root, 6));
    assert_int_equal(root->children[0]->num_keys, 2);

    free_node(root);
}

static void test_btree_delete_from_non_leaf(void **state)
{
    (void)state;

    BTreeNode *root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_true(btree_search(root, 10));
    btree_delete(&root, 10);
    assert_false(btree_search(root, 10));
    assert_int_equal(root->num_keys, 2);

    free_node(root);
}

static void test_btree_delete_internal_node(void **state)
{
    (void) state;
    BTreeNode* root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_true(btree_search(root, 10));
    assert_int_equal(btree_delete(&root, 10), 0);
    assert_false(btree_search(root, 10));

    free_node(root);
}

static void test_btree_delete_non_existent(void **state)
{
    (void) state;
    BTreeNode* root = new_node(0, true);
    setup_btree_for_delete(&root);

    assert_int_equal(btree_delete(&root, 99), -1);

    free_node(root);
}

static void test_btree_delete_all_keys(void **state)
{
    (void) state;
    BTreeNode* root = new_node(0, true);
    setup_btree_for_delete(&root);

    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17};
    for (int i = 0; i < (int)(sizeof(keys)/sizeof(keys[0])); i++)
    {
        assert_int_equal(btree_delete(&root, keys[i]), 0);
    }

    assert_null(root);
    free(root);
}

int main(void)
{
    const struct CMUnitTest tests[] =
        {
            cmocka_unit_test(test_btree_insert_nonfull_leaf),
            cmocka_unit_test(test_btree_insert_nonfull_nonleaf),
            cmocka_unit_test(test_btree_split_child),
            cmocka_unit_test(test_insert_into_empty_tree),
            cmocka_unit_test(test_insert_causing_split),
            cmocka_unit_test(test_btree_search),
            cmocka_unit_test(test_btree_find_key_index),
            cmocka_unit_test(test_btree_find_predecessor),
            cmocka_unit_test(test_btree_find_successor),
            cmocka_unit_test(test_btree_merge_nodes),
            cmocka_unit_test(test_btree_borrow_from_prev),
            cmocka_unit_test(test_btree_borrow_from_next),
            cmocka_unit_test(test_btree_fill),
            cmocka_unit_test(test_btree_delete_from_leaf),
            cmocka_unit_test(test_btree_delete_from_non_leaf),
            cmocka_unit_test(test_btree_delete_internal_node),
            cmocka_unit_test(test_btree_delete_non_existent),
            cmocka_unit_test(test_btree_delete_all_keys),
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}