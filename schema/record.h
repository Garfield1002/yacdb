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
    size_t type;
    void *data;
};

enum record_type
{
    RECORD_TYPE_NULL,
    RECORD_TYPE_CHAR,
    RECORD_TYPE_SHORT,
    RECORD_TYPE_INT,
    RECORD_TYPE_LONG,
    RECORD_TYPE_FLOAT,
    RECORD_TYPE_0,
    RECORD_TYPE_1,
};

#define LAST_RECORD_TYPE RECORD_TYPE_1

size_t record_size(struct record *record);

char *record_get_char(struct record *record);
short *record_get_short(struct record *record);
int *record_get_int(struct record *record);
long *record_get_long(struct record *record);
float *record_get_float(struct record *record);
char **record_get_string(struct record *record);

struct record *record_from_char(char *data);
struct record *record_from_short(short *data);
struct record *record_from_int(int *data);
struct record *record_from_long(long *data);
struct record *record_from_float(float *data);
struct record *record_from_string(char **data);