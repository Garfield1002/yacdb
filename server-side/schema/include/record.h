/**
 * @file record.h
 * @author Jack Royer
 * @brief A structure for handling data of unknown type.
 *
 * @date 2022-05-29
 */

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @struct record
 * @brief Handles the general record type that abstracts away from the actual type of the data.
 */
struct record
{
    unsigned char type;
    void *data;
};

/**
 * @def LAST_RECORD_TYPE
 * @brief The last member of #record_type
 *
 * It is used to determine the size of strings.
 */
#define LAST_RECORD_TYPE 7

/**
 * @enum record_type
 * @brief The type stored inside the record.
 *
 * Currently, the only supported record types are:
 * - null
 * - int (char, short, int, long)
 * - float
 * - string
 *
 * A string is stored with any record type greater than LAST_RECORD_TYPE.
 * The value of the type determines the size of the string.
 */
enum record_type
{
    RECORD_TYPE_NULL = 0,
    RECORD_TYPE_CHAR = 1,
    RECORD_TYPE_SHORT = 2,
    RECORD_TYPE_INT = 3,
    RECORD_TYPE_LONG = 4,
    RECORD_TYPE_FLOAT = 5,
    /**
     * @deprecated causes issues with comparisons
     */
    RECORD_TYPE_0 = 6,
    /**
     * @deprecated causes issues with comparisons
     */
    RECORD_TYPE_1 = 7,
};

/**
 * @brief The size of the data stored in the record in bytes.
 */
size_t record_size(struct record *record);

int record_get_char(struct record *record, char **buffer);
int record_get_string(struct record *record, char **buffer);
int record_get_short(struct record *record, short *buffer);
int record_get_long(struct record *record, long *buffer);

/**
 * @todo
 * TODO: implement
 */
int *record_get_int(struct record *record, int *buffer);

/**
 * @todo
 * TODO: implement
 */
float *record_get_float(struct record *record, float *buffer);

/**
 * @brief Creates a pointer to a record from a ::char.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_char(char *data);

/**
 * @brief Creates a pointer to a record from a short.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_short(short *data);

/**
 * @brief Creates a pointer to a record from an int.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_int(int *data);

/**
 * @brief Creates a pointer to a record from a long.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_long(long *data);

/**
 * @brief Creates a pointer to a record from a float.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_float(float *data);

/**
 * @brief Creates a pointer to a record from a string.
 * @throw Returns NULL if an error occured.
 */
struct record *record_from_string(char **data);

/**
 * @brief Takes a list of records and returns a single buffer that is the concatenation of all the records.
 *
 * @param records The array of records to compress
 * @param nb_records The size of the array
 * @param size The final size of the buffer
 * @param buffer The buffer to be written to
 *
 * @throw Returns -1 if an error occured.
 */
int compress_records(struct record **records, size_t nb_records, size_t *size, void **buffer);

/**
 * @brief Extracts the nth record from a compressed group of records.
 * @throw Returns NULL if an error occured.
 */
struct record *extract_record(void *buffer, size_t n);

/**
 * @brief Replaces the nth record from a compressed group of records
 *
 * The buffer as well as the size will be modified (if needed).
 *
 * @throw Returns -1 if an error occured.
 */
int replace_record(void **buffer, size_t *size, size_t n, struct record *record);

/**
 * @brief Compares 2 records r1 r2.
 * @returns `0` if `r1 == r2`, `-1` if `r1 < r2`, `1` if `r1 > r2`.
 * @throw returns 2 if r1 and r2 are of different types
 */
int compare_r(struct record *r1, struct record *r2);
