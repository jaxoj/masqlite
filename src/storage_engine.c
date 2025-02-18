
#include "storage_engine.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *db_file = NULL;

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
    if (db_file == NULL) {
        return -1;
    }
    fseek(db_file, page_number * PAGE_SIZE, SEEK_SET);
    fwrite(page->data, PAGE_SIZE, 1, db_file);
    return 0;
}