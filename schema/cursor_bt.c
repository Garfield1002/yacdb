#include "record.h"
#include "cursor.h"
#include "cursor_bt.h"
#include "db_ops.h"
#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"
#include <string.h>

/**
 * @brief find the correct leaf node for a given key
 */
Cursor *find_bt(Page root, struct record *key, size_t n, Page table)
{
    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        printf("Err find_bt: Failed to allocate buffer.\n");
        return NULL;
    }

    cursor->page = root;
    cursor->cell = 0;
    cursor->node = read_node(root);

    if (cursor->node == NULL)
    {
        free(cursor);
        printf("Err find_bt: Failed to read node.\n");
        return NULL;
    }

    for (;;)
    {

        while (cursor->cell < cursor->node->nb_keys)
        {
            struct record *node_key = get_n_record(
                table,
                cursor->node->key_vals[cursor->cell]->key,
                n);

            int cmp = compare_r(key, node_key);

            free(node_key);

            if (cmp == 0)
            {
                // check if the key is in the leaf
                if (is_leaf(cursor->node))
                {
                    return cursor;
                }

                Cursor *child = find_bt(
                    cursor->node->child_addrs[cursor->cell],
                    key,
                    n,
                    table);
                if (child != NULL)
                {
                    free(cursor);
                    return child;
                }
                return cursor;
            }

            if (cmp < 0)
                break;

            cursor->cell++;
        }

        if (is_leaf(cursor->node))
        {
            printf("Err find_bt: Key not found.\n");
            free(cursor);
            return NULL;
        }

        step_in(cursor);
    }

    return cursor;
}

Cursor *find_bt_leaf(Page root, Key key, size_t n, Page table)
{
    struct record *key_r = get_n_record(table, key, n);
    if (key_r == NULL)
    {
        printf("Err insert_bt: Failed to get record.\n");
        return NULL;
    }

    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        free(key_r);
        printf("Err find: Failed to allocate buffer.\n");
        return NULL;
    }

    cursor->node = read_node(root);

    if (cursor->node == NULL)
    {
        free(key_r);
        free(cursor);
        printf("Err find: Failed to read node.\n");
        return NULL;
    }
    cursor->page = root;
    cursor->cell = 0;

    while (!is_leaf(cursor->node))
    {
        while (cursor->cell < cursor->node->nb_keys)
        {
            struct record *node_key = get_n_record(
                table,
                cursor->node->key_vals[cursor->cell]->key,
                n);

            int cmp = compare_r(key_r, node_key);

            free(node_key);

            if (cmp < 0)
            {
                break;
            }

            cursor->cell++;
        }
        step_in(cursor);
    }

    return cursor;
}

int cursor_next_bt(Cursor *cursor)
{
    cursor->cell++;

    if (!is_leaf(cursor->node))
    {
        while (!is_leaf(cursor->node))
        {
            step_in(cursor);

            if (cursor == NULL)
            {
                printf("Err cursor_next_bt: Failed to step in.\n");
                return -1;
            }
        }

        return 0;
    }

    for (;;)
    {

        if (cursor->cell < cursor->node->nb_keys)
        {
            return 0;
        }

        Page old_page = cursor->page;
        cursor->page = cursor->node->parent_addr;

        if (cursor->page == (Page)-1)
        {
            return 1;
        }

        cursor->node = read_node(cursor->page);

        // find the right cell
        cursor->cell = 0;

        for (size_t i = 0; i < cursor->node->nb_keys + 1; i++)
        {
            if (cursor->node->child_addrs[i] == old_page)
            {
                cursor->cell = i;
                break;
            }
        }
    }

    return 0;
}

int find_all(Page root, struct record *key_r, size_t n, Page table, Key **keys, size_t *size)
{
    Cursor *cursor = find_bt(root, key_r, n, table);

    *size = 0;

    if (cursor == NULL)
    {
        printf("Err find_all: Failed to find leaf.\n");
        free(cursor);
        free(key_r);
        return 0;
    }

    *keys = (Key *)malloc(sizeof(Key));
    (*keys)[*size] = cursor->node->key_vals[cursor->cell]->key;
    (*size)++;

    while (cursor_next_bt(cursor) == 0)
    {
        struct record *node_key = get_n_record(
            table,
            cursor->node->key_vals[cursor->cell]->key,
            n);

        int cmp = compare_r(key_r, node_key);

        free(node_key);

        if (cmp != 0)
        {
            break;
        }

        *keys = (Key *)realloc(*keys, sizeof(Key) * (*size + 1));
        (*keys)[*size] = cursor->node->key_vals[cursor->cell]->key;
        (*size)++;
    }

    return 0;
}

/**
 * @brief Splits an internal node into two nodes with ORDER/2 keys each.
 *
 * TODO: refactor this function to as it is the same as split internal
 */
int split_bt_node(struct node *node, Key *key, Page *page)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node == NULL)
    {
        printf("Err split_bt_node: Failed to allocate buffer.\n");
        return -1;
    }

    new_node->type = node->type;

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
        printf("Err split_bt_node: Failed to create a new page.\n");
        return -1;
    }
    return write_node(new_node, *page);
}

int create_root_bt(Cursor *cursor, Page sibling_page)
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
        new_root->type = NODE_TYPE_INDEX_INTERIOR;
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

size_t find_offset(Cursor *cursor, Key key, size_t n, Page table)
{
    // find the correct offset for the key
    size_t offset = 0;

    struct record *key_r = get_n_record(table, key, n);
    if (key_r == NULL)
    {
        printf("Err insert_bt: Failed to get record.\n");
        return -1;
    }

    while (offset < cursor->node->nb_keys)
    {
        struct record *node_r = get_n_record(table, cursor->node->key_vals[offset]->key, n);
        if (node_r == NULL)
        {
            free(key_r);
            printf("Err insert_bt: Failed to get record.\n");
            return -1;
        }
        int cmp = compare_r(key_r, node_r);
        free(node_r);

        if (cmp <= 0)
            break;

        offset++;
    }

    return offset;
}

/**
 * @brief Inserts a key as well as a pointer in an internal node.
 */
int insert_internal_bt(Page addr, Key key, Page value, size_t n, Page table)
{
    Cursor cursor;
    cursor.page = addr;
    cursor.node = read_node(addr);
    if (cursor.node == NULL)
    {
        printf("Err insert_bt: Failed to read node.\n");
        return -1;
    }

    // find the correct offset for the key
    size_t offset = find_offset(&cursor, key, n, table);

    // insert the key and the pointer
    insert_in_arr_kv(
        cursor.node->key_vals,
        cursor.node->nb_keys,
        create_kv(key, NULL, 0),
        offset);

    insert_in_arr_Page(cursor.node->child_addrs, cursor.node->nb_keys + 1, value, offset + 1);

    // updates the number of keys
    cursor.node->nb_keys++;

    // is the node saturated?
    if (cursor.node->nb_keys == ORDER)
    {

        // split the node and insert the key in the parent node.
        Key new_key;
        Page new_page;
        if (split_bt_node(cursor.node, &new_key, &new_page) == -1)
        {
            printf("Err insert_bt: Failed to split node.\n");
            return -1;
        }

        if (cursor.node->parent_addr == (Page)-1)
        {
            create_root_bt(&cursor, new_page);
        }

        // insert the new key in the parent node.
        if (insert_internal_bt(cursor.node->parent_addr, new_key, new_page, n, table) == -1)
        {
            printf("Err insert_bt: Failed to insert key in parent node.\n");
            return -1;
        }
    }
    return write_node(cursor.node, cursor.page);
}

/**
 * @brief Inserts a key value pair into the tree.
 */
int insert_bt(Page root, Key key, size_t n, Page table)
{
    Cursor *cursor = find_bt_leaf(root, key, n, table);

    if (cursor == NULL)
    {
        printf("Err insert: Failed to allocate buffer.\n");
        return -1;
    }

    // find the correct offset for the key
    size_t offset = find_offset(cursor, key, n, table);

    // insert the key
    insert_in_arr_kv(
        cursor->node->key_vals,
        cursor->node->nb_keys,
        create_kv(key, NULL, 0),
        offset);

    // updates the number of keys
    cursor->node->nb_keys++;

    if (cursor->node->nb_keys == ORDER)
    {
        // split the node and insert the key in the parent node.
        Key new_key;
        Page new_page;
        if (split_bt_node(cursor->node, &new_key, &new_page) == -1)
        {
            printf("Err insert: Failed to split node.\n");
            return -1;
        }

        if (cursor->node->parent_addr == (Page)-1)
        {
            create_root_bt(cursor, new_page);
        }

        // insert the new key in the parent node.
        if (insert_internal_bt(cursor->node->parent_addr, new_key, new_page, n, table) == -1)
        {
            printf("Err insert: Failed to insert key in parent node.\n");
            return -1;
        }
    }
    return write_node(cursor->node, cursor->page);
}
