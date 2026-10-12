#ifndef MEMORY_H
#define MEMORY_H

#define PAGE_SIZE 4096UL

void memory_init(void);

void *page_alloc(void);
void page_free(void *page);

unsigned long memory_total_pages(void);
unsigned long memory_free_pages(void);
unsigned long memory_used_pages(void);

#endif
