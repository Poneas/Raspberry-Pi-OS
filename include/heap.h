#ifndef HEAP_H
#define HEAP_H

void heap_init(unsigned long start, unsigned long end);

void *kmalloc(unsigned long size);
void kfree(void *ptr);

unsigned long heap_total_bytes(void);
unsigned long heap_free_bytes(void);

#endif
