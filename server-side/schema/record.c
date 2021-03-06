#include "include/record.h"

size_t record_type_size(char type)
{
    switch (type)
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
        if (type < 0)
            return -1;

        // otherwise, it's a string
        return type - LAST_RECORD_TYPE - 1;
    }
}

size_t record_size(struct record *record)
{
    return record_type_size(record->type);
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
            return -1;
        }
        **buffer = *(char *)record->data;
        return 0;

    case RECORD_TYPE_0:
        *buffer = (char *)malloc(sizeof(char));
        if (*buffer == NULL)
        {
            printf("Err record_get_char: Failed to allocate buffer\n");
            return -1;
        }
        **buffer = 0;
        return 0;

    case RECORD_TYPE_1:
        *buffer = (char *)malloc(sizeof(char));
        if (*buffer == NULL)
        {
            printf("Err record_get_char: Failed to allocate buffer\n");
            return -1;
        }
        **buffer = 1;
        return 0;

    default:
        return -1;
    };
}

int record_get_short(struct record *record, short *buffer)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        *buffer = 0;
        return 0;

    case RECORD_TYPE_SHORT:
        *buffer = *(short *)record->data;
        return 0;

    case RECORD_TYPE_0:
        *buffer = 0;
        return 0;

    case RECORD_TYPE_1:
        *buffer = 1;
        return 0;

    default:
        return -1;
    };
}

int record_get_long(struct record *record, long *buffer)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        *buffer = 0;
        return 0;

    case RECORD_TYPE_LONG:
        *buffer = *(long *)record->data;
        return 0;

    case RECORD_TYPE_0:
        *buffer = 0;
        return 0;

    case RECORD_TYPE_1:
        *buffer = 1;
        return 0;

    default:
        return -1;
    };
}

int record_get_string(struct record *record, char **buffer)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        *buffer = "NULL";
        return 0;

    case RECORD_TYPE_CHAR:
        *buffer = (char *)malloc(sizeof(char) * 2);
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        sprintf(*buffer, "%c", *(char *)record->data);
        return 0;

    case RECORD_TYPE_0:
        *buffer = "0";
        return 0;

    case RECORD_TYPE_1:
        *buffer = "1";
        return 0;

    case RECORD_TYPE_SHORT:
        *buffer = (char *)malloc(sizeof(char) * 6);
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        sprintf(*buffer, "%hd", *(short *)record->data);
        return 0;

    case RECORD_TYPE_INT:
        *buffer = (char *)malloc(sizeof(char) * 11);
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        sprintf(*buffer, "%d", *(int *)record->data);
        return 0;

    case RECORD_TYPE_LONG:
        *buffer = (char *)malloc(sizeof(char) * 21);
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        sprintf(*buffer, "%ld", *(long *)record->data);
        return 0;

    case RECORD_TYPE_FLOAT:
        *buffer = (char *)malloc(sizeof(char) * 21);
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        sprintf(*buffer, "%f", *(float *)record->data);
        return 0;
    default:
        // otherwise, it's a string
        *buffer = (char *)malloc(sizeof(char) * (t - LAST_RECORD_TYPE - 1));
        if (*buffer == NULL)
        {
            printf("Err record_get_string: Failed to allocate buffer\n");
            return -1;
        }
        strcpy(*buffer, (char *)record->data);
        return 0;
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
        record->type = RECORD_TYPE_CHAR;
        record->data = malloc(sizeof(char));
        if (record->data == NULL)
        {
            printf("Err record_from_char: Failed to allocate buffer\n");
            return NULL;
        }
        *(char *)record->data = *data;
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
        record->type = RECORD_TYPE_SHORT;
        record->data = malloc(sizeof(short));
        if (record->data == NULL)
        {
            printf("Err record_from_short: Failed to allocate data\n");
            return NULL;
        }
        *(short *)record->data = *data;
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
        record->type = RECORD_TYPE_INT;
        record->data = malloc(sizeof(int));
        if (record->data == NULL)
        {
            printf("Err record_from_int: Failed to allocate data\n");
            return NULL;
        }
        *(int *)record->data = *data;
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
        record->type = RECORD_TYPE_LONG;
        record->data = malloc(sizeof(long));
        if (record->data == NULL)
        {
            printf("Err record_from_long: Failed to allocate data\n");
            return NULL;
        }
        *(long *)record->data = *data;
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
int compress_records(struct record *records[], size_t nb_records, size_t *size, void **buffer)
{

    *size = 0;

    *buffer = NULL;

    for (size_t i = 0; i < nb_records; i++)
    {
        struct record *record = records[i];

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

/**
 * @brief Extracts the nth record from a compressed group of records
 */
struct record *extract_record(void *buffer, size_t n)
{
    struct record *record = (struct record *)malloc(sizeof(struct record));

    if (record == NULL)
    {
        printf("Err extract_record: Failed to allocate record\n");
        return NULL;
    }

    // find the offset of the nth record
    size_t offs = 0;

    for (size_t i = 0; i < n; i++)
    {
        offs += record_size((struct record *)((char *)buffer + offs));
        offs++; // record header
    }

    record->type = ((char *)buffer)[offs];
    record->data = malloc(record_type_size(record->type));

    if (record->data == NULL)
    {
        printf("Err extract_record: Failed to allocate data\n");
        return NULL;
    }

    if (record_type_size(record->type) > 0)
    {
        if (memcpy(record->data, (char *)buffer + offs + 1, record_type_size(record->type)) == NULL)
        {
            printf("Err extract_record: Failed to copy data\n");
            return NULL;
        }
    }

    return record;
}

int replace_record(void **buffer, size_t *size, size_t n, struct record *record)
{
    struct record *old_record = (struct record *)malloc(sizeof(struct record));

    if (old_record == NULL)
    {
        printf("Err extract_record: Failed to allocate record\n");
        return -1;
    }

    // find the offset of the nth record
    size_t offs = 0;

    for (size_t i = 0; i < n; i++)
    {
        offs += record_size((struct record *)((char *)(*buffer) + offs));
        offs++; // record header
    }

    old_record->type = ((char *)(*buffer))[offs];
    old_record->data = (void *)((char *)(*buffer) + offs + 1);

    size_t r_size = record_size(record), old_r_size = record_size(old_record);

    size_t new_size = *size - old_r_size + r_size;

    void *new_buffer = realloc((*buffer), new_size);
    if (new_buffer == NULL)
    {
        printf("Err replace_record: Failed to allocate buffer\n");
        free(old_record);
        return -1;
    }

    if (r_size != record_size(old_record))
    {
        // move the data after the old record to the end of the buffer
        if (memmove(
                (char *)new_buffer + offs + r_size,
                (char *)new_buffer + offs + old_r_size,
                *size - offs) == NULL)
        {
            printf("Err replace_record: Failed to move data\n");
            free(old_record);
            return -1;
        };
    }

    char *r_type = (char *)new_buffer;

    r_type[offs] = record->type;

    if (r_size > 0)
    {
        if (memcpy((char *)new_buffer + offs + 1, record->data, r_size) == NULL)
        {
            printf("Err replace_record: Failed to copy data\n");
            free(old_record);
            return -1;
        }
    }

    free(old_record);

    *buffer = new_buffer;
    *size = new_size;

    return 0;
}

int compare_r(struct record *r1, struct record *r2)
{
    if (r1->type != r2->type && !(r1->type > LAST_RECORD_TYPE && r2->type > LAST_RECORD_TYPE))
    {
        return 2;
    }

    switch (r1->type)
    {
        char c1, c2;
        short s1, s2;
        int i1, i2;
        long l1, l2;
        float f1, f2;

    case RECORD_TYPE_CHAR:
        c1 = *(char *)(r1->data);
        c2 = *(char *)(r2->data);
        return (c1 > c2) - (c1 < c2);

    case RECORD_TYPE_LONG:
        l1 = *(long *)r1->data;
        l2 = *(long *)r2->data;
        return (l1 > l2) - (l1 < l2);

    case RECORD_TYPE_SHORT:
        s1 = *(short *)r1->data;
        s2 = *(short *)r2->data;
        return (s1 > s2) - (s1 < s2);

    case RECORD_TYPE_INT:
        i1 = *(int *)r1->data;
        i2 = *(int *)r2->data;
        return (i1 > i2) - (i1 < i2);

    case RECORD_TYPE_FLOAT:
        f1 = *(float *)r1->data;
        f2 = *(float *)r2->data;
        return (f1 > f2) - (f1 < f2);

    default:
        // the record is a string
        return strcmp((char *)r1->data, (char *)r2->data);
    }
}

void free_record(struct record *record)
{
    if (record->data != NULL && record_size(record) > 0)
    {
        free(record->data);
        record->data = NULL;
    }
    free(record);
}
