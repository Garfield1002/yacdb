#include "../db_ops.h"
#include "../cursor.h"
#include "../record.h"
#include "../../diskio/diskiod.h"

void dump_record(struct record *r)
{
    printf("Dumping record: \n");
    printf("  type: %u\n", r->type);
    printf("  data: %s\n", (char *)(r->data));
}

void dump_tree(Page page)
{
    struct node *node = read_node(page);
    printf("Dumping tree: \n");
    printf("  page: %lu\n", page);
    printf("  type: %u\n", node->type);
    printf("  nb_keys: %lu\n", node->nb_keys);
    printf("  keys: ");
    for (int i = 0; i < node->nb_keys; i++)
    {
        printf("%lu ", node->key_vals[i]->key);
    }
    printf("\n");
    if (!is_leaf(node))
    {
        printf("  children: ");
        for (int i = 0; i < node->nb_keys + 1; i++)
        {
            printf("%lu ", node->child_addrs[i]);
        }
        printf("\n");

        for (int i = 0; i < node->nb_keys + 1; i++)
        {
            dump_tree(node->child_addrs[i]);
        }
    }
    else
    {
        printf("  neighbor: %lu\n", node->child_addrs[0]);
    }
}

Key create_user(Key customer_table, char *first_name, char *last_name, long account_id)
{
    struct record *records[3];
    records[0] = record_from_string(&first_name);
    records[1] = record_from_string(&last_name);
    records[2] = record_from_long(&account_id);
    Key id = insert_row(customer_table, records, 3);

    for (size_t i = 0; i < 3; i++)
    {
        free(records[i]->data);
        free(records[i]);
    }
    return id;
}

void bank_db()
{
    // Use the database as a database for bank data
    printf("Initializing Database...\n");
    initialize_tables();

    printf("Creating tables...\n");

    // Create a table for the customers
    Key customer_table = create_table("Customers");

    // Create a table for the accounts
    Key account_table = create_table("Accounts");

    // Create a table for the accounts
    Key transactions_table = create_table("Transactions");

    printf("Creating indexes...\n");
    create_user(customer_table, "John", "Doe", 12345);
    create_user(customer_table, "Bob", "Marley", 89242);
    create_user(customer_table, "Jane", "Doe", 54321);
    create_user(customer_table, "Jill", "Doe", 2323);
    create_user(customer_table, "Joe", "Doe", 65432);
    create_user(customer_table, "Fred", "Johnson", 98765);

    printf("Selecting data...\n");
    printf("\n\nSELECT first_name, last_name FROM customers WHERE id = 5\n\n%s\n",
           select_row_columns(customer_table, 5, (size_t[]){0, 1}, 2));

    printf("\n\nSELECT last_name, first_name FROM customers\n\n%s",
           select_all(customer_table, (size_t[]){1, 0}, 2));

    create_user(customer_table, "Jack", "Royer", 67294);
    printf("\n\nINSERT INTO customers VALUES (Jack, Royer);\nSELECT first_name, last_name FROM customers\n\n%s",
           select_all(customer_table, (size_t[]){0, 1}, 2));
}

int record_test()
{

    char *s1 = "Hello";
    char *s2 = "World";
    char *s3 = "!";

    // create a compressed record with 3 strings
    struct record *r1 = record_from_string(&s1);
    struct record *r2 = record_from_string(&s2);
    struct record *r3 = record_from_string(&s3);

    // dump the records
    dump_record(r1);
    dump_record(r2);
    dump_record(r3);

    struct record *entry[3] = {
        r1,
        r2,
        r3};

    // compress the record
    size_t size;
    void *buffer;
    compress_records(entry, 3, &size, &buffer);

    // decompress the record
    struct record *r = extract_record(buffer, 2);

    // dump the decompressed record
    dump_record(r);

    // replace the second string with a different string

    char *s4 = "Hello World!";
    replace_record(&buffer, &size, 1, record_from_string(&s4));

    // dump the records
    dump_record(extract_record(buffer, 0));
    dump_record(extract_record(buffer, 1));
    dump_record(extract_record(buffer, 2));
}

int btree_test()
{
    init_db();

    create_addr();
    create_addr();

    struct node root;

    root.parent_addr = (Page)-1;
    root.type = NODE_TYPE_TABLE_LEAF;
    root.nb_keys = 1;
    root.key_vals[0] = (struct key_value *)malloc(sizeof(struct key_value));
    root.key_vals[0]->key = 1;
    root.key_vals[0]->size = 0;

    Page page = create_addr();
    write_node(&root, page);

    dump_tree(page);
    printf("\n\n\n\n\n");

    // struct key_value kv15 =
    //     {
    //         .key = 15,
    //         .size = 0,
    //     };

    // struct key_value kv25 =
    //     {
    //         .key = 25,
    //         .size = 0,
    //     };

    // struct key_value kv35 =
    //     {
    //         .key = 35,
    //         .size = 0,
    //     };

    // struct key_value kv45 =
    //     {
    //         .key = 45,
    //         .size = 0,
    //     };

    // insert(page, &kv15);
    // dump_tree(page);

    // printf("\n\n\n\n\n");

    // insert(page, &kv25);
    // dump_tree(page);

    // printf("\n\n\n\n\n");

    // printf("Inserting 35\n");
    // insert(page, &kv35);
    // dump_tree(page);

    // printf("\n\n\n\n\n");

    // insert(page, &kv45);
    // dump_tree(page);

    // return 0;

    struct key_value kv4 =
        {
            .key = 4,
            .size = 0,
        };
    insert(page, &kv4);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv16 =
        {
            .key = 16,
            .size = 0,
        };
    insert(page, &kv16);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv25 =
        {
            .key = 25,
            .size = 0,
        };
    insert(page, &kv25);
    dump_tree(page);

    printf("\n\n\n\n\n");

    // exit(-1);

    struct key_value kv9 =
        {
            .key = 9,
            .size = 0,
        };
    insert(page, &kv9);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv20 =
        {
            .key = 20,
            .size = 0,
        };
    insert(page, &kv20);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv13 =
        {
            .key = 13,
            .size = 0,
        };
    insert(page, &kv13);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv15 =
        {
            .key = 15,
            .size = 0,
        };
    insert(page, &kv15);
    dump_tree(page);

    printf("\n\n\n\n\n");

    struct key_value kv10 =
        {
            .key = 10,
            .size = 0,
        };
    insert(page, &kv10);
    dump_tree(page);
    printf("\n\n\n\n\n");

    struct key_value kv11 =
        {
            .key = 11,
            .size = 0,
        };
    insert(page, &kv11);
    dump_tree(page);
    printf("\n\n\n\n\n");

    struct key_value kv12 =
        {
            .key = 12,
            .size = 0,
        };
    insert(page, &kv12);
    dump_tree(page);

    return 0;
}

int main()
{
    // record_test();
    bank_db();
    return 0;
}