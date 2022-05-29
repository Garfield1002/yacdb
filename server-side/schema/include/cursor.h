#pragma once

#include "../../diskio/include/diskio.h"
#include "../../diskio/include/db_structs.h"

typedef struct
{
    Page page;
    struct node *node;
    unsigned short cell;
} Cursor;

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
