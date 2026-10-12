#include "memory.h"

extern char __kernel_end[];

#define MEMORY_LIMIT 0x42000000UL

typedef struct free_page {
    struct free_page *next;
} free_page_t;

static free_page_t *free_list = 0;

static unsigned long total_pages = 0;
static unsigned long free_pages = 0;

static unsigned long heap_start_addr = 0;
static unsigned long heap_end_addr = 0;

static unsigned long align_up(unsigned long value,
                              unsigned long alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

void memory_init(void)
{
    unsigned long start =
        align_up((unsigned long)__kernel_end, PAGE_SIZE);

 /*
     * Reserve first 1 MiB after the kernel for kmalloc heap.
     */
    heap_start_addr = start;
    heap_end_addr =
        align_up(heap_start_addr + KERNEL_HEAP_SIZE, PAGE_SIZE);

    /*
     * Physical page allocator starts after the heap.
     */
    unsigned long page_start = heap_end_addr;

    free_list = 0;
    total_pages = 0;
    free_pages = 0;

    for (unsigned long addr = start;
         addr + PAGE_SIZE <= MEMORY_LIMIT;
         addr += PAGE_SIZE)
    {
        free_page_t *page = (free_page_t *)addr;

        page->next = free_list;
        free_list = page;

        total_pages++;
        free_pages++;
    }
}

void *page_alloc(void)
{
    if (free_list == 0)
        return 0;

    free_page_t *page = free_list;
    free_list = page->next;

    free_pages--;

    /*
     * Clear the allocated page.
     */
    unsigned char *ptr = (unsigned char *)page;

    for (unsigned long i = 0; i < PAGE_SIZE; i++) {
        ptr[i] = 0;
    }

    return page;
}

void page_free(void *address)
{
    if (address == 0)
        return;

    free_page_t *page = (free_page_t *)address;

    page->next = free_list;
    free_list = page;

    free_pages++;
}

unsigned long memory_total_pages(void)
{
    return total_pages;
}

unsigned long memory_free_pages(void)
{
    return free_pages;
}
	
unsigned long memory_used_pages(void)
{
    return total_pages - free_pages;
}
unsigned long memory_heap_start(void)
{
    return heap_start_addr;
}

unsigned long memory_heap_end(void)
{
    return heap_end_addr;
}
