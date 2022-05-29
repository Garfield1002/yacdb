#pragma once
#include "../../diskio/include/diskio.h"
#include "../../diskio/include/db_structs.h"
#include "../../diskio/include/diskiod.h"

#include "record.h"
#include "cursor.h"
#include "cursor_bt.h"
#include <string.h>
#include <assert.h>

#define TABLES 0
#define COLUMNS 1
#define TABLES_NAMES 2
#define COLUMNS_TIDX 3
#define COLUMNS_NAMES 4

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
Page get_table_addr(Key table_id);

/**
 * @brief Get the table id from a table name
 */
Key get_table_id(char *table_name);

/**
 * @brief Get the next available index in the table.
 */

Key get_next_id(Key root_id);

/**
 * @brief Insert a row into the table with key root_id.
 */
Key insert_row(Key root_id, struct record **records, size_t size);

Key advanced_insert_row(Key tidx, struct record **records, size_t size);

/**
 * @brief Creates a new table in the database.
 */
Page create_table(char *table_name, char **column_names, unsigned short n);

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
short *get_column_offsets(Key table_id, char *column_names[], size_t nb_cols);

/**
 * @brief The backend for the SQL command
 * `SELECT col1, col2, col3 FROM TABLE WHERE key=value`
 */
char *select_row_columns(char *table_name, Key key, char *columns[],
                         size_t num_columns);

char *select_all(char *table_name, char *columns[],
                 size_t num_columns);

/**
 * @brief Selects all rows from a table where the given column is equal to the
 * given value.
 *
 * @param table_name The name of the table
 * @param col_names
 * @param num_columns
 * @param where_col_name
 * @param where_record
 * @return char*
 */
char *select_where(
    char *table_name, char *col_names[],
    size_t num_columns,
    char *where_col_name,
    struct record *where_record);