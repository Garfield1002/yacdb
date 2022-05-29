# `yac.db` File Specification

## Pages

The main database file consists of one or more pages. The size of a page is a power of two between 512 and 32768 inclusive. All pages within the same database are the same size. The page size for a database file is determined by the 2-byte integer located at an offset of 13 bytes from the beginning of the database file.

Pages are numbered beginning with 1. The maximum page number is 4294967294 (232 - 2). The minimum size yacDB database is a single 512-byte page. The maximum size database would be 2147483646 pages at 65536 bytes per page or 281,474,976,579,584 bytes (about 281 terabytes). Usually yacDB will hit the maximum file size limit of the underlying filesystem or disk hardware long before it hits its own internal size limit.

At any point in time, every page in the main database has a single use which is one of the following:

- A freelist page
- A b-tree page
  - A table b-tree interior page
  - A table b-tree leaf page
  - An index b-tree interior page
  - An index b-tree leaf page
- A payload overflow page

All reads from and writes to the main database file begin at a page boundary and all writes are an integer number of pages in size. There is one exception to this rule: [the database header](#the-database-header).

Before any information-bearing page of the database is modified, the original unmodified content of that page is written into the rollback journal. If a transaction is interrupted and needs to be rolled back, the rollback journal can then be used to restore the database to its original state. Freelist leaf pages bear no information that would need to be restored on a rollback and so they are not written to the journal prior to modification, in order to reduce disk I/O.

## Database Header

| Offset | Size | Description                                                                              |
| ------ | ---- | ---------------------------------------------------------------------------------------- |
| 0      | 13   | The header string: "YACDB format\0"                                                      |
| 13     | 2    | The database page size in bytes. Must be a power of two between 512 and 32768 inclusive. |
| 15     | 4    | File change counter. (Currently unused)                                                  |
| 19     | 4    | Size of the database file in pages. The "in-header database size".                       |
| 23     | 4    | Default page cache size.                                                                 |
| 27     | 4    | Number of free blocks.                                                                   |
| 31     | 4    | First free block                                                                         |

### Magic Header string

Every valid YACDB file begins with the following 13 bytes (in hex): `59 41 43 44 42 20 66 6F 72 6D 61 74 00`. This byte sequence corresponds to the ASCII string "YACDB format" including the nul terminator character at the end.

### Page Size

The two-byte value beginning at offset 13 determines the page size of the database. This value is interpreted as a big-endian integer and must be a power of two between 512 and 32768, inclusive.

### File change counter (Unused)

A planned space for checking if cached pages are still valid.

### In-header database size

The 4-byte big-endian integer at offset 20 into the header stores the size of the database file in pages.

### Suggested cache size

The 4-byte big-endian signed integer at offset 24 is the suggested cache size in pages for the database file.

### Number of free blocks

The 4-byte big-endian signed integer at offset 28 is the current number of free blocks in the database.

### First free block

The 4-byte big-endian signed integer at offset 32 is the address of the first free blocks in the database.

## The Freelist

A database file might contain one or more pages that are not in active use. Unused pages can come about, for example, when information is deleted from the database. Unused pages are stored on the freelist and are reused when additional pages are required.

The freelist is organized as a linked list.

⚠ Unlike in SQlite, the free list is organized as a linked list.
