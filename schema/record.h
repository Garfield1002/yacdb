#pragma once
#include <stdlib.h>

/**
    Handles the general record type that abstracts away from the actual type of the data.

    Currently, the only supported record types are:
        - null
        - int (char, short, int, long)
        - float
        - string
*/

struct record
{
    unsigned char type;
    void *data;
};

enum record_type
{
    RECORD_TYPE_NULL = 0,
    RECORD_TYPE_CHAR = 1,
    RECORD_TYPE_SHORT = 2,
    RECORD_TYPE_INT = 3,
    RECORD_TYPE_LONG = 4,
    RECORD_TYPE_FLOAT = 5,
    RECORD_TYPE_0 = 6,
    RECORD_TYPE_1 = 7,
};

#define LAST_RECORD_TYPE 7

size_t record_size(struct record *record);

int record_get_char(struct record *record, char **buffer);
int record_get_string(struct record *record, char **buffer);
short *record_get_short(struct record *record);
int *record_get_int(struct record *record);
int record_get_long(struct record *record, long *buffer);
float *record_get_float(struct record *record);

struct record *record_from_char(char *data);
struct record *record_from_short(short *data);
struct record *record_from_int(int *data);
struct record *record_from_long(long *data);
struct record *record_from_float(float *data);
struct record *record_from_string(char **data);

/**
 * @brief Takes a list of records and returns a single buffer that is the concatenation of all the records.
 *
 * @param records The list of records
 * @param nb_records The number of records in the list
 * @param size The size of the buffer
 * @param buffer The buffer to be written to
 */
int compress_records(struct record **records, size_t nb_records, size_t *size, void **buffer);

/**
 * @brief Extracts the nth record from a compressed group of records
 */
struct record *extract_record(void *buffer, size_t n);

/**
 * @brief Replaces the nth record from a compressed group of records
 */
int replace_record(void **buffer, size_t *size, size_t n, struct record *record);
