#pragma once
#include "../diskio/diskio.h"

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
 * @brief Get row from table
 */
void *get_row(const char *table_name, const char *key);

/**
 * @brief Creates a new table in the database.
 */
int create_table(char *table_name, char *column_names[],
                 enum field_type column_types[], col_num_t num_columns);

/**
 * @brief Get the table tree address of the given table.
 */
Page get_table_addr(char *table_name);

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
