#include "include/db_ops.h"

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
 *
 * Creates the initial tables in the database if they are missing.
 * There are 2 tables:
 * - `TABLES`: a table of all tables in the database.
 * - `COLUMNS`: a table of all columns in the database.
 *
 * These trees have "hidden" columns: they are marked with an underscore.
 *
 * ## TABLES table
 *
 * It's root is on page TABLES, it has index TABLES.
 *
 * The table has the following columns:
 * | Column | Offset | Type |  Description |
 * | :-: | :-: | :-: | :-: |
 * | `_idx` | - | `::Key` | The index of the table in the database |
 * | `name` | 0 | `char *`| The name of the table |
 * | `_root` | 1 | `::Page` | the root page of the table |
 * | `_next_id`| 2|  `::Key` | the next available id for a row in the table|

 * ## COLUMNS table
 *
 * It's root is on page COLUMNS, it has index COLUMNS.
 * It points to the index trees associated with each column.
 *
 * The table has the following columns:
 * | Column | Offset | Type |  Description |
 * | :-: | :-: | :-: | :-: |
 * | `_idx` | - | `::Key` | The index of the column in the database |
 * | `table_idx` | 0 | `::Key` | The index of the table |
 * | `name` | 1 | `char *`| The name of the column |
 * | `_offset` | 2 | `short` | The offset of the column in the table |
 * | `_root` | 3 | `::Page` | The root page of the index tree |
 *
 */
int initialize_tables()
{
    init_db();

#ifdef DEBUG
    printf("Initializing tables...\n");
#endif

    if (db.header->nb_pages == 1)
    {
#ifdef DEBUG
        printf("Creating OG tables...\n");
#endif

        create_addr(); // 1
        create_addr(); // 2
        create_addr(); // 3
        create_addr(); // 4

        init_TABLES_table();

        init_TABLES_NAME_table();

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

Key advanced_insert_row(Key tidx, struct record **records, size_t size)
{
    Key id = insert_row(tidx, records, size);

    Page root_page = get_table_addr(tidx);

    Key *cols;
    size_t nb_cols;

    struct record *tidx_r = record_from_long((long *)&tidx);

    find_all(COLUMNS_TIDX, tidx_r, 0, COLUMNS, &cols, &nb_cols);

    free(tidx_r);

    for (size_t i = 0; i < nb_cols; i++)
    {
        // get the column info from the COLUMNS table
        Cursor *cursor;
        void *buffer = get_row(COLUMNS, cols[i], &cursor);
        free(cursor);

        struct record *offs_r = extract_record(buffer, 2);
        struct record *page_r = extract_record(buffer, 3);

        if (offs_r == NULL || page_r == NULL)
        {
            printf("Err advanced_insert_row: Failed to extract records\n");
            return -1;
        }

        short offs;
        Page page;

        if (record_get_short(offs_r, &offs) == -1 ||
            record_get_long(page_r, (long *)&page) == -1)
        {
            printf("Err advanced_insert_row: Failed to extract records\n");
            return -1;
        }

        free(offs_r);
        free(page_r);

        insert_bt(page, id, offs, root_page);

        free(buffer);
    }

    return id;
}

short *get_column_offsets(Key table_id, char *column_names[], size_t nb_cols)
{
    short *my_offsets = (short *)calloc(sizeof(short), nb_cols);

    struct record *table_id_r = record_from_long((long *)&table_id);

    Key *cols;
    size_t nb_cols_t;
    if (
        find_all(COLUMNS_TIDX, table_id_r, 0, COLUMNS,
                 &cols, &nb_cols_t) == -1)
    {
        printf("Err get_column_offsets: Failed to find columns\n");
        free(my_offsets);
        free(table_id_r);
        return NULL;
    }

    free(table_id_r);

    short *offsets = (short *)calloc(sizeof(short), nb_cols_t);
    char **names = (char **)calloc(sizeof(char *), nb_cols_t);

    // retrieves all the column names
    for (size_t i = 0; i < nb_cols_t; i++)
    {
        struct record *name_r = get_n_record(COLUMNS, cols[i], 1);

        if (name_r == NULL)
        {
            printf("Err get_column_offsets: Failed to extract records\n");
            // TODO: free everything
            return NULL;
        }

        char *name;

        if (record_get_string(name_r, &name) == -1)
        {
            printf("Err get_column_offsets: Failed to extract records\n");
            // TODO: free everything
            return NULL;
        }

        free(name_r);

        names[i] = name;
    }

    // retrieves all the offsets
    for (size_t i = 0; i < nb_cols_t; i++)
    {
        struct record *offs_r = get_n_record(COLUMNS, cols[i], 2);

        if (offs_r == NULL)
        {
            printf("Err get_column_offsets: Failed to extract records\n");
            // TODO: free everything
            return NULL;
        }

        short offs;

        if (record_get_short(offs_r, &offs) == -1)
        {
            printf("Err get_column_offsets: Failed to extract records\n");
            // TODO: free everything
            return NULL;
        }

        free(offs_r);

        offsets[i] = offs;
    }

    // finds the offsets of the columns
    for (size_t i = 0; i < nb_cols; i++)
    {
        my_offsets[i] = -1;

        for (size_t j = 0; j < nb_cols_t; j++)
        {
            if (strcmp(names[j], column_names[i]) == 0)
            {
                my_offsets[i] = offsets[j];
                break;
            }
        }

        if (my_offsets[i] == -1)
        {
            printf("Err get_column_offsets: Failed to find column\n");
        }
    }

    free(cols);
    free(offsets);

    for (size_t i = 0; i < nb_cols_t; i++)
    {
        free(names[i]);
    }
    free(names);

    return my_offsets;
}

Page create_table(char *table_name, char **column_names, short nb_cols)
{
    Page root_page = create_addr();

    Key next_id = 0;

    if (root_page == (Page)-1)
    {
        printf("Err create_table: Failed to create address\n");
        return (Page)-1;
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

    Key id = advanced_insert_row(TABLES, rec, 3);

    for (size_t i = 0; i < 3; i++)
    {
        free(rec[i]);
    }

    // Creates the columns
    for (short i = 0; i < nb_cols; i++)
    {
        Page col_page = create_addr();
        struct node *col_node = create_node();
        if (col_node == NULL)
        {
            printf("Err create_table: Failed to create node\n");
            return (Page)-1;
        }
        col_node->type = NODE_TYPE_INDEX_LEAF;

        if (write_node(col_node, col_page) == -1)
        {
            free(col_node);
            printf("Err create_table: Failed to write node\n");
            return (Page)-1;
        }

        struct record *rec[4];
        rec[0] = record_from_long((long *)&id);
        rec[1] = record_from_string(&column_names[i]);
        rec[2] = record_from_short((short *)&i);
        rec[3] = record_from_long((long *)&col_page);

        if (rec[0] == NULL || rec[1] == NULL || rec[2] == NULL ||
            rec[3] == NULL)
        {
            printf("Err create_table: Failed to create record\n");
            return (Page)-1;
        }

        Key col_id = advanced_insert_row(COLUMNS, rec, 4);

        for (size_t j = 0; j < 4; j++)
        {
            free(rec[j]);
        }

        free(col_node);
    }

    struct node *root_node = create_node();

    if (write_node(root_node, root_page) == -1)
    {
        free(root_node);
        printf("Err create_table: Failed to write node\n");
        return (Page)-1;
    }

    free(root_node);

    return id;
}

/**
 * @brief Prints the header for the database output.
 */
int print_header(char *buffer, char *columns[], size_t nb_cols)
{
    sprintf(buffer, "+-----+");

    for (size_t i = 0; i < nb_cols; i++)
    {
        sprintf(buffer, "%s------------+", buffer);
    }

    sprintf(buffer, "%s\n| idx |", buffer);

    // adds the ids of the columns to the result
    for (size_t i = 0; i < nb_cols; i++)
    {
        sprintf(buffer, "%s %10s |", buffer, columns[i]);
    }

    sprintf(buffer, "%s\n+-----+", buffer);

    for (size_t i = 0; i < nb_cols; i++)
    {
        sprintf(buffer, "%s------------+", buffer, columns[i]);
    }
    return 0;
}

int print_row(char *buffer, void *row, short columns[],
              size_t nb_cols)
{
    for (size_t i = 0; i < nb_cols; i++)
    {
        struct record *record = extract_record(row, columns[i]);

        if (record == NULL)
        {
            printf("Err select_row_columns: Failed to decompress records\n");
            return -1;
        }

        char *s;
        if (record_get_string(record, &s) == -1)
        {
            printf("Err select_row_columns: Failed to get string\n");
            free(record);
            return -1;
        }

        sprintf(buffer, "%s %10s |", buffer, s);

        free(s);
        free(record);
    }

    return 0;
}

int print_footer(char *buffer, size_t num_columns)
{
    sprintf(buffer, "%s\n+-----+", buffer);

    for (size_t i = 0; i < num_columns; i++)
    {
        sprintf(buffer, "%s------------+", buffer);
    }
    return 0;
}

int get_all_columns(Key table_id, char ***col_names, size_t *nb_cols)
{
    struct record *table_id_r = record_from_long((long *)&table_id);

    Key *cols;
    if (
        find_all(COLUMNS_TIDX, table_id_r, 0, COLUMNS,
                 &cols, nb_cols) == -1)
    {
        printf("Err get_column_offsets: Failed to find columns\n");
        free(table_id_r);
        return -1;
    }

    free(table_id_r);

    // Gets the names of the columns
    *col_names = (char **)calloc(sizeof(char *), *nb_cols);

    for (size_t i = 0; i < *nb_cols; i++)
    {
        struct record *col_name_r = get_n_record(COLUMNS, cols[i], 1);

        if (col_name_r == NULL)
        {
            printf("Err get_all_columns: Failed to extract column name\n");
            return -1;
        }

        char *col_name;
        if (record_get_string(col_name_r, &col_name) == -1)
        {
            printf("Err get_all_columns: Failed to get column name\n");
            return -1;
        }

        (*col_names)[i] = col_name;
    }

    free(cols);
    return 0;
}

char *select_row_columns(char *table_name, Key key, char *col_names[],
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
    if (result == NULL)
    {
        printf("Err select_row_columns: Failed to allocate memory\n");
        free(buffer);
        return NULL;
    }

    if (num_columns == 0)
    {
        // Get all columns
        get_all_columns(table_id, &col_names, &num_columns);
    }

    short *columns;

    // get the column offsets
    columns = get_column_offsets(table_id, col_names, num_columns);
    if (columns == NULL)
    {
        printf("Err select_all: Failed to get column offsets\n");
        free(result);
        return NULL;
    }

    if (print_header(result, col_names, num_columns) == -1)
    {
        printf("Err select_row_columns: Failed to print header\n");
        free(result);
        free(buffer);
        return NULL;
    }

    sprintf(result, "%s\n| %3lu |", result, key);

    // adds the values of the columns to the result
    if (print_row(result, buffer, columns, num_columns) == -1)
    {
        printf("Err select_row_columns: Failed to print row\n");
        free(result);
        free(buffer);
        return NULL;
    }

    print_footer(result, num_columns);

    free(buffer);

    return result;
}

char *select_all(char *table_name, char *col_names[],
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

    if (num_columns == 0)
    {
        // Get all columns
        get_all_columns(table_id, &col_names, &num_columns);
    }

    print_header(result, col_names, num_columns);

    // get the column offsets
    short *columns = get_column_offsets(table_id, col_names, num_columns);

    if (columns == NULL)
    {
        printf("Err select_all: Failed to get column offsets\n");
        free(result);
        free(cursor);
        return NULL;
    }

    do
    {
        Key key = cursor->node->key_vals[cursor->cell]->key;
        void *buffer = cursor->node->key_vals[cursor->cell]->value;

        sprintf(result, "%s\n| %3lu |", result, key);

        if (print_row(result, buffer, columns, num_columns) == -1)
        {
            printf("Err select_all: Failed to print row\n");
            free(result);
            free(cursor);
            return NULL;
        }
    } while (cursor_next(cursor) == 0);

    print_footer(result, num_columns);

    free(columns);
    free(cursor);
    return result;
}

char *select_where(
    char *table_name, char *col_names[],
    size_t num_columns,
    char *where_col_name,
    struct record *where_record)
{
    Key table_id = get_table_id(table_name);
    Page root = get_table_addr(table_id);

    // find the column offset and root
    struct record *table_id_r = record_from_long((long *)&table_id);

    Key *cols;
    size_t nb_cols_t;
    if (
        find_all(COLUMNS_TIDX, table_id_r, 0, COLUMNS,
                 &cols, &nb_cols_t) == -1)
    {
        printf("Err select_where: Failed to find columns\n");
        free(table_id_r);
        return NULL;
    }
    free(table_id_r);

    short where_col_offset = -1;
    Page where_col_root = (Page)-1;

    for (size_t i = 0; i < nb_cols_t; i++)
    {
        Cursor *cursor;
        void *buffer = get_row(COLUMNS, cols[i], &cursor);
        free(cursor);

        if (buffer == NULL)
        {
            printf("Err select_where: Failed to get row\n");
            free(cols);
            return NULL;
        }

        struct record *r = extract_record(buffer, 1);

        if (r == NULL)
        {
            printf("Err select_where: Failed to extract record\n");
            free(buffer);
            free(cols);
            return NULL;
        }

        char *col_name;

        if (record_get_string(r, &col_name) == -1)
        {
            printf("Err select_where: Failed to get string\n");
            free(r);
            free(buffer);
            free(cols);
            return NULL;
        }

        free(r);

        if (strcmp(col_name, where_col_name) == 0)
        {
            r = extract_record(buffer, 2);
            if (r == NULL)
            {
                printf("Err select_where: Failed to extract record\n");
                free(buffer);
                free(cols);
                return NULL;
            }
            where_col_offset = *(short *)r->data;
            free(r);

            r = extract_record(buffer, 3);
            if (r == NULL)
            {
                printf("Err select_where: Failed to extract record\n");
                free(buffer);
                free(cols);
                return NULL;
            }
            where_col_root = *(Page *)r->data;
            free(r);
            free(buffer);
            break;
        }
        free(buffer);
    }

    free(cols);
    cols = NULL;
    if (where_col_offset == -1)
    {
        printf("Err select_where: Failed to find column\n");
        return NULL;
    }

    Key *keys;
    size_t size;

    if (
        find_all(
            where_col_root,
            where_record,
            where_col_offset,
            root,
            &keys,
            &size) == -1)
    {
        printf("Err select_where: Failed to find keys\n");
        free(cols);
        return NULL;
    }

    char *result = (char *)malloc(sizeof(char) * 2048);
    if (result == NULL)
    {
        printf("Err select_where: Failed to allocate memory\n");
        return NULL;
    }

    if (num_columns == 0)
    {
        // Get all columns
        get_all_columns(table_id, &col_names, &num_columns);
    }

    if (print_header(result, col_names, num_columns) == -1)
    {
        printf("Err select_where: Failed to print header\n");
        free(result);
        return NULL;
    }

    // get the column offsets
    short *columns = get_column_offsets(table_id, col_names, num_columns);
    if (columns == NULL)
    {
        printf("Err select_where: Failed to get column offsets\n");
        free(result);
        return NULL;
    }

    for (size_t i = 0; i < size; i++)
    {
        Key key = keys[i];

        Cursor *cursor;
        void *buffer = get_row(root, key, &cursor);
        free(cursor);

        if (buffer == NULL)
        {
            printf("Err select_where: Failed to get row\n");
            return NULL;
        }
        sprintf(result, "%s\n| %3lu |", result, key);

        // adds the values of the columns to the result
        if (print_row(result, buffer, columns, num_columns) == -1)
        {
            printf("Err select_where: Failed to print row\n");
            free(result);
            free(buffer);
            return NULL;
        }
        free(buffer);
    }

    print_footer(result, num_columns);

    free(columns);
    free(keys);

    return result;
}
