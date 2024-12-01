#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h> //pour size_t

#define HEADER_SIZE sizeof(BlockHeader)
#define NUM_CLASSES 10
#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 1024


typedef struct BlockHeader {
    size_t size;                //taille du bloc
    struct BlockHeader* next;   //pointeur vers le prochain bloc libre
} BlockHeader;

int get_class_index(size_t size, size_t* class_size);

void* my_malloc(size_t size);
void* my_malloc_simple(size_t size);

void my_free(void* ptr);

double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void*));



#endif