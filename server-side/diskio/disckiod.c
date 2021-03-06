#include "include/diskiod.h"

/**
 * @brief Prints the database header for debugging purposes
 */
void dump_header()
{
    printf("DB Header:\n");
    printf("  Signature: %s\n", db.header->signature);
    printf("  Page size: %hu\n", db.header->page_size);
    printf("  Change counter: %du\n", db.header->change_counter);
    printf("  Number of pages: %zu\n", db.header->nb_pages);
    printf("  Default cache size: %du\n", db.header->default_cache_size);
    printf("  Number of free pages: %zu\n", db.header->nb_free_pages);
    printf("  First free page: %zu\n", db.header->fst_free_page);
}

/**
 * @brief print a linked overflow
 */
void dump_linked_overflow(struct linked_overflow *overflow)
{
    if (overflow == NULL)
    {
        return;
    }

    printf("    %zu\n", overflow->page);
    dump_linked_overflow(overflow->next);
}

/**
 * @brief Prints a node for debugging purposes
 */

void dump_node(struct node *node)
{
    printf("Node:\n");
    printf("  Type: %d\n", node->type);
    printf("  Nb keys: %zu\n", node->nb_keys);
    for (size_t i = 0; i < node->nb_keys; i++)
    {
        printf("  Key %zu: %lu\n", i, node->key_vals[i]->key);
    }
    for (size_t i = 0; i < node->nb_keys; i++)
    {
        printf("  Child %zu: %zu\n", i, node->child_addrs[i]);
    }

    if (is_leaf(node))
    {
        for (size_t i = 0; i < node->nb_keys; i++)
        {
            printf("  Value %zu: %p\n", i, node->key_vals[i]->value);
        }
    }
}