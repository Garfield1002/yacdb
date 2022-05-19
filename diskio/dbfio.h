#pragma once
#include <stdio.h>

typedef size_t Page;

struct yacdb_header
{
    char signature[13];
    unsigned short page_size;
    char reserved_space_byte;

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

enum btree_page_type
{
    INTERIOR_INDEX_PAGE = 2,
    INTERIOR_TABLE_PAGE = 5,
    LEAF_INDX_PAGE = 10,
    LEAF_TABLE_PAGE = 13
};

struct btree_header
{
    unsigned char page_type;
    unsigned short start_free_block;
    unsigned short number_cells;
    unsigned short start_content_area;
    unsigned int right_pointer;
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
    unsigned int size;
    unsigned int key;
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
    Page page;
};
