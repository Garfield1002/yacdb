#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"

typedef struct
{
    Page page;
    struct node *node;
    unsigned short cell;
} Cursor;

int insert(Page root, struct key_value *kv);