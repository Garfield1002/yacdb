#pragma once

#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"

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