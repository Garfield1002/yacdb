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
    int type;
    void *data;
};


enum record_type{
    RECORD_TYPE_NULL,
    RECORD_TYPE_CHAR,
    RECORD_TYPE_SHORT,
    RECORD_TYPE_INT,
    RECORD_TYPE_LONG,
    RECORD_TYPE_FLOAT,
    RECORD_TYPE_0,
    RECORD_TYPE_1,
};

/**
 * @brief Gets the size of the record. The size is given by the table in the documentation.
 *
 * @param record
 * @return int
 */
int record_size(struct record *record);

char record_get_char(struct record *record);

short record_get_short(struct record *record);

int record_get_int(struct record *record);

long record_get_long(struct record *record);

float record_get_float(struct record *record);

char *record_get_string(struct record *record);
