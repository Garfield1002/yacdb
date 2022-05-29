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
 * @param table_name The name of the table to create.
 * @param column_names The names of the columns in the table.
 * @param nb_cols The number of columns in the table.
 *
 * @return The identifier of the newly created table, -1 if an error occurred.
 */
Page create_table(
    char *table_name,
    char **column_names,
    short nb_cols);

/**
 * @brief Get the offsets of a list of columns in a table
 *
 * For a table TABLE with columns idx, COL 0, COL 1, COL 2; the offset of COL 2 is 2.
 *
 * Use this function to get all the offsets at once as it is a costly operation.
 *
 * @param table_id The identifier of the table (it can be obtained with get_table_id)
 * @param column_names The names of the columns
 * @param nb_cols The number of columns
 *
 * @return The offsets of the columns in the table, NULL if an error occurred.
 */
short *get_column_offsets(
    Key table_id,
    char *col_names[],
    size_t nb_cols);

/**
 * @brief Selects a row from a table with the given key
 * @param table_name The name of the table to select from
 * @param key The key of the row to select
 * @param col_names The names of the columns the user wants to select
 * @param nb_cols The number of columns to select
 */
char *select_row_columns(
    char *table_name,
    Key key,
    char *col_names[],
    size_t nb_cols);

/**
 * @brief Selects all rows from a table
 *
 * @param table_name The name of the table
 * @param col_names The names of the columns the user wants to select
 * @param nb_cols The number of columns the user wants to select, if 0 select all
 *
 * @return char* The result of the query, NULL if an error occured
 */
char *select_all(
    char *table_name,
    char *col_names[],
    size_t nb_cols);

/**
 * @brief Selects all rows from a table where the given column is equal to the
 * given value.
 *
 * @param table_name The name of the table
 * @param col_names The names of the columns the user wants to select
 * @param nb_cols The number of columns the user wants to select, if 0 select all
 * @param where_col_name The name of the column to compare
 * @param where_record  The value to compare
 *
 * @return char* The result of the query, NULL if an error occured
 */
char *select_where(
    char *table_name,
    char *col_names[],
    size_t nb_cols,
    char *where_col_name,
    struct record *where_record);