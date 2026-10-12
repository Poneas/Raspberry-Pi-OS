#include "heap.h"

typedef struct heap_block {
    unsigned long size;
    struct heap_block *next;
    unsigned long free;
    unsigned long reserved;
} heap_block_t;

static heap_block_t *heap_head = 0;

static unsigned long heap_start_addr = 0;
static unsigned long heap_end_addr = 0;

static unsigned long align16(unsigned long value)
{
    return (value + 15UL) & ~15UL;
}

void heap_init(unsigned long start, unsigned long end)
{
    heap_start_addr = start;
    heap_end_addr = end;

    heap_head = (heap_block_t *)start;

    heap_head->size =
        end - start - sizeof(heap_block_t);

    heap_head->next = 0;
    heap_head->free = 1;
    heap_head->reserved = 0;
}

void *kmalloc(unsigned long size)
{
    if (size == 0)
        return 0;

    size = align16(size);

    heap_block_t *block = heap_head;

    while (block != 0) {

        if (block->free && block->size >= size) {

            /*
             * Split the block if enough space remains
             * for another useful block.
             */
            if (block->size >=
                size + sizeof(heap_block_t) + 16)
            {
                heap_block_t *new_block =
                    (heap_block_t *)
                    ((unsigned char *)block
                     + sizeof(heap_block_t)
                     + size);

                new_block->size =
                    block->size
                    - size
                    - sizeof(heap_block_t);

                new_block->next = block->next;
                new_block->free = 1;
                new_block->reserved = 0;

                block->size = size;
                block->next = new_block;
            }

            block->free = 0;

            return
                (unsigned char *)block
                + sizeof(heap_block_t);
        }

        block = block->next;
    }

    return 0;
}

void kfree(void *ptr)
{
    if (ptr == 0)
        return;

    heap_block_t *block =
        (heap_block_t *)
        ((unsigned char *)ptr
         - sizeof(heap_block_t));

    block->free = 1;

    /*
     * Coalesce adjacent free blocks.
     */
    heap_block_t *current = heap_head;

    while (current != 0 && current->next != 0) {

        if (current->free && current->next->free) {
            current->size +=
                sizeof(heap_block_t)
                + current->next->size;

            current->next = current->next->next;
        }
        else {
            current = current->next;
        }
    }
}

unsigned long heap_total_bytes(void)
{
    return heap_end_addr
           - heap_start_addr
           - sizeof(heap_block_t);
}

unsigned long heap_free_bytes(void)
{
    unsigned long total = 0;

    heap_block_t *block = heap_head;

    while (block != 0) {

        if (block->free)
            total += block->size;

        block = block->next;
    }

    return total;
}
