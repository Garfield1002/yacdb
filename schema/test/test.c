#include "../db_ops.h"

void dump_record(struct record *r)
{
    printf("Dumping record: \n");
    printf("  type: %u\n", r->type);
    printf("  data: %s\n", (char *)(r->data));
}

int main()
{
    // char *s = "Hello";
    // struct record *r1 = record_from_string(&s);
    // struct record *r2 = record_from_string(&s);
    // struct record *r3 = record_from_string(&s);

    // dump_record(r1);

    // struct record records[] = {
    //     *r1,
    //     *r2,
    //     *r3};

    // void *buffer;
    // size_t size;

    // compress_records(records, 3, &size, &buffer);

    // printf("Size: %lu\n", size);

    // printf("Buffer:");

    // for (size_t i = 0; i < size; i++)
    // {
    //     printf("%u ", ((char *)buffer)[i]);
    // }

    initialize_tables();

    return 0;
}
