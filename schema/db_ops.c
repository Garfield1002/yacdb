#include "db_ops.h"
#include "record.h"
#include "../diskio/diskio.h"
#include "../diskio/diskiod.h"
#include "../diskio/dbfio.h"

/**
 * @brief Creates the TABLES table
 */

int init_TABLES_table()
{
    struct node table_root;

    table_root.type = NODE_TYPE_TABLE_LEAF;

    table_root.nb_keys = 2;

    table_root.key_vals[0] = (struct key_value *)malloc(sizeof(struct key_value));

    if (table_root.key_vals[0] == NULL)
    {
        printf("Err initialize_tables: Failed to allocate buffer\n");
        return -1;
    }

    table_root.key_vals[0]->key = 0;

    char *name = "TABLES";
    long root = 0, next_id = 2;

    struct record
        *r_name = record_from_string(&name),
        *r_root = record_from_long(&root),
        *r_next_id = record_from_long(&next_id);

    struct record entry[3] = {
        *r_name,
        *r_root,
        *r_next_id};

    compress_records(entry, 3, &table_root.key_vals[0]->size, &(table_root.key_vals[0]->value));

    table_root.key_vals[1] = (struct key_value *)malloc(sizeof(struct key_value));
    table_root.key_vals[1]->key = 1;

    name = "COLUMNS";
    root = 1;
    next_id = 3;

    r_name = record_from_string(&name);
    r_root = record_from_long(&root);
    r_next_id = record_from_long(&next_id);

    entry[0] = *r_name;
    entry[1] = *r_root;
    entry[2] = *r_next_id;

    compress_records(entry, 3, &table_root.key_vals[1]->size, &table_root.key_vals[1]->value);

    return write_node(&table_root, 0);
}

/**
 * @brief Creates the COLUMNS table
 */

int init_COLUMNS_table()
{
    struct node table_root;

    table_root.type = NODE_TYPE_TABLE_LEAF;

    table_root.nb_keys = 3;

    table_root.key_vals[0] = (struct key_value *)malloc(sizeof(struct key_value));

    if (table_root.key_vals[0] == NULL)
    {
        printf("Err initialize_tables: Failed to allocate buffer\n");
        return -1;
    }

    table_root.key_vals[0]->key = 0;
    //     `_idx`: the index of the column
    //  * - `table_idx`: the index of the table
    //  * - `name`: the name of the column
    //  * - `_type`: the type of the column
    //  * - `_offset`: the offset of the column in the table
    //  * - `_root`

    long table_idx = 0;
    char *name = "NAME";
    char _type = FIELD_TYPE_STRING;
    char _offset = 0;
    long _root = 2;

    struct record
        *r_table_idx = record_from_long(&table_idx),
        *r_name = record_from_string(&name),
        *r_type = record_from_char(&_type),
        *r_offset = record_from_char(&_offset),
        *r_root = record_from_long(&_root);

    struct record entry[5] = {
        *r_table_idx,
        *r_name,
        *r_type,
        *r_offset,
        *r_root};

    compress_records(entry, 5, &table_root.key_vals[0]->size, &table_root.key_vals[0]->value);

    table_root.key_vals[1] = (struct key_value *)malloc(sizeof(struct key_value));
    table_root.key_vals[1]->key = 1;

    table_idx = 1;
    name = "TABLE_IDX";
    _type = FIELD_TYPE_LONG;
    _offset = 0;
    _root = 3;

    r_table_idx = record_from_long(&table_idx),
    r_name = record_from_string(&name),
    r_type = record_from_char(&_type),
    r_offset = record_from_char(&_offset),
    r_root = record_from_long(&_root);

    entry[0] = *r_table_idx;
    entry[1] = *r_name;
    entry[2] = *r_type;
    entry[3] = *r_offset;
    entry[4] = *r_root;

    compress_records(entry, 5, &table_root.key_vals[1]->size, &table_root.key_vals[1]->value);

    table_root.key_vals[2] = (struct key_value *)malloc(sizeof(struct key_value));
    table_root.key_vals[2]->key = 2;

    table_idx = 1;
    name = "NAME";
    _type = FIELD_TYPE_STRING;
    _offset = 1;
    _root = 4;

    r_table_idx = record_from_long(&table_idx),
    r_name = record_from_string(&name),
    r_type = record_from_char(&_type),
    r_offset = record_from_char(&_offset),
    r_root = record_from_long(&_root);

    entry[0] = *r_table_idx;
    entry[1] = *r_name;
    entry[2] = *r_type;
    entry[3] = *r_offset;
    entry[4] = *r_root;

    compress_records(entry, 5, &table_root.key_vals[2]->size, &table_root.key_vals[2]->value);

    return write_node(&table_root, 1);
}

/**
 * @brief Create initial tables
 *
 * Creates the initial tables in the database. There are 2 tables:
 * - `TABLES`: a table of all tables in the database.
 * - `COLUMNS`: a table of all columns in the database.
 *
 * ## `TABLES` table
 *
 * It's root is on page 0, it has index 0.
 *
 * The table has the following columns:
 * - `_idx`: the index of the table
 * - `name`: the name of the table
 * - `_root`: the root page of the table
 * - `_next_id`: the next available id for a row in the table
 *
 * ## `COLUMNS` table
 *
 * It's root is on page 1, it has index 1.
 * It points to the index trees associated with each column.
 *
 * The table has the following columns:
 * - `_idx`: the index of the column
 * - `table_idx`: the index of the table
 * - `name`: the name of the column
 * - `_type`: the type of the column
 * - `_offset`: the offset of the column in the table
 * - `_root`: the root page of the index tree associated with the column
 */
int initialize_tables()
{
    init_db();

    // In total we need 5 pages
    for (size_t i = 0; i < 4; i++)
    {
        if (create_addr() == (Page)-1)
        {
            printf("Err initialize_tables: Failed to create address\n");
            return -1;
        }
    }

    return init_TABLES_table() || init_COLUMNS_table();
}
