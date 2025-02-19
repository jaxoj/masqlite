#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include<stdint.h>

#define PAGE_SIZE   4096
#define FILENAME    "database.db"
#define MAX_PAGES   1000
#define MAX_CACHE   10

// Page is the representative format of the stored data inside the the datafile.
typedef struct Page
{
    uint8_t data[PAGE_SIZE];
} Page;

// FeeList is where we keep track of the available pages in the datafile.
typedef struct FreeList
{
    int list[MAX_PAGES];
    int count;
} FreeList;

typedef struct CacheEntry {
    Page page;
    int page_number;
} CacheEntry;

// it opens the datafile or create it if it doesn't exist and returns 0
// if the file is already open or something happened during the process it returns -1
int open_database();

// closing the datafile and freeing the variable that holds it.
// if it's already closed if returns -1.
int close_database();

// reads `PAGE_SIZE` of bytes from the datafile starting from the position `page_number`.
int read_page(int page_number, Page *page);

// writes `PAGE_SIZE` of bytes into the datafile starting from position `page_number`.
int write_page(int page_number, const Page *page);

// write the page in the appropriate free space inside the datafile.
// @return `page_number`
int allocate_page(Page *page);

// freeing the page in the position `page_number`.
int free_page(int page_number);

// add the read page to the cache memory
int read_page_with_cache(int page_number, Page *page);

// if the page exists in the cache memory we update it with new page and write the changes to the datafile.
int write_page_with_cache(int page_number, const Page *page);

#endif