#pragma once
#include "../diskio/diskio.h"
#include "../diskio/dbfio.h"
#include "cursor.h"

#define TABLES 0
#define COLUMNS 1

typedef unsigned short col_num_t;

enum field_type
{
    FIELD_TYPE_CHAR,
    FIELD_TYPE_SHORT,
    FIELD_TYPE_INT,
    FIELD_TYPE_LONG,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_BOOL,
    FIELD_TYPE_STRING,
};

/**
 * @brief Create initial tables
 */
int initialize_tables();

/**
 * @brief Get row from table with root root
 */
void *get_row(Page root, Key key, Cursor **cursor);

/**
 * @brief Retrieves the nth record from the row with key key
 */
struct record *get_n_record(Page root, Key key, size_t n);

/**
 * @brief Get the table tree address of the given table.
 */
Page get_table_addr(Key root_id);

/**
 * @brief Get the next available index in the table.
 */

Key get_next_id(Key root_id);

/**
 * @brief Insert a row into the table with key root_id.
 */
Key insert_row(Key root_id, struct record **records, size_t size);

/**
 * @brief Creates a new table in the database.
 */
Page create_table(char *table_name);

/**
 * @brief Get the address of the index tree associated with this column
 */
Page get_column_addr(char *table_name, char *column_name);

/**
 * @brief Get the offset of a column in a table
 *
 * For a table `TABLE` with columns
 * idx | COL 0 | COL 1 | COL 2
 *
 * The offset of COL 2 is 2.
 */
col_num_t get_column_offset(char *table_name, char *column_name);

/**
 * @brief The backend for the SQL command
 * `SELECT col1, col2, col3 FROM TABLE WHERE key=value`
 */
char *select_row_columns(Key table, Key key, size_t columns[],
                         size_t num_columns);

char *select_all(Key table, size_t columns[],
                 size_t num_columns);