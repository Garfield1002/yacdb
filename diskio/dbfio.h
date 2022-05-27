#pragma once
#include <stdio.h>

typedef size_t Page;

#define YACDB_FILE_NAME "yac.db"
#define YACDB_SIGNATURE "YACDB format"
#define YACDB_PAGE_SIZE 512
#define YACDB_DEFAULT_CACHE_SIZE 10

struct yacdb_header
{
    char signature[13];
    unsigned short page_size;

    unsigned int change_counter;

    Page nb_pages;
    unsigned int default_cache_size;

    Page nb_free_pages;
    Page fst_free_page;
};

struct yacdb
{
    FILE *file;
    struct yacdb_header *header;

    // Page *cache;
    // int cache_size;
    // int cache_index;
    // int cache_count;
};

enum page_type
{
    NODE_TYPE_TABLE_LEAF = 13,
    NODE_TYPE_TABLE_INTERIOR = 5,
    NODE_TYPE_INDEX_LEAF = 10,
    NODE_TYPE_INDEX_INTERIOR = 2
};

struct btree_header
{
    Page parent_addr;
    enum page_type page_type;
    unsigned short nb_cells;
    Page right_pointer;
};

struct overflow_header
{
    Page next_page;
    size_t size;
};

struct overflow_data
{
    size_t size;
    void *data;
};

struct ti_cell
{
    unsigned int key;
    Page pointer;
};

struct tl_cell_header
{
    unsigned int key;
    unsigned int size;
    Page overflow;
};

struct tl_cell
{
    struct tl_cell_header *header;
    void *data;
};

struct btree_page
{
    struct yacdb *yacdb;
    struct btree_header *btree_h;

    void **cells_h;

    Page page;

    void *data;

    unsigned short *offsets;
};
