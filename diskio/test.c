#include "diskio.h"
#include "diskiod.h"
#include <string.h>

/**
 * @brief Create a node
 */
void test_create_node(Page addr)
{
    struct node node;

    node.type = NODE_TYPE_TABLE_LEAF;
    node.nb_keys = 1;
    node.key_vals[0] = (struct key_value *)malloc(sizeof(struct key_value));
    node.key_vals[0]->key = 1;

    char data[] = "Hello World!";
    node.key_vals[0]->size = sizeof(data);
    node.key_vals[0]->value = malloc(node.key_vals[0]->size);
    memcpy(node.key_vals[0]->value, data, node.key_vals[0]->size);

    write_node(&node, addr);

    dump_node(&node);
}

/**
 * @brief Creates a large node that requires overflow
 */
void test_create_overflow_node(Page addr)
{
    struct node node;

    node.type = NODE_TYPE_TABLE_LEAF;
    node.nb_keys = 1;
    node.key_vals[0] = (struct key_value *)malloc(sizeof(struct key_value));
    node.key_vals[0]->key = 1;

    char data[] = "Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum  Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua Ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur Excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum";

    node.key_vals[0]->size = sizeof(data);
    node.key_vals[0]->value = malloc(node.key_vals[0]->size);
    memcpy(node.key_vals[0]->value, data, node.key_vals[0]->size);

    write_node(&node, addr);

    dump_node(&node);
}

int main()
{
    printf("AAAAAAAAAAAAAAAAA");
    init_db();

    dump_header();

    remove_node(2);

    Page addr = create_addr();

    printf("Created page at %p\n", addr);

    test_create_node(addr);

    dump_header();

    struct node *node = read_node(addr);

    dump_node(node);

    write_node(node, addr);

    remove_node(2);

    dump_header();

    return 0;
}