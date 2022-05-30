#define DEBUG
#include "../include/db_ops.h"
#include "../include/cursor.h"
#include "../include/cursor_bt.h"
#include "../include/record.h"
#include "../../diskio/include/diskiod.h"

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
    for (size_t i = 0; i < node->nb_keys; i++)
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

Key create_user(char *table_name, char *first_name, char *last_name, long account_id)
{
    struct record *records[3];
    records[0] = record_from_string(&first_name);
    records[1] = record_from_string(&last_name);
    records[2] = record_from_long(&account_id);

    Key root_id = get_table_id(table_name);
    Key id = advanced_insert_row(root_id, records, 3);

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
    initialize_tables();

    printf("Creating tables\n");

    // Create a table for the customers
    Key customer_table = create_table("Customers", (char *[]){"first_name", "last_name", "account_id"}, 3);

    // Create a table for the accounts
    Key account_table = create_table("Accounts", (char *[]){"acc_id", "balance"}, 2);

    // Create a table for the accounts
    Key transactions_table = create_table("Transactions", (char *[]){"acc_id1", "acc_id_2", "amount"}, 3);

    printf("Creating indexes...\n");
    create_user("Customers", "John", "Doe", 12345);
    create_user("Customers", "Bob", "Marley", 89242);
    create_user("Customers", "Jane", "Doe", 54321);
    create_user("Customers", "Jill", "Doe", 2323);
    create_user("Customers", "Joe", "Doe", 65432);
    create_user("Customers", "Fred", "Johnson", 98765);

    printf("Selecting data...\n");
    char *s1 = select_all("COLUMNS", (char *[]){"TABLE_IDX", "NAMES", "NAMES"}, 3);
    printf("\n\nSELECT * FROM COLUMNS\n\n%s\n",
           s1);
    free(s1);

    char *s2 = select_row_columns("Customers", 5, (char *[]){"first_name", "last_name"}, 0);
    printf("\n\nSELECT first_name, last_name FROM customers WHERE id = 5\n\n%s\n",
           s2);
    free(s2);

    char *s3 = select_all("Customers", (char *[]){"last_name", "first_name"}, 0);
    printf("\n\nSELECT last_name, first_name FROM customers\n\n%s",
           s3);
    free(s3);

    char *s4 = select_all("Customers", (char *[]){"first_name", "last_name"}, 0);
    create_user("Customers", "Jack", "Royer", 67294);
    printf("\n\nINSERT INTO customers VALUES (Jack, Royer);\nSELECT first_name, last_name FROM customers\n\n%s\n\n",
           s4);
    free(s4);

    char *s = "Doe";
    struct record *r = record_from_string(&s);

    char *s5 = select_where("Customers", (char *[]){"first_name", "last_name"}, 2, "last_name", r);
    printf("\n\nSELECT first_name, last_name FROM customers WHERE last_name = Doe\n\n%s\n",
           s5);
    free(s5);

    free(r->data);
    free(r);
}

int main()
{
    // record_test();
    bank_db();
    exit_db();

    return 0;
}