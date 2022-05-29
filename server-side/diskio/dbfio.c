#include "include/dbfio.h"

/**
 * @brief Moves in the database file to a given offset.
 */
int set_cursor(struct yacdb *db, size_t offset)
{
    if (offset >= db->header->page_size * db->header->nb_pages)
    {
        printf("ERR set_cursor: Offset out of range\n");
        return -1;
    }

    if (fseek(db->file, offset, SEEK_SET))
    {
        printf("ERR set_cursor: Failed to seek\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Reads `size` bytes from the db at the given offset.
 * If an error occurs, the function returns NULL.
 */
void *read_at(struct yacdb *db, size_t offset, size_t size)
{
    void *buf = malloc(size);

    if (buf == NULL)
    {
        printf("ERR read_at: Failed to allocate buffer\n");
        return NULL;
    }

    if (set_cursor(db, offset) == -1)
    {
        free(buf);
        printf("ERR read_at: Failed to set cursor\n");
        return NULL;
    }

    if (read(db->file, buf, size) != size)
    {
        free(buf);
        printf("ERR read_at: Failed to read\n");
        return NULL;
    }

    return buf;
}

/**
 * @brief Move the file cursor to the kth cell of the page.
 */
void *read_k_cell(struct yacdb *db, struct btree_page *page, int k, size_t size)
{
    if (k < 0 || k >= page->btree_h->nb_cells)
        return -1;

    // Retrieve the offset of the key
    int offset = sizeof(struct btree_header) + k * sizeof(int);

    if (page->page == 0)
        // If we are on the first page there is also the yacdb header
        offset += sizeof(struct yacdb_header);
    else
        // Otherwise, move to the beginning of the page
        offset += db->header->page_size * page->page;

    return read_at(db, offset, size);
}

/**
 * @brief Gets the kth cell of a Table Interior Node page.
 *
 * TODO: Add support for overflow pages.
 */
struct ti_cell *get_k_ti_cell(struct yacdb *db, struct btree_page *page, int k)
{
    return read_k_cell(db, page, k, sizeof(struct ti_cell));
}

/**
 * @brief Gets the kth cell header of a Table Leaf Node page.
 * This method is usefull if we want to get the key of a cell and not load the whole cell.
 */
struct tl_cell_header *get_k_tl_cell_h(struct yacdb *db, struct btree_page *page, int k)
{
    return read_k_cell(db, page, k, sizeof(struct tl_cell_header));
}

/**
 * @brief Gets data out of overflow pages recursively.
 */

struct overflow_data *get_overflow_data(struct yacdb *db, Page page)
{
    if (page == 0)
        return NULL;

    struct overflow_header *header = read_at(db, db->header->page_size * page, sizeof(struct overflow_header));

    if (header == NULL)
    {
        printf("ERR get_overflow_data: Failed to read header\n");
        return NULL;
    }

    struct overflow_data *data = malloc(sizeof(struct overflow_data));

    if (data == NULL)
    {
        printf("ERR get_overflow_data: Failed to allocate data\n");
        return NULL;
    }

    data->size = header->size;
    data->data = malloc(header->size);

    if (data->data == NULL)
    {
        printf("ERR get_overflow_data: Failed to allocate data\n");
        return NULL;
    }

    if (read(db->file, data->data, header->size) != header->size)
    {
        printf("ERR get_overflow_data: Failed to read data\n");
        return NULL;
    }

    if (header->next_page != 0)
    {
        struct overflow_data *next = get_overflow_data(db, header->next_page);

        if (next == NULL)
        {
            printf("ERR get_overflow_data: Failed to get next overflow page\n");
            return NULL;
        }

        data->size += next->size;

        data->data = realloc(data->data, data->size + next->size);

        if (memcpy(data->data + data->size, next->data, next->size))
        {
            printf("ERR get_overflow_data: Failed to copy next overflow page\n");
            return NULL;
        }

        free(next->data);
        free(next);
    }

    return data;
}

/**
 * @brief Gets the kth cell of a Table Leaf Node page, reconstructs the data if an overflow occurred.
 */
struct tl_cell *get_k_tl_cell(struct yacdb *db, struct btree_page *page, int k)
{
    struct tl_cell_header *header = get_k_tl_cell_h(db, page, k);
    if (header == NULL)
        return NULL;

    struct tl_cell *cell = malloc(sizeof(struct tl_cell));
    if (cell == NULL)
    {
        printf("ERR get_k_tl_cell: Failed to allocate cell\n");
        return NULL;
    }

    cell->header = header;

    void *buf = malloc(header->size);
    if (buf == NULL)
    {
        printf("ERR get_k_tl_cell: Failed to allocate buffer\n");
        free(cell);
        free(header);
        return NULL;
    }

    // reads the data, the file cursor is at the end of the cell
    if (read(db->file, buf, header->size) != header->size)
    {
        free(buf);
        free(cell);
        free(header);
        printf("ERR read_at: Failed to read\n");
        return NULL;
    }

    // If the cell is not overflow, we can return the cell
    if (header->overflow == 0)
    {
        cell->data = buf;
        return cell;
    }

    // Otherwise, we need to reconstruct the cell
}

/**
 * @brief Writes `size` bytes to the db at the given offset.
 * If an error occurs, the function returns -1.
 */
int write_at(struct yacdb *db, size_t offset, void *buf, size_t size)
{
    if (set_cursor(db, offset) == -1)
    {
        printf("ERR write_at: Failed to set cursor\n");
        return -1;
    }

    if (write(db->file, buf, size) != size)
    {
        printf("ERR write_at: Failed to write\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Saves the header of the db.
 */
int save_header(struct yacdb *db)
{
    return write_at(db, 0, db->header, sizeof(struct yacdb_header));
}

/**
 * @brief Finds the first available page in the file. If there is a free page available it is removed from the free page list and returned. The necessary changes will be made to the file.
 * @return the page number or -1 if an error occurred.
 */
int find_empty_page(struct yacdb *db)
{
    // Is a free page available?
    if (db->header->nb_free_pages > 0)
    {
        // If so, we can return it
        int page = db->header->fst_free_page;

        // Set the first free page to the next free page
        int *buf = (int *)read_at(db, db->header->page_size * db->header->fst_free_page, sizeof(int));

        if (buf == NULL)
        {
            printf("ERR find_page: Failed to read\n");
            return -1;
        }

        db->header->fst_free_page = *buf;
        free(buf);

        // Remove the page from the free page list
        db->header->nb_free_pages--;

        // Save the changes
        if (save_header(db) == -1)
        {
            printf("ERR find_page: Failed to save header\n");
            return -1;
        }

        return page;
    }

    // If not, we need to allocate a new page
    int page = ++db->header->nb_pages;

    // Resize the file
    if (ftruncate(db->file, db->header->page_size * db->header->nb_pages) == -1)
    {
        printf("ERR find_page: Failed to resize file\n");
        return -1;
    }

    // Save the changes
    if (save_header(db) == -1)
    {
        printf("ERR find_page: Failed to save header\n");
        return -1;
    }

    return page;
}

// TODO: FINISH
int write_page(struct yacdb *db, struct btree_page *page)
{
    if (write_at(db, db->header->page_size * page->page, page, db->header->page_size) == -1)
    {
        printf("ERR write_page: Failed to write page\n");
        return -1;
    }

    return 0;
}
