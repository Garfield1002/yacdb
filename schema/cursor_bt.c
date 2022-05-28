#include "cursor.h"
#include "cursor_bt.h"
#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"
#include <string.h>

// TODO: add support for 0 and 1
char compare_r(struct record *r1, struct record *r2)
{
    if (r1->type != r2->type && !(r1->type > LAST_RECORD_TYPE && r2->type > LAST_RECORD_TYPE))
    {
        return 0;
    }

    switch (r1->type)
    {
        char c1, c2;
        short s1, s2;
        int i1, i2;
        long l1, l2;
        float f1, f2;

    case RECORD_TYPE_CHAR:
        c1 = *(char *)(r1->data);
        c2 = *(char *)(r2->data);
        return (c1 > c2) - (c1 < c2);

    case RECORD_TYPE_LONG:
        l1 = *(long *)r1->data;
        l2 = *(long *)r2->data;
        return (l1 > l2) - (l1 < l2);

    case RECORD_TYPE_SHORT:
        s1 = *(short *)r1->data;
        s2 = *(short *)r2->data;
        return (s1 > s2) - (s1 < s2);

    case RECORD_TYPE_INT:
        i1 = *(int *)r1->data;
        i2 = *(int *)r2->data;
        return (i1 > i2) - (i1 < i2);

    case RECORD_TYPE_FLOAT:
        f1 = *(float *)r1->data;
        f2 = *(float *)r2->data;
        return (f1 > f2) - (f1 < f2);

    default:
        // the record is a string
        return strcmp((char *)r1->data, (char *)r2->data);
    }
}

/**
 * @brief find the correct leaf node for a given key
 */
Cursor *find_leaf_bt(Page root, struct record *key)
{
    Cursor *cursor = create_at_start_bt(root);
    while (cursor_next_bt(cursor) != 0)
    {
        if (key->key < cursor->node->keys[0])
        {
            break;
        }
    }
    return cursor;
}