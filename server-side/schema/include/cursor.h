/**
 * @file cursor.h
 * @author Jack Royer
 * @brief Operations on b-plus-trees using a cursor.
 *
 * @date 2022-05-29
 */
#pragma once

#include "../../diskio/include/diskio.h"
#include "../../diskio/include/db_structs.h"

/**
 * @struct Cursor
 * @typedef Cursor
 * @brief A cursor for navigating a tree.
 */
typedef struct
{
    /**
     * @brief The page the cursor is on
     */
    Page page;

    /**
     * @brief The node associated with the page that the cursor is on
     *
     * At all time we have #node = read_node()(#page)
     */
    struct node *node;

    /**
     * @brief The current cell the cursor is on
     */
    unsigned short cell;
} Cursor;

/**
 * @brief Inserts a key value pair inside the tree. The key value will be
 * freed.
 */
int insert(Page root, struct key_value *kv);

/**
 * @brief find the correct leaf node for a given key
 */
Cursor *find_leaf(Page root, Key key);

int cursor_next(Cursor *cursor);

Cursor *create_at_start(Page root);

void step_in(Cursor *cursor);

struct key_value *create_kv(Key key, void *value, size_t size);

void insert_in_arr_Page(Page pages[], size_t size, Page val, size_t offset);

void insert_in_arr_kv(struct key_value *key_vals[], size_t size, struct key_value *val, size_t offset);

void free_cursor(Cursor *cursor);
