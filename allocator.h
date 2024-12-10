#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h> //pour size_t
#include <pthread.h>

#define HEADER_SIZE sizeof(BlockHeader)
#define NUM_CLASSES 10
#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 1024
#define ALIGNMENT 16

#define SLAB_SIZE_8 64
#define SLAB_SIZE_16 32
#define SLAB_SIZE_32 16
#define SLAB_SIZE_64 8

// Slab to store blocks for small allocations
typedef struct {
    void* slabs[SLAB_SIZE_8];  // Slab for 8-byte allocations
    void* slabs_16[SLAB_SIZE_16];  // Slab for 16-byte allocations
    void* slabs_32[SLAB_SIZE_32];  // Slab for 32-byte allocations
    void* slabs_64[SLAB_SIZE_64];  // Slab for 64-byte allocations
} SlabAllocator;


typedef struct BlockHeader {
    size_t size;                //taille du bloc
    struct BlockHeader* next;   //pointeur vers le prochain bloc libre
} BlockHeader;

// traquer l'allocation des pointeurs pour éviter les memoery leaks
typedef struct {
    void* ptr;
    size_t size;
} AllocRecord;



int get_class_index(size_t size, size_t* class_size);
int recycle_block(BlockHeader* block, int verbose);
void* my_malloc(size_t size, int verbose );
void my_free(void* ptr, int verbose);
void* align_memory(void* ptr, size_t alignment);
void coalesce_blocks(BlockHeader* block);
void track_deallocation(void* ptr);
void track_allocation(void* ptr, size_t size);
void detect_leaks();
void* multithread_malloc(size_t size);



double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t,int), void (*free_func)(void*, int), int verbose);
double measure_allocations_default(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void*));



#endif