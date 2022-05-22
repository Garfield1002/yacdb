#include "record.h"

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

char *record_get_char(struct record *record)
{
    int t = record->type;

    switch (t)
    {
    case RECORD_TYPE_NULL:
        return 0;
    case RECORD_TYPE_CHAR:
        char *data = (char *)memcpy(malloc(sizeof(char)), record->data);
        return data;
    case RECORD_TYPE_0:
        char *data = malloc(sizeof(char));
        *data = 0;
        return data;
    case RECORD_TYPE_1:
        char *data = malloc(sizeof(char));
        *data = 1;
        return data;
    default:
        return 0;
    };
}

struct record *record_from_char(char *data)
{
    struct record *record = malloc(sizeof(struct record));

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
            record->data = data;
        }
    }
    return record;
}

struct record *record_from_short(short *data)
{
    struct record *record = malloc(sizeof(struct record));
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
            record->data = data;
        }
    }
    return record;
}

struct record *record_from_int(int *data)
{
    struct record *record = malloc(sizeof(struct record));
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
            record->data = data;
        }
    }
    return record;
}

struct record *record_from_long(long *data)
{
    struct record *record = malloc(sizeof(struct record));
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
            record->data = data;
        }
    }
    return record;
}

struct record *record_from_float(float *data)
{
    struct record *record = malloc(sizeof(struct record));
    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        record->type = RECORD_TYPE_FLOAT;
        record->data = data;
    }
    return record;
}

struct record *record_from_string(char **data)
{
    struct record *record = malloc(sizeof(struct record));
    if (data == 0)
    {
        record->type = RECORD_TYPE_NULL;
        record->data = 0;
    }
    else
    {
        record->type = LAST_RECORD_TYPE + strlen(data) + 1;
        record->data = data;
    }
    return record;
}