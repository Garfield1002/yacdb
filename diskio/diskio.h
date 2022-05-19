#pragma once
#include <stdio.h>
#include "../schema/record.h"

#define ORDER 10

enum node_type
{
    NODE_TYPE_TABLE_LEAF,
    NODE_TYPE_TABLE_INTERIOR,
    NODE_TYPE_INDEX_LEAF,
    NODE_TYPE_INDEX_INTERIOR
};

struct key_value
{
    int key;
    struct record *values[];
};

struct node
{
    char type;
    int nb_keys;
    struct key_value key_val[ORDER - 1];
    size_t child_addrs[ORDER];
};

/**
 * @brief Verifies if the node is a leaf
 */
bool is_leaf(struct node *node);

/**
 * @brief Reads a node from disk at a given address
 */
struct node *read_node(size_t addr);

/**
 * @brief Writes a node to disk (or cache) at a given address. If an error occurs returns -1
 */
int write_node(struct node *node, size_t addr);

/**
 * @brief Removes a node from disk at a given address. If an error occurs returns -1
 */
int remove_node(size_t addr);

/**
 * @brief Creates an address for a new node. If an error occurs returns -1
 */
int create_addr();
