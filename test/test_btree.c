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
    BTreeNode *parent = new_node(2, false);
    parent->keys[0] = 10;
    parent->keys[1] = 20;

    parent->children[1] = new_node(4, true);
    parent->children[1]->keys[0] = 12;
    parent->children[1]->keys[1] = 15;
    parent->children[1]->keys[2] = 17;

    btree_split_child(parent, 1);

    assert_int_equal(parent->num_keys, 3);
    assert_int_equal(parent->keys[1], 15);
    assert_int_equal(parent->children[1]->num_keys, 1);
    assert_int_equal(parent->children[1]->keys[0], 12);
    assert_int_equal(parent->children[2]->num_keys, 1);
    assert_int_equal(parent->children[2]->keys[0], 17);


    free_node(parent);
}

static void test_insert_into_empty_tree(void **state) {
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
    (void) state;
    BTreeNode *root = new_node(0, true);

    btree_insert(&root, 10);
    btree_insert(&root, 20);
    btree_insert(&root, 5);
    btree_insert(&root, 30);
    btree_insert(&root, 25);

    assert_int_equal(root->num_keys, 1);
    assert_int_equal(root->keys[0], 10);
    assert_int_equal(root->children[0]->num_keys, 1);
    assert_int_equal(root->children[0]->keys[0], 5);
    assert_int_equal(root->children[1]->num_keys, 3);
    assert_int_equal(root->children[1]->keys[0], 20);
    assert_int_equal(root->children[1]->keys[1], 25);
    assert_int_equal(root->children[1]->keys[2], 30);

    free_node(root);
}

static void test_btree_search(void **state)
{
    (void)state;
}

static void test_btree_delete(void **state)
{
    (void)state;
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
            cmocka_unit_test(test_btree_delete),
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}