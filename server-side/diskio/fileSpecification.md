# The Database file

## Pages

The main database file consists of one or more pages. The size of a page is a power of two between 512 and 32768 inclusive. All pages within the same database are the same size. The page size for a database file is determined by the 2-byte integer located at an offset of 13 bytes from the beginning of the database file.

Pages are numbered beginning with 1. The maximum page number is 4294967294 (232 - 2). The minimum size yacDB database is a single 512-byte page. The maximum size database would be 2147483646 pages at 65536 bytes per page or 281,474,976,579,584 bytes (about 281 terabytes). Usually yacDB will hit the maximum file size limit of the underlying filesystem or disk hardware long before it hits its own internal size limit.

At any point in time, every page in the main database has a single use which is one of the following:

- A freelist page
  - A freelist trunk page
  - A freelist leaf page
- A b-tree page
  - A table b-tree interior page
  - A table b-tree leaf page
  - An index b-tree interior page
  - An index b-tree leaf page
- A payload overflow page
- A pointer map page

All reads from and writes to the main database file begin at a page boundary and all writes are an integer number of pages in size. Reads are also usually an integer number of pages in size, with the one exception that when the database is first opened, the first 100 bytes of the database file (the database file header) are read as a sub-page size unit.

Before any information-bearing page of the database is modified, the original unmodified content of that page is written into the rollback journal. If a transaction is interrupted and needs to be rolled back, the rollback journal can then be used to restore the database to its original state. Freelist leaf pages bear no information that would need to be restored on a rollback and so they are not written to the journal prior to modification, in order to reduce disk I/O.

## The Database header

| Offset | Size | Description                                                                              |
| ------ | ---- | ---------------------------------------------------------------------------------------- |
| 0      | 13   | The header string: "YACDB format\0"                                                      |
| 13     | 2    | The database page size in bytes. Must be a power of two between 512 and 32768 inclusive. |
| 15     | 1    | Bytes of unused "reserved" space at the end of each page. Usually 0.                     |
| 16     | 4    | File change counter.                                                                     |
| 20     | 4    | Size of the database file in pages. The "in-header database size".                       |
| 24     | 4    | Default page cache size.                                                                 |
| 28     | 4    | Number of free blocks.                                                                   |
| 32     | 4    | First free block                                                                         |

### Magic Header string

Every valid YACDB file begins with the following 13 bytes (in hex): 59 41 43 44 42 20 66 6F 72 6D 61 74 00. This byte sequence corresponds to the ASCII string "YACDB format" including the nul terminator character at the end.

### Page Size

The two-byte value beginning at offset 13 determines the page size of the database. This value is interpreted as a big-endian integer and must be a power of two between 512 and 32768, inclusive.

### Reserved bytes per page

YACDB has the ability to set aside a small number of extra bytes at the end of every page for use by extensions. This value is usually 0. The value can be odd. (Currently unused)

The "usable size" of a database page is the page size specified by the 2-byte integer at offset 13 in the header less the "reserved" space size recorded in the 1-byte integer at offset 15 in the header. The usable size of a page might be an odd number. However, the usable size is not allowed to be less than 480. In other words, if the page size is 512, then the reserved space size cannot exceed 32.

### File change counter

The file change counter is a 4-byte big-endian integer at offset 16 that is incremented whenever the database file is unlocked after having been modified. When two or more processes are reading the same database file, each process can detect database changes from other processes by monitoring the change counter. A process will normally want to flush its database page cache when another process modified the database, since the cache has become stale. The file change counter facilitates this.

### In-header database size

The 4-byte big-endian integer at offset 20 into the header stores the size of the database file in pages.

### Suggested cache size

The 4-byte big-endian signed integer at offset 24 is the suggested cache size in pages for the database file.

### Number of free blocks

The 4-byte big-endian signed integer at offset 28 is the current number of free blocks in the database.

### First free block

The 4-byte big-endian signed integer at offset 32 is the address of the first free blocks in the database.

## Rollback Journal

The rollback journal is a file associated with each yacDB file that holds information used to restore the database file to its initial state during the course of a transaction. The rollback journal file is always located in the same directory as the database file and has the same name as the database file but with the string `"-journal"` appended. There can only be a single rollback journal associated with a give database and hence there can only be one write transaction open against a single database at one time.

If a transaction is aborted due to an application crash, an operating system crash, or a hardware power failure or crash, then the database may be left in an inconsistent state. The next time yacDB attempts to open the database file, the presence of the rollback journal file will be detected and the journal will be automatically played back to restore the database to its state at the start of the incomplete transaction.

A valid rollback journal begins with a header in the following format:

TODO

After the header and zero padding are zero or more page records. Each page record stores a copy of the content of a page from the database file before it was changed. The same page may not appear more than once within a single rollback journal. To rollback an incomplete transaction, a process has merely to read the rollback journal from beginning to end and write pages found in the journal back into the database file at the appropriate location.

Let the database page size (the value of the integer at offset 24 in the journal header) be N. Then the format of a page record is as follows:

| Offset | Size | Description                                         |
| ------ | ---- | --------------------------------------------------- |
| 0      | 4    | The page number in the file                         |
| 4      | N    | Original content of the page prior to a transaction |
| N + 4  | 4    | Checksum                                            |

The checksum is an unsigned 32-bit integer computed as follows:

1. Initialize the checksum to the checksum nonce value found in the journal header at offset 12.
2. Initialize index X to be N-200 (where N is the size of a database page in bytes).
3. Interpret the byte at offset X into the page as an 8-bit unsigned integer and add the value of that integer to the checksum.
4. Subtract 200 from X.
5. If X is greater than or equal to zero, go back to step 3.

The checksum value is used to guard against incomplete writes of a journal page record following a power failure. A different random nonce is used each time a transaction is started in order to minimize the risk that unwritten sectors might by chance contain data from the same page that was a part of prior journals. By changing the nonce for each transaction, stale data on disk will still generate an incorrect checksum and be detected with high probability. The checksum only uses a sparse sample of 32-bit words from the data record for performance reasons - design studies during the planning phases of SQLite 3.0.0 showed a significant performance hit in check summing the entire page.

## The Freelist

A database file might contain one or more pages that are not in active use. Unused pages can come about, for example, when information is deleted from the database. Unused pages are stored on the freelist and are reused when additional pages are required.

The freelist is organized as a linked list of freelist trunk pages with each trunk page containing page numbers for zero or more freelist leaf pages.

A freelist trunk page consists of an array of 4-byte big-endian integers. The size of the array is as many integers as will fit in the usable space of a page. The minimum usable space is 480 bytes so the array will always be at least 120 entries in length. The first integer on a freelist trunk page is the page number of the next freelist trunk page in the list or zero if this is the last freelist trunk page. The second integer on a freelist trunk page is the number of leaf page pointers to follow. Call the second integer on a freelist trunk page L. If L is greater than zero then integers with array indexes between 2 and L+1 inclusive contain page numbers for freelist leaf pages.

Freelist leaf pages contain no information.

TODO

The number of freelist pages is stored as a 4-byte big-endian integer in the database header at an offset of 36 from the beginning of the file. The database header also stores the page number of the first freelist trunk page as a 4-byte big-endian integer at an offset of 32 from the beginning of the file.

## The free list

Unlike in SQlite, the free list is organized as a linked list.
