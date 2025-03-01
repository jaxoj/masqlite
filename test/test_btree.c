#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "btree.h"

static void test_btree_insert_nonfull_leaf(void **state)
{
    (void)state;
    BTreeNode *node = new_node(1, 1);
    node->pairs[0] = (Pair){.key_type = INT, .key = (Key){.integer = 10}};
    btree_insert_nonfull(node, node->pairs[0]);

    assert_int_equal(node->num_pairs, 2);
    assert_int_equal(node->pairs[1].key.integer, 10);

    free_node(node);
}

static void test_btree_insert_nonfull_nonleaf(void **state)
{
    (void)state;
    BTreeNode *root = new_node(1, 0);
    root->pairs[0] = (Pair){.key_type = INT, .key = (Key){.integer = 20}};
    root->children[0] = new_node(1, 1);
    root->children[0]->pairs[0] = (Pair){.key_type = INT, .key = (Key){.integer = 10}};
    root->children[1] = new_node(2, 1);
    root->children[1]->pairs[0] = root->pairs[0];
    root->children[1]->pairs[1] = (Pair){.key_type = INT, .key = (Key){.integer = 30}};

    btree_insert_nonfull(root, (Pair){.key_type = INT, .value_type = STR, .key = (Key){.integer = 25}, .value = (Value){.column = "25"}});

    assert_int_equal(root->num_pairs, 1);
    assert_int_equal(root->pairs[0].key.integer, 20);
    assert_int_equal(root->children[0]->num_pairs, 1);
    assert_int_equal(root->children[0]->pairs[0].key.integer, 10);
    assert_int_equal(root->children[1]->num_pairs, 3);
    assert_int_equal(root->children[1]->pairs[0].key.integer, 20);
    assert_int_equal(root->children[1]->pairs[1].key.integer, 25);
    assert_int_equal(root->children[1]->pairs[2].key.integer, 30);

    free_node(root);
}

static void test_btree_split_child(void **state)
{
    (void)state;

    BTreeNode *parent = new_node(1, 0);
    parent->pairs[0] = (Pair){.key_type = INT, .key = {.integer = 10}};

    // Child that will be split
    parent->children[0] = new_node(3, 1);
    parent->children[0]->pairs[0] = (Pair){.key_type = INT, .key = {.integer = 5}};
    parent->children[0]->pairs[1] = (Pair){.key_type = INT, .key = {.integer = 8}};
    parent->children[0]->pairs[2] = (Pair){.key_type = INT, .key = {.integer = 9}};


    btree_split_child(parent, 0);

    // Parent should now have an extra key
    assert_int_equal(parent->num_pairs, 2);
    assert_int_equal(parent->pairs[0].key.integer, 8);
    assert_int_equal(parent->pairs[1].key.integer, 10);

    // Left child should have only one key
    assert_int_equal(parent->children[0]->num_pairs, 1);
    assert_int_equal(parent->children[0]->pairs[0].key.integer, 5);

    // Right child should have one key
    assert_int_equal(parent->children[1]->num_pairs, 2);
    assert_int_equal(parent->children[1]->pairs[0].key.integer, 8);
    assert_int_equal(parent->children[1]->pairs[1].key.integer, 9);

    free_node(parent);
}

static void test_insert_into_empty_tree(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, 1);

    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 10}, .value = (Value){}});
    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 20}, (Value){}});

    assert_int_equal(root->num_pairs, 2);
    assert_int_equal(root->pairs[0].key.integer, 10);
    assert_int_equal(root->pairs[1].key.integer, 20);

    // Free memory
    free_node(root);
}

static void test_insert_causing_split(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, 1); // Start with an empty tree

    // Insert keys that will cause a split
    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 10}, .value = (Value){}});
    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 20}, .value = (Value){}});
    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 5}, .value = (Value){}});
    btree_insert(&root, (Pair){.key_type = INT, .key = (Key){.integer = 30}, .value = (Value){}});

    // After splitting, the tree should be balanced correctly
    assert_int_equal(root->num_pairs, 1);
    assert_int_equal(root->pairs[0].key.integer, 10); // The middle key moves up

    // Left child should contain [5]
    assert_int_equal(root->children[0]->num_pairs, 1);
    assert_int_equal(root->children[0]->pairs[0].key.integer, 5);

    // Right child should contain [10, 20, 30]
    assert_int_equal(root->children[1]->num_pairs, 3);
    assert_int_equal(root->children[1]->pairs[0].key.integer, 10);
    assert_int_equal(root->children[1]->pairs[1].key.integer, 20);
    assert_int_equal(root->children[1]->pairs[2].key.integer, 30);

    free_node(root);
}

static void test_btree_search(void **state)
{
    (void)state;
    BTreeNode *root = new_node(0, 1);
    int keys[] = {10, 20, 25, 15, 6};
    
    for (int i = 0; i < sizeof(keys)/sizeof(int); i++)
    {
        Pair pair = {.key_type = INT, .key = {.integer = keys[i]}, .value_type = STR, .value = {.column = malloc(sizeof(char)*5)}};
        sprintf(pair.value.column, "%d", keys[i]); 
        btree_insert(&root, pair);
    }

    for (int i = 0; i < sizeof(keys)/sizeof(int); i++)
    {
        Pair pair = {.key_type = INT, .key = {.integer = keys[i]}};
        bool found = btree_search(root, &pair);
        assert_true(found);
        char expected_value[5];
        sprintf(expected_value, "%d", keys[i]);
        assert_string_equal(pair.value.column, expected_value);
    }

    free_node(root);
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
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}