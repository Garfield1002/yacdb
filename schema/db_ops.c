#include "db_ops.h"
#include "record.h"
#include "../diskio/diskio.h"
#include "../diskio/diskiod.h"
#include "../diskio/dbfio.h"
#include "cursor.h"
#include <string.h>

/**
 * @brief Creates the TABLES table
 */

int init_TABLES_table()
{
    Page root_page = TABLES;

    struct node *root_node = create_node();

    if (write_node(root_node, root_page) == -1)
    {
        printf("Error: Could not write node to disk.\n");
        free(root_node);
        return -1;
    }

    Key row_id = 0;

    struct key_value kv = {.key = row_id};

    row_id++;

    char *name = "TABLES";

    struct record *records[3] = {
        record_from_string(&name),
        record_from_long((long *)&root_page),
        record_from_long((long *)&row_id)};

    if (compress_records(records, 3, &kv.size, &kv.value) == -1)
    {
        for (size_t i = 0; i < 3; i++)
        {
            free(records[i]->data);
            free(records[i]);
        }
        printf("Err insert_row: Failed to compress records\n");
        return -1;
    }

    for (size_t i = 0; i < 3; i++)
    {
        free(records[i]->data);
        free(records[i]);
    }

    if (insert(root_page, &kv) == -1)
    {
        printf("Err insert_row: Failed to insert row\n");
        free(kv.value);
        return -1;
    }

    free(kv.value);

    return 0;
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

    struct record *entry[5] = {
        r_table_idx,
        r_name,
        r_type,
        r_offset,
        r_root};

    compress_records(entry, 5, &table_root.key_vals[0]->size, &table_root.key_vals[0]->value);

    free(r_table_idx->data);
    free(r_table_idx);

    free(r_name->data);
    free(r_name);

    free(r_type->data);
    free(r_type);

    free(r_offset->data);
    free(r_offset);

    free(r_root->data);
    free(r_root);

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

    entry[0] = r_table_idx;
    entry[1] = r_name;
    entry[2] = r_type;
    entry[3] = r_offset;
    entry[4] = r_root;

    compress_records(entry, 5, &table_root.key_vals[1]->size, &table_root.key_vals[1]->value);

    free(r_table_idx->data);
    free(r_table_idx);

    free(r_name->data);
    free(r_name);

    free(r_type->data);
    free(r_type);

    free(r_offset->data);
    free(r_offset);

    free(r_root->data);
    free(r_root);

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

    entry[0] = r_table_idx;
    entry[1] = r_name;
    entry[2] = r_type;
    entry[3] = r_offset;
    entry[4] = r_root;

    compress_records(entry, 5, &table_root.key_vals[2]->size, &table_root.key_vals[2]->value);

    free(r_table_idx->data);
    free(r_table_idx);

    free(r_name->data);
    free(r_name);

    free(r_type->data);
    free(r_type);

    free(r_offset->data);
    free(r_offset);

    free(r_root->data);
    free(r_root);

    int ret = write_node(&table_root, 1);

    free(table_root.key_vals[0]);
    free(table_root.key_vals[1]);
    free(table_root.key_vals[2]);
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

    return init_TABLES_table();
}

void *get_row(Page root, Key key, Cursor **cursor)
{
    *cursor = find_leaf(root, key);

    if (*cursor == NULL)
    {
        printf("Err get_row: Failed to find leaf\n");
        return NULL;
    }

    if ((*cursor)->node->nb_keys == 0)
    {
        printf("Err get_row: No row found\n");
        return NULL;
    }

    // Find the first index that is greater than or equal to the key
    (*cursor)->cell = 0;

    while ((*cursor)->cell < (*cursor)->node->nb_keys && (*cursor)->node->key_vals[(*cursor)->cell]->key < key)
    {
        (*cursor)->cell++;
    }

    // If the key is not found, return NULL
    if ((*cursor)->cell == (*cursor)->node->nb_keys || (*cursor)->node->key_vals[(*cursor)->cell]->key != key)
    {
        printf("Err get_row: No row found\n");
        return NULL;
    }

    return (*cursor)->node->key_vals[(*cursor)->cell]->value;
}

/**
 * @brief Retrieves the nth record from the row with key key
 */
struct record *get_n_record(Page root, Key key, size_t n)
{
    Cursor *cursor;

    void *buffer = get_row(root, key, &cursor);

    free(cursor);

    if (buffer == NULL)
    {
        printf("Err get_n_record: Failed to get row\n");
        return NULL;
    }

    struct record *records = extract_record(buffer, n);

    if (records == NULL)
    {
        printf("Err get_n_record: Failed to decompress records\n");
        return NULL;
    }

    return records;
}

/**
 * @brief Set the nth record from the row with key key
 */
int set_n_record(Page root, Key key, size_t n, struct record *new_record)
{
    Cursor *cursor;

    void *buffer = get_row(root, key, &cursor);

    if (buffer == NULL)
    {
        free(cursor);
        printf("Err get_n_record: Failed to get row\n");
        return -1;
    }

    if (replace_record(
            &cursor->node->key_vals[cursor->cell]->value,
            &cursor->node->key_vals[cursor->cell]->size,
            n,
            new_record) == -1)
    {
        free(cursor);
        printf("Err get_n_record: Failed to replace record\n");
        return -1;
    }

    write_node(cursor->node, cursor->page);

    free(cursor);
    return 0;
}

/**
 * @brief Get the table tree address of the given table.
 */
Page get_table_addr(Key root_id)
{
    struct record *rec = get_n_record(TABLES, root_id, 1);

    if (rec == NULL)
    {
        printf("Err get_next_id: Failed to get next id\n");
        return -1;
    }

    Key next_id;

    if (record_get_long(rec, (long *)&next_id) == -1)
    {
        printf("Err get_next_id: Failed to get next id\n");
        return -1;
    }

    free(rec);

    return next_id;
}

/**
 * @brief Get the next available index in the table.
 */

Key get_next_id(Key root_id)
{
    struct record *rec = get_n_record(TABLES, root_id, 2);

    if (rec == NULL)
    {
        printf("Err get_next_id: Failed to get next id\n");
        return -1;
    }

    Key next_id;

    if (record_get_long(rec, (long *)&next_id) == -1)
    {
        printf("Err get_next_id: Failed to get next id\n");
        return -1;
    }

    free(rec);

    long new_id = next_id + 1;

    set_n_record(TABLES, root_id, 2, record_from_long(&new_id));

    return next_id;
}

Key insert_row(Key root_id, struct record **records, size_t size)
{

    Page root_page = get_table_addr(root_id);

    Key row_id = get_next_id(root_id);

    if (row_id == (Page)-1)
    {
        printf("Err insert_row: Failed to get next id\n");
        return -1;
    }

    struct key_value kv = {.key = row_id};

    if (compress_records(records, size, &kv.size, &kv.value) == -1)
    {
        printf("Err insert_row: Failed to compress records\n");
        return -1;
    }

    if (insert(root_page, &kv) == -1)
    {
        printf("Err insert_row: Failed to insert row\n");
        free(kv.value);
        return -1;
    }

    free(kv.value);

    return row_id;
}

Page create_table(char *table_name)
{
    Page root_page = create_addr();

    Key next_id = 0;

    if (root_page == (Page)-1)
    {
        printf("Err create_table: Failed to create address\n");
        return -1;
    }

    struct record *rec[3];
    rec[0] = record_from_string(&table_name);
    rec[1] = record_from_long((long *)&root_page);
    rec[2] = record_from_long((long *)&next_id);

    if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL)
    {
        printf("Err create_table: Failed to create record\n");
        return (Page)-1;
    }

    Key id = insert_row(TABLES, rec, 3);

    for (size_t i = 0; i < 3; i++)
    {
        free(rec[i]);
    }

    struct node *root_node = create_node();

    if (write_node(root_node, root_page) == -1)
    {
        free(root_node);
        printf("Err create_table: Failed to write node\n");
        return -1;
    }

    free(root_node);

    return id;
}

char *select_row_columns(Key table, Key key, size_t columns[],
                         size_t num_columns)
{
    Page root = get_table_addr(table);
    Cursor *cursor;

    void *buffer = get_row(root, key, &cursor);

    free(cursor);

    if (buffer == NULL)
    {
        printf("Err select_row_columns: Failed to get row\n");
        return NULL;
    }

    char *result = (char *)malloc(sizeof(char) * 2048);

    sprintf(result, "+-----+");

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    sprintf(result, "%s\n| idx |", result);

    // adds the ids of the columns to the result
    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s %10lu |", result, columns[i]);
    }

    sprintf(result, "%s\n+-----+", result);

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    sprintf(result, "%s\n| %3lu |", result, key);

    // adds the values of the columns to the result
    for (size_t i = 0; i < num_columns; i++)
    {
        struct record *record = extract_record(buffer, columns[i]);

        if (record == NULL)
        {
            printf("Err select_row_columns: Failed to decompress records\n");
            return NULL;
        }

        char *s;
        if (record_get_string(record, &s) == -1)
        {
            printf("Err select_row_columns: Failed to get string\n");
            free(record);
            return NULL;
        }

        sprintf(result, "%s %10s |", result, s);

        free(s);
        free(record);
    }

    sprintf(result, "%s\n+-----+", result);

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    return result;
}

char *select_all(Key table, size_t columns[],
                 size_t num_columns)
{
    Page root = get_table_addr(table);
    Cursor *cursor = create_at_start(root);

    if (cursor == NULL)
    {
        printf("Err select_all: Failed to get cursor\n");
        return NULL;
    }

    char *result = (char *)malloc(sizeof(char) * 8192);

    sprintf(result, "+-----+");

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    sprintf(result, "%s\n| idx |", result);

    // adds the ids of the columns to the result
    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s %10lu |", result, columns[i]);
    }

    sprintf(result, "%s\n+-----+", result);

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    do
    {
        Key key = cursor->node->key_vals[cursor->cell]->key;
        void *buffer = cursor->node->key_vals[cursor->cell]->value;

        sprintf(result, "%s\n| %3lu |", result, key);

        // adds the values of the columns to the result
        for (size_t i = 0; i < num_columns; i++)
        {
            struct record *record = extract_record(buffer, columns[i]);

            if (record == NULL)
            {
                printf("Err select_row_columns: Failed to decompress records\n");
                return NULL;
            }

            char *s;
            if (record_get_string(record, &s) == -1)
            {
                printf("Err select_row_columns: Failed to get string\n");
                free(record);
                return NULL;
            }

            sprintf(result, "%s %10s |", result, s);

            free(s);
            free(record);
        }
    } while (cursor_next(cursor) == 0);

    sprintf(result, "%s\n+-----+", result);

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(result, "%s------------+", result, columns[i]);
    }

    return result;
}
