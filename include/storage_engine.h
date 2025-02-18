#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include<stdint.h>

#define PAGE_SIZE   4096
#define FILENAME    "database.db"

typedef struct
{
    uint8_t data[PAGE_SIZE];
} Page;

int open_database();
int close_database();
int read_page(int page_number, Page *page);
int write_page(int page_number, const Page *page);

#endif