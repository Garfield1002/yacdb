/**
 * @file diskio.h
 * @author Jack Royer
 * @brief Interactions with the disk and cache.
 *
 * @date 2022-05-29
 */

#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "db_structs.h"
#include "../../schema/include/record.h"

// #define ORDER 4
#define ORDER 4

/**
 * @typedef Key
 * @brief A key to a record.
 *
 * This type is mainly used for auto-documentation, in order to better destinguish keys from addresses.
 */
typedef unsigned long Key;

struct linked_overflow
{
    Page page;
    struct linked_overflow *next;
};

struct key_value
{
    Key key;
    void *value;
    size_t size;

    // Private field please ignore
    struct linked_overflow *overflow;
};

struct node
{
    Page parent_addr;

    enum page_type type;
    size_t nb_keys;
    struct key_value *key_vals[ORDER + 1]; // +1 for when handling splits
    Page child_addrs[ORDER + 2];
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
 * @brief Reads a node from disk at a given address.
 *
 * @param addr The page number of the node
 * @return a pointer to the node.
 * @throw Returns NULL if an error occurred
 */
// TODO: Finish this
struct node *read_node(Page addr);

/**
 * @brief Writes a node to disk (or cache) at a given address. If an error occurs returns -1
 */
// TODO: Finish this
int write_node(struct node *node, Page addr);

/**
 * @brief Removes a node from disk at a given address. If an error occurs returns -1
 */
// TODO: Finish this
int remove_node(Page addr);

/**
 * @brief Creates an address for a new node. If an error occurs returns -1
 */
Page create_addr();

/**
 * @brief initializes the database
 */
int init_db();

struct node *create_node();
