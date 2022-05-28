#include "db_ops.h"
#include "record.h"
#include "../diskio/diskio.h"
#include "../diskio/diskiod.h"
#include "../diskio/dbfio.h"
#include "cursor.h"
#include "cursor_bt.h"
#include <string.h>
#include <assert.h>

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

    struct key_value kv = {.key = row_id, .overflow = NULL};

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

int init_TABLES_NAME_table()
{
    Page root_page = TABLES_NAMES;

    struct node *root_node = create_node();

    root_node->type = NODE_TYPE_INDEX_LEAF;

    root_node->nb_keys = 1;
    root_node->key_vals[0] = (struct key_value *)calloc(sizeof(struct key_value), 1);

    root_node->key_vals[0]->key = TABLES;
    root_node->key_vals[0]->size = 0;
    root_node->key_vals[0]->value = NULL;
    root_node->key_vals[0]->overflow = NULL;

    if (write_node(root_node, root_page) == -1)
    {
        printf("Error: Could not write node to disk.\n");
        free(root_node);
        return -1;
    }
    free(root_node);
    return 0;
}

/**
 * @brief Creates the COLUMNS table
 */
int init_COLUMNS_table()
{

    // Create the root node
    {
        Page root_page = COLUMNS;

        Key next_id = 0;

        char *table_name = "COLUMNS";

        struct record *rec[3];
        rec[0] = record_from_string(&table_name);
        rec[1] = record_from_long((long *)&root_page);
        rec[2] = record_from_long((long *)&next_id);

        if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL)
        {
            printf("Err init_COLUMNS_table: Failed to create record\n");
            return -1;
        }

        Key id = insert_row(TABLES, rec, 3);

        assert(id == COLUMNS);

        for (size_t i = 0; i < 3; i++)
        {
            free(rec[i]);
        }

        struct node *root_node = create_node();

        if (write_node(root_node, root_page) == -1)
        {
            free(root_node);
            printf("Err init_COLUMNS_table: Failed to write node\n");
            return -1;
        }

        free(root_node);

        // add the table name to the TABLES_NAMES index tree
        insert_bt(TABLES_NAMES, COLUMNS, 0, TABLES);
    }

    // Insert the TABLES_NAMES column into the COLUMNS table
    {

        Key table_idx = TABLES;
        char *name = "NAMES";
        short _offset = 0;
        Page _root = TABLES_NAMES;

        struct record *rec[4];
        rec[0] = record_from_long((long *)&table_idx);
        rec[1] = record_from_string(&name);
        rec[2] = record_from_short(&_offset);
        rec[3] = record_from_long((long *)&_root);

        if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL || rec[3] == NULL)
        {
            printf("Err init_COLUMNS_table: Failed to create record\n");
            return -1;
        }

        Key id = insert_row(COLUMNS, rec, 4);

        for (size_t i = 0; i < 4; i++)
        {
            free(rec[i]);
        }
    }

    // Insert the COLUMNS_TIDX column into the COLUMNS table
    {

        Key table_idx = COLUMNS;
        char *name = "TABLE_IDX";
        short _offset = 0;
        Page _root = COLUMNS_TIDX;

        struct record *rec[4];
        rec[0] = record_from_long((long *)&table_idx);
        rec[1] = record_from_string(&name);
        rec[2] = record_from_short(&_offset);
        rec[3] = record_from_long((long *)&_root);

        if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL || rec[3] == NULL)
        {
            printf("Err init_COLUMNS_table: Failed to create record\n");
            return -1;
        }

        Key id = insert_row(COLUMNS, rec, 4);

        for (size_t i = 0; i < 4; i++)
        {
            free(rec[i]);
        }
    }

    // Insert the COLUMNS_NAMES column into the COLUMNS table
    {

        Key table_idx = COLUMNS;
        char *name = "NAMES";
        short _offset = 1;
        Page _root = COLUMNS_NAMES;

        struct record *rec[4];
        rec[0] = record_from_long((long *)&table_idx);
        rec[1] = record_from_string(&name);
        rec[2] = record_from_short(&_offset);
        rec[3] = record_from_long((long *)&_root);

        if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL || rec[3] == NULL)
        {
            printf("Err init_COLUMNS_table: Failed to create record\n");
            return -1;
        }

        Key id = insert_row(COLUMNS, rec, 4);

        for (size_t i = 0; i < 4; i++)
        {
            free(rec[i]);
        }
    }
}

int init_COLUMNS_TIDX_table()
{
    Page root_page = COLUMNS_TIDX;

    struct node *root_node = create_node();

    root_node->type = NODE_TYPE_INDEX_LEAF;

    root_node->nb_keys = 0;

    if (write_node(root_node, root_page) == -1)
    {
        printf("Error: Could not write node to disk.\n");
        free(root_node);
        return -1;
    }
    free(root_node);

    insert_bt(COLUMNS_TIDX, 0, 0, COLUMNS);
    insert_bt(COLUMNS_TIDX, 1, 0, COLUMNS);
    insert_bt(COLUMNS_TIDX, 2, 0, COLUMNS);
    return 0;
}

int init_COLUMNS_NAME_table()
{
    Page root_page = COLUMNS_NAMES;

    struct node *root_node = create_node();

    root_node->type = NODE_TYPE_INDEX_LEAF;

    root_node->nb_keys = 0;

    if (write_node(root_node, root_page) == -1)
    {
        printf("Error: Could not write node to disk.\n");
        free(root_node);
        return -1;
    }
    free(root_node);

    insert_bt(COLUMNS_NAMES, 0, 1, COLUMNS);
    insert_bt(COLUMNS_NAMES, 1, 1, COLUMNS);
    insert_bt(COLUMNS_NAMES, 2, 1, COLUMNS);
    return 0;
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
 * - `_offset`: the offset of the column in the table
 * - `_root`: the root page of the index tree associated with the column
 */
int initialize_tables()
{
    init_db();

    printf("Initializing tables...\n");

    if (db.header->nb_pages == 1)
    {
        printf("Creating OG tables...\n");
        create_addr(); // 1
        create_addr(); // 2
        create_addr(); // 3
        create_addr(); // 4

        init_TABLES_table();

        init_TABLES_NAME_table();
        printf("HERE\n");

        init_COLUMNS_table();

        init_COLUMNS_TIDX_table();
        init_COLUMNS_NAME_table();

        return 1;
    }
    return 0;
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

Key get_table_id(char *table_name)
{
    Key *keys;
    size_t size;

    struct record *r = record_from_string(&table_name);

    find_all(TABLES_NAMES, r, 0, TABLES, &keys, &size);

    free(r);

    if (size == 0)
    {
        printf("Err get_table_id: No table found\n");
        return -1;
    }

    if (size > 1)
    {
        printf("Err get_table_id: More than one table found\n");
        return -1;
    }

    return keys[0];
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

    struct key_value kv = {.key = row_id, .overflow = NULL};

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

    // add the table name to the TABLES_NAMES index tree
    insert_bt(TABLES_NAMES, id, 0, TABLES);

    return id;
}

char *select_row_columns(char *table_name, Key key, size_t columns[],
                         size_t num_columns)
{
    Key table_id = get_table_id(table_name);

    if (table_id == (Key)-1)
    {
        printf("Err select_row_columns: Failed to get table id\n");
        return NULL;
    }

    Page root = get_table_addr(table_id);
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

char *select_all(char *table_name, size_t columns[],
                 size_t num_columns)
{
    Key table_id = get_table_id(table_name);

    if (table_id == (Key)-1)
    {
        printf("Err select_row_columns: Failed to get table id\n");
        return NULL;
    }

    Page root = get_table_addr(table_id);

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
