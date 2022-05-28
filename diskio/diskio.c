#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include "diskio.h"

bool is_leaf(struct node *node)
{
    return node->type == NODE_TYPE_TABLE_LEAF ||
           node->type == NODE_TYPE_INDEX_LEAF;
}

/**
 * @brief Reads the content of a page and returns it
 */
// TODO: Cache
void *read_page(struct yacdb *db, size_t addr)
{
    if (addr >= db->header->nb_pages)
    {
        printf("ERR set_cursor: Offset out of range\n");
        return NULL;
    }

    size_t offset = addr * db->header->page_size;
    size_t size = db->header->page_size;

    // In the first block we need to ignore the header
    if (addr == 0)
    {
        offset += sizeof(struct yacdb_header);
        size -= sizeof(struct yacdb_header);
    }

    if (fseek(db->file, offset, SEEK_SET))
    {
        printf("ERR set_cursor: Failed to seek\n");
        return NULL;
    }

    void *buf = malloc(size);

    if (buf == NULL)
    {
        printf("ERR read_at: Failed to allocate buffer\n");
        return NULL;
    }

    if (fread(buf, size, 1, db->file) != 1)
    {
        free(buf);
        printf("ERR read_at: Failed to read\n");
        return NULL;
    }

    return buf;
}

/**
 * @brief Get the btree header object
 *
 * @param rpage the raw data of the page
 *
 * @return struct btree_header*
 */
struct btree_header *get_btree_header(void *rpage)
{
    struct btree_header *header = (struct btree_header *)malloc(sizeof(struct btree_header));

    if (header == NULL)
    {
        printf("ERR get_btree_header: Failed to allocate buffer\n");
        return NULL;
    }

    if (memcpy(header, rpage, sizeof(struct btree_header)) == NULL)
    {
        free(header);
        printf("ERR get_btree_header: Failed to copy data\n");
        return NULL;
    }

    return header;
}

/**
 * @brief Get the offsets of the different cells
 * @deprecated
 */
int get_offsets(struct btree_page *page)
{
    size_t size = sizeof(unsigned short) * page->btree_h->nb_cells;

    page->offsets = (unsigned short *)malloc(size);

    if (page->offsets == NULL)
    {
        free(page->offsets);
        printf("ERR get_offsets: Failed to allocate buffer\n");
        return -1;
    }

    if (memcpy(page->offsets, (char *)page->data + sizeof(struct btree_header), size) == NULL)
    {
        free(page->offsets);
        printf("ERR get_offsets: Failed to copy data\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Gets data out of overflow pages recursively.
 */
int get_overflow_data(Page page, struct overflow_data *data, struct linked_overflow *history)
{
    if (page == 0)
        return -1;

    history = (struct linked_overflow *)malloc(sizeof(struct linked_overflow));

    if (history == NULL)
    {
        printf("ERR get_overflow_data: Failed to allocate buffer\n");
        return -1;
    }

    history->page = page;

    void *raw = read_page(&db, page);

    if (raw == NULL)
    {
        free(history);
        printf("ERR get_overflow_data: Failed to read page\n");
        return -1;
    }

    struct overflow_header *header = (struct overflow_header *)raw;

    data->size = header->size;
    data->data = malloc(header->size);

    if (data->data == NULL)
    {
        free(history);
        printf("ERR get_overflow_data: Failed to allocate data\n");
        return -1;
    }

    if (memcpy(data->data, (char *)raw + sizeof(struct overflow_header), header->size) == NULL)
    {
        free(history);

        free(data->data);
        printf("ERR get_overflow_data: Failed to copy data\n");
        return -1;
    }

    if (header->next_page != 0)
    {
        struct overflow_data *next = (struct overflow_data *)malloc(sizeof(struct overflow_data));

        if (next == NULL)
        {
            free(history);

            free(data->data);
            printf("ERR get_overflow_data: Failed to allocate buffer\n");
            return -1;
        }

        if (get_overflow_data(header->next_page, next, history->next) == -1)
        {
            free(history);
            free(data->data);
            free(next);
            printf("ERR get_overflow_data: Failed to get next page\n");
            return -1;
        }

        data->size += next->size;
        data->data = realloc(data->data, data->size);

        if (memcpy((char *)data->data + data->size - next->size, next->data, next->size))
        {
            free(history);
            free(data->data);
            free(header);
            free(next->data);
            free(next);
            printf("ERR get_overflow_data: Failed to copy next overflow page\n");
            return -1;
        }

        free(next->data);
        free(next);
    }

    free(raw);

    return 0;
}

/**
 * @brief Gets the overflow data of a table leaf
 */
int get_tl_overflow(Page page, struct key_value *kv)
{
    struct overflow_data *data = (struct overflow_data *)malloc(sizeof(struct overflow_data));

    if (data == NULL)
    {
        printf("ERR get_tl_overflow: Failed to allocate buffer\n");
        return -1;
    }

    if (get_overflow_data(page, data, kv->overflow) == -1)
    {
        free(data);
        printf("ERR get_tl_overflow: Failed to get overflow data\n");
        return -1;
    }

    kv->size += data->size;
    kv->value = realloc(kv->value, kv->size);

    if (kv->value == NULL)
    {
        free(data->data);
        free(data);
        printf("ERR get_tl_overflow: Failed to reallocate buffer\n");
        return -1;
    }

    if (memcpy((char *)kv->value + kv->size - data->size, data->data, data->size) == NULL)
    {
        free(data->data);
        free(data);
        printf("ERR get_tl_overflow: Failed to copy data\n");
        return -1;
    }

    free(data->data);
    free(data);

    return 0;
}

/**
 * @deprecated
 */
int read_tlcell(struct key_value *kv, void *rpage)
{
    struct tl_cell_header *header = (struct tl_cell_header *)rpage;

    kv->key = header->key;

    kv->size = header->size;

    kv->value = malloc(header->size);

    if (kv->value == NULL)
    {
        printf("ERR read_tlcell: Failed to allocate buffer\n");
        return -1;
    }

    if (memcpy(kv->value, (char *)rpage + sizeof(struct tl_cell_header), header->size) == NULL)
    {
        free(kv->value);
        printf("ERR read_tlcell: Failed to copy data\n");
        return -1;
    }

    if (header->overflow != 0)
    {
        if (get_tl_overflow(header->overflow, kv) == -1)
        {
            free(kv->value);
            printf("ERR read_tlcell: Failed to get overflow data\n");
            return -1;
        }
    }

    return 0;
}

struct node *read_node(size_t addr)
{
    struct node *node = (struct node *)malloc(sizeof(struct node));

    struct btree_page page;

    page.data = read_page(&db, addr);

    if (page.data == NULL)
    {
        free(node);
        printf("ERR read_node: Failed to read page\n");
        return NULL;
    }

    page.btree_h = (struct btree_header *)page.data;

    node->parent_addr = page.btree_h->parent_addr;
    node->type = page.btree_h->page_type;
    node->nb_keys = page.btree_h->nb_cells;

    void *cell_ptr = (char *)page.data + sizeof(struct btree_header);

    switch (page.btree_h->page_type)
    {
    case NODE_TYPE_TABLE_LEAF:
        node->child_addrs[0] = page.btree_h->right_pointer;

        for (size_t i = 0; i < page.btree_h->nb_cells; i++)
        {
            struct tl_cell_header *tlh = (struct tl_cell_header *)cell_ptr;
            node->key_vals[i] = (struct key_value *)calloc(sizeof(struct key_value), 1);

            node->key_vals[i]->key = tlh->key;
            node->key_vals[i]->size = tlh->size;
            node->key_vals[i]->value = calloc(tlh->size, 1);
            node->key_vals[i]->overflow = NULL;

            cell_ptr = (char *)cell_ptr + sizeof(struct tl_cell_header);
            node->key_vals[i]->value = memcpy(node->key_vals[i]->value, cell_ptr, tlh->size);

            if (tlh->overflow != 0)
            {
                if (get_tl_overflow(tlh->overflow, node->key_vals[i]) == -1)
                {
                    printf("ERR read_node: Failed to get overflow data\n");
                    return NULL;
                }
            }

            cell_ptr = (char *)cell_ptr + tlh->size;

            assert((char *)cell_ptr - (char *)page.data < db.header->page_size);
        }

        break;

    case NODE_TYPE_INDEX_LEAF:
    case NODE_TYPE_INDEX_INTERIOR:
    case NODE_TYPE_TABLE_INTERIOR:
        node->child_addrs[node->nb_keys] = page.btree_h->right_pointer;

        for (size_t i = 0; i < page.btree_h->nb_cells; i++)
        {
            struct tl_cell_header *tlh = (struct tl_cell_header *)cell_ptr;
            node->key_vals[i] = (struct key_value *)calloc(sizeof(struct key_value), 1);

            node->key_vals[i]->key = tlh->key;
            node->key_vals[i]->size = 0;
            node->key_vals[i]->value = NULL;
            node->key_vals[i]->overflow = NULL;

            cell_ptr = (char *)cell_ptr + sizeof(struct tl_cell_header);
            node->child_addrs[i] = *(Page *)cell_ptr;

            cell_ptr = (char *)cell_ptr + tlh->size;

            assert((char *)cell_ptr - (char *)page.data < db.header->page_size);
        }
        break;

    default:
        printf("ERR read_node: Unknown node type\n");
        printf("%zu\n", page.btree_h->page_type);
        printf("addr: %zu\n", addr);
        return NULL;
    }

    free(page.data);

    return node;
}

// TODO: Implement Cache
/**
 * @brief Saves the header of the db.
 */
int save_header()
{
    size_t size = sizeof(struct yacdb_header);

    if (fseek(db.file, 0, SEEK_SET) == -1)
    {
        printf("ERR save_header: Failed to seek\n");
        return -1;
    }

    if (fwrite(db.header, size, 1, db.file) != 1)
    {
        printf("ERR write_at: Failed to write\n");
        return -1;
    }

    return 0;
}

/**
 * TODO: Cache optimization
 * @brief Finds the first available page in the file. If there is a free page available it is removed from the free page list and returned. The necessary changes will be made to the file.
 * @return the page number or -1 if an error occurred.
 */
Page create_addr()
{
    Page page;

    // Is a free page available?
    if (db.header->nb_free_pages > 0)
    {
        // If so, we can return it
        page = db.header->fst_free_page;

        // Set the first free page to the next free page
        // Reading the page is OK as it will probably need to be in cache
        void *rpage = read_page(&db, page);

        if (rpage == NULL)
        {
            printf("ERR create_addr: Failed to read page\n");
            return -1;
        }

        int *buf = (int *)rpage;

        db.header->fst_free_page = *buf;
        free(buf);

        // Remove the page from the free page list
        db.header->nb_free_pages--;
    }
    else
    {
        // If not, we need to allocate a new page
        page = db.header->nb_pages++;

        // TODO: CACHE

        if (ftruncate(fileno(db.file), db.header->page_size * db.header->nb_pages) == -1)
        {
            printf("ERR find_page: Failed to resize file\n");
            return -1;
        }
    }

    // Save the changes
    // TODO: CACHE
    if (save_header() == -1)
    {
        printf("ERR find_page: Failed to save header\n");
        return -1;
    }

    printf("INFO find_page: Page %zu created\n", page);

    return page;
}

/**
 * @brief Writes a page to the file.
 * @param rpage The page to write. This needs to be 512bytes long.
 */
// TODO: Cache
int write_page(size_t addr, void *rpage)
{

    size_t offset = addr * db.header->page_size;
    size_t size = db.header->page_size;

    if (addr == 0)
    {
        offset += sizeof(struct yacdb_header);
        size -= sizeof(struct yacdb_header);
    }

    if (fseek(db.file, offset, SEEK_SET) == -1)
    {
        printf("ERR write_page: Failed to seek\n");
        return -1;
    }

    if (fwrite(rpage, size, 1, db.file) != 1)
    {
        printf("ERR write_page: Failed to write\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Frees a page by adding it to the free page list.
 */
int free_page(size_t addr)
{
    // Simply change the current page to a free page
    db.header->nb_free_pages++;

    // link the current page to the first free page
    void *rpage = malloc(db.header->page_size);

    if (rpage == NULL)
    {
        printf("ERR free_page: Failed to allocate memory\n");
        return -1;
    }

    *(int *)rpage = db.header->fst_free_page;

    if (write_page(addr, rpage) == -1)
    {
        printf("ERR remove_node: Failed to write page\n");
        return -1;
    }

    db.header->fst_free_page = addr;

    return 0;
}

int remove_node(size_t addr)
{
    // TODO: Remove overflow pages

    return free_page(addr);
}

/**
 * @brief Frees a list of overflow pages.
 */
int free_linked_overflow_pages(struct linked_overflow *overflow)
{
    if (overflow == NULL)
    {
        return 0;
    }

    if (free_linked_overflow_pages(overflow->next) == -1)
    {
        printf("ERR free_linked_overflow_pages: Failed to free next overflow\n");
        return -1;
    }

    if (free_page(overflow->page) == -1)
    {
        printf("ERR free_linked_overflow_pages: Failed to remove node\n");
        return -1;
    }

    free(overflow);
    overflow = NULL;

    return 0;
}

int write_overflow(void *value, size_t size, struct linked_overflow **overflow_ptr)
{
    if (*overflow_ptr == NULL)
    {
        *overflow_ptr = (struct linked_overflow *)malloc(sizeof(struct linked_overflow));

        if (*overflow_ptr == NULL)
        {
            printf("ERR write_overflow: Failed to allocate memory\n");
            return -1;
        }

        (*overflow_ptr)->next = NULL;
        (*overflow_ptr)->page = create_addr();
    }

    struct linked_overflow *overflow = *overflow_ptr;

    void *rpage = malloc(db.header->page_size);

    if (rpage == NULL)
    {
        printf("ERR write_overflow: Failed to allocate memory\n");
        return -1;
    }

    size_t actual_size = db.header->page_size - sizeof(Page);

    if (size > actual_size)
    {
        // use a new overflow page
        write_overflow((char *)value + actual_size, size - actual_size, &(overflow->next));

        *(Page *)rpage = overflow->next->page;
    }
    else
    {
        // remove all the overflow pages
        if (free_linked_overflow_pages(overflow->next) == -1)
        {
            printf("ERR write_overflow: Failed to free overflow pages\n");
            return -1;
        }

        *(Page *)rpage = 0;
    }

    if (memcpy((char *)rpage + sizeof(Page), value, actual_size) != (char *)rpage + sizeof(Page))
    {
        printf("ERR write_overflow: Failed to copy data\n");
        return -1;
    }

    if (write_page(overflow->page, rpage) == -1)
    {
        printf("ERR write_overflow: Failed to write page\n");
        return -1;
    }

    free(rpage);

    return 0;
}

int write_tl_node(struct node *node, size_t addr)
{
    // calculate the size of the page
    size_t min_size = sizeof(struct btree_header)                      // the header
                      + sizeof(struct tl_cell_header) * node->nb_keys; // the key values

    size_t page_size = db.header->page_size;

    if (addr == 0)
    {
        page_size -= sizeof(struct yacdb_header);
    }

    if (min_size > db.header->page_size)
    {
        printf("ERR write_tl_node: Node is too big\n");
        return -1;
    }

    size_t available_size = page_size - min_size;

    void *rpage = calloc(page_size, 1);

    ((struct btree_header *)rpage)->parent_addr = node->parent_addr;
    ((struct btree_header *)rpage)->page_type = NODE_TYPE_TABLE_LEAF;
    ((struct btree_header *)rpage)->nb_cells = node->nb_keys;
    ((struct btree_header *)rpage)->right_pointer = node->child_addrs[0];

    // char* is used to do pointer arithmetic
    void *cell_ptr = (char *)rpage + sizeof(struct btree_header);

    for (size_t i = 0; i < node->nb_keys; i++)
    {
        struct tl_cell_header *cell = (struct tl_cell_header *)cell_ptr;

        cell->key = node->key_vals[i]->key;
        cell->size = node->key_vals[i]->size;

        if (cell->size > available_size)
        {
            cell->size = available_size;

            write_overflow((char *)node->key_vals[i]->value + cell->size, node->key_vals[i]->size - cell->size, &(node->key_vals[i]->overflow));
        }
        else
        {
            // remove all the overflow pages
            if (free_linked_overflow_pages(node->key_vals[i]->overflow) == -1)
            {
                printf("ERR write_overflow: Failed to free overflow pages\n");
                return -1;
            }

            cell->overflow = 0;
        }

        available_size -= cell->size;

        cell_ptr = (char *)cell_ptr + sizeof(struct tl_cell_header);

        if (cell->size > 0)
        {
            memcpy(cell_ptr, node->key_vals[i]->value, cell->size);
            cell_ptr = (char *)cell_ptr + cell->size;
        }
    }

    if (write_page(addr, rpage) == -1)
    {
        printf("ERR write_tl_node: Failed to write page\n");
        return -1;
    }

    free(rpage);

    return 0;
}

int write_ti_node(struct node *node, size_t addr)
{
    // calculate the size of the page
    size_t min_size = sizeof(struct btree_header)                      // the header
                      + sizeof(struct tl_cell_header) * node->nb_keys; // the key values

    size_t page_size = db.header->page_size;

    if (addr == 0)
    {
        page_size -= sizeof(struct yacdb_header);
    }

    if (min_size > db.header->page_size)
    {
        printf("ERR write_tl_node: Node is too big\n");
        return -1;
    }

    size_t available_size = page_size - min_size;

    void *rpage = calloc(page_size, 1);

    ((struct btree_header *)rpage)->parent_addr = node->parent_addr;
    ((struct btree_header *)rpage)->page_type = node->type;
    ((struct btree_header *)rpage)->nb_cells = node->nb_keys;
    ((struct btree_header *)rpage)->right_pointer = node->child_addrs[node->nb_keys];

    // char* is used to do pointer arithmetic
    void *cell_ptr = (char *)rpage + sizeof(struct btree_header);

    for (size_t i = 0; i < node->nb_keys; i++)
    {
        struct tl_cell_header *cell = (struct tl_cell_header *)cell_ptr;

        cell->key = node->key_vals[i]->key;
        cell->size = sizeof(Page);

        available_size -= cell->size;

        cell_ptr = (char *)cell_ptr + sizeof(struct tl_cell_header);

        Page *page_ptr = (Page *)cell_ptr;

        *page_ptr = node->child_addrs[i];

        cell_ptr = (char *)cell_ptr + sizeof(Page);
    }

    if (write_page(addr, rpage) == -1)
    {
        printf("ERR write_tl_node: Failed to write page\n");
        return -1;
    }

    free(rpage);

    return 0;
}

int write_node(struct node *node, size_t addr)
{
    switch (node->type)
    {
    case NODE_TYPE_INDEX_LEAF:
    case NODE_TYPE_INDEX_INTERIOR:
    case NODE_TYPE_TABLE_INTERIOR:
        return write_ti_node(node, addr);
    case NODE_TYPE_TABLE_LEAF:
        return write_tl_node(node, addr);
    default:
        printf("ERR write_node: Unknown node type\n");
        return -1;
    }
}

int init_db()
{
    FILE *file;
    FILE *temp;

    // Checks if the database file exists
    if ((file = fopen(YACDB_FILE_NAME, "r+")))
    {
        db.file = file;

        // Reads the header
        db.header = (struct yacdb_header *)malloc(sizeof(struct yacdb_header));

        if (db.header == NULL)
        {
            printf("ERR init_db: Failed to allocate memory\n");
            return -1;
        }
        if (fseek(db.file, 0, SEEK_SET))
        {
            printf("ERR init_db: Failed to seek\n");
            return -1;
        };

        if (fread(db.header, sizeof(struct yacdb_header), 1, db.file) != 1)
        {
            printf("ERR init_db: Failed to read header\n");
            return -1;
        }
    }
    else
    {
        // Creates the database file
        if (!(file = fopen(YACDB_FILE_NAME, "w+")))
        {
            printf("ERR init_db: Failed to create database file\n");
            return -1;
        }

        if (fclose(file))
        {
            printf("ERR init_db: Failed to close file\n");
            return -1;
        }

        if (!(file = fopen(YACDB_FILE_NAME, "r+")))
        {
            printf("ERR init_db: Failed to open file\n");
            return -1;
        }

        db.header = (struct yacdb_header *)malloc(sizeof(struct yacdb_header));

        if (db.header == NULL)
        {
            printf("ERR init_db: Failed to allocate memory\n");
            return -1;
        }

        db.file = file;

        strcpy(db.header->signature, YACDB_SIGNATURE);

        db.header->page_size = YACDB_PAGE_SIZE;

        db.header->change_counter = 0;

        db.header->nb_pages = 1;
        db.header->default_cache_size = YACDB_DEFAULT_CACHE_SIZE;

        db.header->nb_free_pages = 0;
        db.header->fst_free_page = 0;

        // Resize the file
        if (ftruncate(fileno(db.file), db.header->page_size * db.header->nb_pages) == -1)
        {
            printf("ERR find_page: Failed to resize file\n");
            return -1;
        }

        save_header();
    }

    return 0;
}

struct node *create_node()
{
    struct node *node = (struct node *)malloc(sizeof(struct node));

    if (node == NULL)
    {
        printf("ERR create_node: Failed to allocate memory\n");
        return NULL;
    }

    node->parent_addr = (Page)-1;
    node->type = NODE_TYPE_TABLE_LEAF;
    node->nb_keys = 0;

    // The right pointer is set to -1 this is the rightemost child
    node->child_addrs[0] = (Page)-1;

    return node;
}