#include "cursor.h"
#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"

/**
 * @brief Create a cursor object on the leftmost leaf
 */
Cursor *create_at_start(Page root)
{
    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        printf("Err table_start: Failed to allocate buffer.\n");
        return NULL;
    }

    cursor->node = read_node(root);
    if (cursor->node == NULL)
    {
        printf("Err step_in: Failed to read node.\n");
        return NULL;
    }
    cursor->page = root;
    cursor->cell = 0;

    while (!is_leaf(cursor->node))
    {
        cursor->page = cursor->node->child_addrs[0];
        cursor->node = read_node(cursor->node->child_addrs[0]);
    }

    return cursor;
}

void step_in(Cursor *cursor)
{
    cursor->page = cursor->node->child_addrs[cursor->cell];
    cursor->node = read_node(cursor->node->child_addrs[cursor->cell]);
    if (cursor->node == NULL)
    {
        printf("Err step_in: Failed to read node.\n");
        return;
    }
    cursor->cell = 0;
}

/**
 * @brief Returns the correct leaf node for a given key.
 */
Cursor *find_leaf(Page root, Key key)
{
    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        printf("Err find: Failed to allocate buffer.\n");
        return NULL;
    }

    cursor->node = read_node(root);

    if (cursor->node == NULL)
    {
        printf("Err find: Failed to read node.\n");
        return NULL;
    }
    cursor->page = root;
    cursor->cell = 0;

    while (!is_leaf(cursor->node))
    {
        while (cursor->cell < cursor->node->nb_keys)
        {
            if (key < cursor->node->key_vals[cursor->cell]->key)
            {
                break;
            }

            cursor->cell++;
        }
        step_in(cursor);
    }

    return cursor;
}

int cursor_next(Cursor *cursor)
{
    cursor->cell++;

    if (cursor->cell < cursor->node->nb_keys)
    {
        return 0;
    }

    if (cursor->node->child_addrs[0] == (Page)-1)
    {
        return -1;
    }

    cursor->cell = 0;
    cursor->page = cursor->node->child_addrs[0];
    cursor->node = read_node(cursor->node->child_addrs[0]);

    return 0;
}

/**
 * @brief Inserts the value into the array at a given index.
 * @param size is the size of the array BEFORE the insertion
 */
void insert_in_arr_kv(struct key_value *key_vals[], size_t size, struct key_value *val, size_t offset)
{
    for (size_t i = size; i > offset; i--)
    {
        key_vals[i] = key_vals[i - 1];
    }

    key_vals[offset] = val;
}

void insert_in_arr_Page(Page pages[], size_t size, Page val, size_t offset)
{
    for (size_t i = size; i > offset; i--)
    {
        pages[i] = pages[i - 1];
    }

    pages[offset] = val;
}

/**
 * @brief Splits an internal node into two nodes with ORDER/2 keys each.
 */
int split_internal_node(struct node *node, Key *key, Page *page)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node == NULL)
    {
        printf("Err split_internal_node: Failed to allocate buffer.\n");
        return -1;
    }

    new_node->type = NODE_TYPE_TABLE_INTERIOR;

    *key = node->key_vals[ORDER / 2]->key;

    // Copy the keys original node to the new node and remove them from the original node.
    for (size_t i = ORDER / 2 + 1; i < ORDER; i++)
    {
        new_node->key_vals[i - ORDER / 2 - 1] = node->key_vals[i];
        node->key_vals[i] = NULL;
    }

    // Copy the child addresses original node to the new node and remove them from the original node.
    for (size_t i = ORDER / 2 + 1; i < ORDER + 1; i++)
    {
        new_node->child_addrs[i - ORDER / 2 - 1] = node->child_addrs[i];
        node->child_addrs[i] = 0;
    }

    // sets the number of keys in both nodes.
    node->nb_keys = ORDER / 2;
    new_node->nb_keys = ORDER - ORDER / 2 - 1;

    new_node->parent_addr = node->parent_addr;

    // write the new node to disk.
    *page = create_addr();
    if (*page == (Page)-1)
    {
        printf("Err split_internal_node: Failed to create a new page.\n");
        return -1;
    }
    return write_node(new_node, *page);
}

int split_leaf_node(struct node *node, Key *key, Page *page)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node == NULL)
    {
        printf("Err split_internal_node: Failed to allocate buffer.\n");
        return -1;
    }

    new_node->type = NODE_TYPE_TABLE_LEAF;
    new_node->child_addrs[0] = (Page)-1;

    *key = node->key_vals[ORDER / 2]->key;

    // Copy the keys original node to the new node and remove them from the original node.
    for (size_t i = ORDER / 2; i < ORDER; i++)
    {
        new_node->key_vals[i - ORDER / 2] = node->key_vals[i];
        node->key_vals[i] = NULL;
    }

    // // Copy the child addresses original node to the new node and remove them from the original node.
    // for (size_t i = ORDER / 2; i < ORDER + 1; i++)
    // {
    //     new_node->child_addrs[i - ORDER / 2] = node->child_addrs[i];
    //     node->child_addrs[i] = 0;
    // }

    // sets the number of keys in both nodes.
    node->nb_keys = ORDER / 2;
    new_node->nb_keys = ORDER - ORDER / 2;

    new_node->parent_addr = node->parent_addr;

    // write the new node to disk.
    *page = create_addr();
    if (*page == (Page)-1)
    {
        printf("Err split_internal_node: Failed to create a new page.\n");
        return -1;
    }

    node->child_addrs[0] = *page;

    if (write_node(new_node, *page) == -1)
    {
        printf("Err split_internal_node: Failed to write new node.\n");
        return -1;
    }

    return 0;
}

struct key_value *create_kv(Key key, void *value, size_t size)
{
    struct key_value *kv = (struct key_value *)malloc(sizeof(struct key_value));

    if (kv == NULL)
    {
        printf("Err create_kv: Failed to allocate buffer.\n");
        return NULL;
    }

    kv->key = key;
    kv->value = value;
    kv->size = size;
    kv->overflow = NULL;

    return kv;
}

int create_root(Cursor *cursor, Page sibling_page)
{
    struct node *node = cursor->node;

    if (node->parent_addr == (Page)-1)
    {
        // the page is the root, create a new root
        struct node *new_root = (struct node *)malloc(sizeof(struct node));

        if (new_root == NULL)
        {
            printf("Err insert_internal: Failed to allocate buffer.\n");
            return -1;
        }

        new_root->parent_addr = (Page)-1;
        new_root->type = NODE_TYPE_TABLE_INTERIOR;
        new_root->nb_keys = 0;

        Page new_page = create_addr();

        if (new_page == (Page)-1)
        {
            printf("Err insert_internal: Failed to create a new page.\n");
            return -1;
        }

        new_root->child_addrs[0] = new_page;

        if (write_node(new_root, cursor->page) == -1)
        {
            printf("Err insert_internal: Failed to write the new root.\n");
            return -1;
        };

        // update the cursor and move the page
        node->parent_addr = cursor->page;
        if (write_node(node, new_page) == -1)
        {
            printf("Err insert_internal: Failed to write the new root.\n");
            return -1;
        };

        // updated the parent of the sibling
        struct node *sibling = read_node(sibling_page);
        if (sibling == NULL)
        {
            printf("Err insert_internal: Failed to read the new page.\n");
            return -1;
        }
        sibling->parent_addr = cursor->page;
        if (write_node(sibling, sibling_page) == -1)
        {
            printf("Err insert_internal: Failed to write the new page.\n");
            return -1;
        }

        cursor->page = new_page;

        // update the children
        if (!is_leaf(node))
        {
            for (size_t i = 0; i < node->nb_keys + 1; i++)
            {
                struct node *child = read_node(node->child_addrs[i]);
                child->parent_addr = new_page;
                write_node(child, node->child_addrs[i]);
            }
        }
    }

    return 0;
}

/**
 * @brief Inserts a key as well as a pointer in an internal node.
 */
int insert_internal(Page addr, Key key, Page value)
{
    Cursor cursor;
    cursor.page = addr;
    cursor.node = read_node(addr);
    if (cursor.node == NULL)
    {
        printf("Err step_in: Failed to read node.\n");
        return -1;
    }

    // find the correct offset for the key
    size_t offset = 0;
    while (offset < cursor.node->nb_keys && key > cursor.node->key_vals[offset]->key)
    {
        offset++;
    }

    // insert the key and the pointer
    insert_in_arr_kv(cursor.node->key_vals, cursor.node->nb_keys, create_kv(key, NULL, 0), offset);
    insert_in_arr_Page(cursor.node->child_addrs, cursor.node->nb_keys + 1, value, offset + 1);

    // updates the number of keys
    cursor.node->nb_keys++;

    // is the node saturated?
    if (cursor.node->nb_keys == ORDER)
    {
        // split the node and insert the key in the parent node.
        Key new_key;
        Page new_page;
        if (split_internal_node(cursor.node, &new_key, &new_page) == -1)
        {
            printf("Err insert_internal: Failed to split node.\n");
            return -1;
        }

        if (cursor.node->parent_addr == (Page)-1)
        {
            create_root(&cursor, new_page);
        }

        // dump the cursor
        printf("new_key: %d\n", new_key);
        printf("addr: %d\n", addr);

        printf("cursor.page: %lu\n", cursor.page);
        printf("cursor.node parent: %lu\n", cursor.node->parent_addr);

        // insert the new key in the parent node.
        if (insert_internal(cursor.node->parent_addr, new_key, new_page) == -1)
        {
            printf("Err insert_internal: Failed to insert key in parent node.\n");
            return -1;
        }
    }
    return write_node(cursor.node, cursor.page);
}

/**
 * @brief Inserts a key value pair into the tree.
 */
int insert(Page root, struct key_value *kv)
{
    Cursor *cursor = find_leaf(root, kv->key);

    if (cursor == NULL)
    {
        printf("Err insert: Failed to allocate buffer.\n");
        return -1;
    }

    // find the correct offset for the key
    size_t offset = 0;

    while (offset < cursor->node->nb_keys && kv->key > cursor->node->key_vals[offset]->key)
    {
        offset++;
    }

    // insert the key
    insert_in_arr_kv(cursor->node->key_vals, cursor->node->nb_keys, kv, offset);

    // updates the number of keys
    cursor->node->nb_keys++;

    if (cursor->node->nb_keys == ORDER)
    {
        // split the node and insert the key in the parent node.
        Key new_key;
        Page new_page;
        if (split_leaf_node(cursor->node, &new_key, &new_page) == -1)
        {
            printf("Err insert: Failed to split node.\n");
            return -1;
        }

        if (cursor->node->parent_addr == (Page)-1)
        {
            create_root(cursor, new_page);
        }

        // insert the new key in the parent node.
        if (insert_internal(cursor->node->parent_addr, new_key, new_page) == -1)
        {
            printf("Err insert: Failed to insert key in parent node.\n");
            return -1;
        }
    }
    return write_node(cursor->node, cursor->page);
}
