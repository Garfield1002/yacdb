#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "../schema/record.h"
#include "dbfio.h"

#define ORDER 10

struct linked_overflow
{
    Page page;
    struct linked_overflow *next;
};

struct key_value
{
    int key;
    void *value;
    size_t size;

    struct linked_overflow *overflow;
};

struct node
{
    enum page_type type;
    size_t nb_keys;
    struct key_value *key_vals[ORDER - 1];
    size_t child_addrs[ORDER];
};

/**
 * @brief The database
 */
struct yacdb db;

/**
 * @brief Verifies if the node is a leaf
 */
bool is_leaf(struct node *node);

/**
 * @brief Reads a node from disk at a given address
 *
 * @param addr The page number of the node
 *
 * @return The node, will return NULL if an error occurred
 */
// TODO: Finish this
struct node *read_node(size_t addr);

/**
 * @brief Writes a node to disk (or cache) at a given address. If an error occurs returns -1
 */
// TODO: Finish this
int write_node(struct node *node, size_t addr);

/**
 * @brief Removes a node from disk at a given address. If an error occurs returns -1
 */
// TODO: Finish this
int remove_node(size_t addr);

/**
 * @brief Creates an address for a new node. If an error occurs returns -1
 */
Page create_addr();

/**
 * @brief initializes the database
 */
int init_db();
