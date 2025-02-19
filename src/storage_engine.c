
#include "storage_engine.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *db_file = NULL;
static FreeList free_list = {{0}, 0};
static CacheEntry cache[MAX_CACHE];
static int cache_count = 0;

int open_database()
{
    db_file = fopen(FILENAME, "r+b");
    if (db_file == NULL)
    {
        db_file = fopen(FILENAME, "w+b");
        if (db_file == NULL)
        {
            return -1;
        }
    }

    // TODO: here we assume that all pages are free in the start of the server.
    //       we need to load the current state of the pages into the free_list.
    for (int i = 0; i < MAX_PAGES; i++)
    {
        free_list.list[i] = i;
    }
    free_list.count = MAX_PAGES;
    return 0;
}

int close_database()
{
    if (db_file != NULL)
    {
        fclose(db_file);
        db_file = NULL;
        return 0;
    }
    return -1;
}

int read_page(int page_number, Page *page)
{
    if (db_file == NULL)
    {
        return -1;
    }
    fseek(db_file, page_number * PAGE_SIZE, SEEK_SET);
    fread(page->data, PAGE_SIZE, 1, db_file);
    return 0;
}

int write_page(int page_number, const Page *page)
{
    if (db_file == NULL)
    {
        return -1;
    }
    fseek(db_file, page_number * PAGE_SIZE, SEEK_SET);
    fwrite(page->data, PAGE_SIZE, 1, db_file);
    return 0;
}

int allocate_page(Page *page)
{
    if (free_list.count == 0)
    {
        return -1; // no free pages
    }
    int page_number = free_list.list[--free_list.count];
    write_page(page_number, page);
    return page_number;
}

int free_page(int page_number)
{
    if (free_list.count == MAX_PAGES)
    {
        return -1; // all pages are free.
    }
    free_list.list[free_list.count++] = page_number;
    return 0;
}

int read_page_with_cache(int page_number, Page *page)
{
    if (cache_count > 0)
    {
        for (int i = 0; i < cache_count; i++)
        {
            if (cache[i].page_number == page_number)
            {
                *page = cache[i].page;
                return 0;
            }
        }
    }

    int result = read_page(page_number, page);
    if (cache_count < MAX_CACHE)
    {
        cache[cache_count++] = (CacheEntry){*page, page_number};
    }
    return result;
}

int write_page_with_cache(int page_number, const Page *page)
{
    if (cache_count < MAX_CACHE)
    {
        for (int i = 0; i < cache_count; i++)
        {
            if (cache[i].page_number == page_number)
            {
                cache[i].page = *page;
                break;
            }
        }
    }
    return write_page(page_number, page);
}