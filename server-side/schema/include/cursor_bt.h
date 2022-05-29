#pragma once

#include "record.h"
#include "cursor.h"
#include "db_ops.h"
#include "../../diskio/include/diskio.h"
#include "../../diskio/include/db_structs.h"
#include <string.h>

int insert_bt(Page root, Key key, size_t n, Page table);

Cursor *find_bt(Page root, struct record *key, size_t n, Page table);

int cursor_next_bt(Cursor *cursor);

int find_all(Page root, struct record *key_r, size_t n, Page table, Key **keys, size_t *size);
