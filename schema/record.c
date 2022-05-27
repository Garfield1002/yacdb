#include "record.h"
#include <stdio.h>
#include <string.h>

size_t record_size(struct record *record)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        return 0;
    case RECORD_TYPE_CHAR:
        return 1;
    case RECORD_TYPE_SHORT:
        return 2;
    case RECORD_TYPE_INT:
        return 4;
    case RECORD_TYPE_LONG:
        return 8;
    case RECORD_TYPE_FLOAT:
        return 4;
    case RECORD_TYPE_0:
        return 0;
    case RECORD_TYPE_1:
        return 0;

    default:
        if (t < 0)
            return -1;

        // otherwise, it's a string
        return t - LAST_RECORD_TYPE - 1;
    }
}

int record_get_char(struct record *record, char **buffer)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        *buffer = NULL;
        return 0;

    case RECORD_TYPE_CHAR:
        *buffer = (char *)malloc(sizeof(char));
        if (*buffer == NULL)
        {
            printf("Err record_get_char: Failed to allocate buffer\n");
            return 0;
        }
        **buffer = *(char *)record->data;
        return 0;

    case RECORD_TYPE_0:
        *buffer = (char *)malloc(sizeof(char));
        if (*buffer == NULL)
        {
            printf("Err record_get_char: Failed to allocate buffer\n");
            return 0;
        }
        **buffer = 0;
        return 0;

    case RECORD_TYPE_1:
        *buffer = (char *)malloc(sizeof(char));
        if (*buffer == NULL)
        {
            printf("Err record_get_char: Failed to allocate buffer\n");
            return 0;
        }
        **buffer = 1;
        return 0;

    default:
        return -1;
    };
}

struct record *record_from_char(char *data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_char: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        switch (*data)
        {
        case 0:
            record->type = RECORD_TYPE_0;
            record->data = 0;
            break;

        case 1:
            record->type = RECORD_TYPE_1;
            record->data = 0;
            break;

        default:
            record->type = RECORD_TYPE_CHAR;
            record->data = malloc(sizeof(char));
            if (record->data == NULL)
            {
                printf("Err record_from_char: Failed to allocate buffer\n");
                return NULL;
            }
            *(char *)record->data = *data;
        }
    }
    return record;
}

struct record *record_from_short(short *data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_short: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        switch (*data)
        {
        case 0:
            record->type = RECORD_TYPE_0;
            record->data = 0;
            break;

        case 1:
            record->type = RECORD_TYPE_1;
            record->data = 0;
            break;

        default:
            record->type = RECORD_TYPE_SHORT;
            record->data = malloc(sizeof(short));
            if (record->data == NULL)
            {
                printf("Err record_from_short: Failed to allocate data\n");
                return NULL;
            }
            *(short *)record->data = *data;
        }
    }
    return record;
}

struct record *record_from_int(int *data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_int: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        switch (*data)
        {
        case 0:
            record->type = RECORD_TYPE_0;
            record->data = 0;
            break;

        case 1:
            record->type = RECORD_TYPE_1;
            record->data = 0;
            break;

        default:
            record->type = RECORD_TYPE_INT;
            record->data = malloc(sizeof(int));
            if (record->data == NULL)
            {
                printf("Err record_from_int: Failed to allocate data\n");
                return NULL;
            }
            *(int *)record->data = *data;
        }
    }
    return record;
}

struct record *record_from_long(long *data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_long: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        switch (*data)
        {
        case 0:
            record->type = RECORD_TYPE_0;
            record->data = 0;
            break;

        case 1:
            record->type = RECORD_TYPE_1;
            record->data = 0;
            break;

        default:
            record->type = RECORD_TYPE_LONG;
            record->data = malloc(sizeof(long));
            if (record->data == NULL)
            {
                printf("Err record_from_long: Failed to allocate data\n");
                return NULL;
            }
            *(long *)record->data = *data;
        }
    }
    return record;
}

struct record *record_from_float(float *data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_float: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        record->type = RECORD_TYPE_FLOAT;
        record->data = malloc(sizeof(float));
        if (record->data == NULL)
        {
            printf("Err record_from_float: Failed to allocate buffer\n");
            return NULL;
        }
        *(float *)record->data = *data;
    }
    return record;
}

struct record *record_from_string(char **data)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err record_from_string: Failed to allocate record\n");
        return NULL;
    }

    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        record->type = LAST_RECORD_TYPE + strlen(*data) + 2;
        record->data = malloc(strlen(*data) + 1);
        if (record->data == NULL)
        {
            printf("Err record_from_string: Failed to allocate data\n");
            return NULL;
        }
        strcpy((char *)record->data, *data);
    }
    return record;
}

/**
 * @brief Takes a list of records and returns a single buffer that is the concatenation of all the records.
 *
 * @param records The list of records
 * @param nb_records The number of records in the list
 * @param size The size of the buffer
 * @param buffer The buffer to be written to
 */
int compress_records(struct record *records, size_t nb_records, size_t *size, void **buffer)
{

    *size = 0;

    *buffer = NULL;

    for (size_t i = 0; i < nb_records; i++)
    {
        struct record *record = &records[i];

        size_t r_size = record_size(record);

        void *temp = realloc(*buffer, *size + r_size + 1);
        if (temp == NULL)
        {
            printf("Err compress_records: Failed to allocate buffer\n");
            return -1;
        }
        *buffer = temp;

        char *r_type = (char *)*buffer;

        r_type[*size] = record->type;

        if (r_size > 0)
        {
            if (memcpy((char *)*buffer + *size + 1, record->data, r_size) == NULL)
            {
                printf("Err compress_records: Failed to copy data\n");
                return -1;
            }
        }

        *size += r_size + 1;
    }

    return 0;
}