#pragma once

#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"
#include "cursor.h"

Cursor *create_at_start_bt(Page root);

int insert_bt(Page root, struct key_value *kv);

/**
 * @brief find the correct leaf node for a given key
 */
Cursor *find_leaf_bt(Page root, Key key);

int cursor_next_bt(Cursor *cursor);
