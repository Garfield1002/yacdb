#include "../db_ops.h"
#include "../cursor.h"

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
}

int main()
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
