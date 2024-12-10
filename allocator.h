#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h> //pour size_t
#include <pthread.h>

#define HEADER_SIZE sizeof(BlockHeader)
#define NUM_CLASSES 10
#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 1024
#define ALIGNMENT 16



#define NUM_THREADS 5

typedef struct BlockHeader {
    size_t size;                //taille du bloc
    struct BlockHeader* next;   //pointeur vers le prochain bloc libre
} BlockHeader;

// traquer l'allocation des pointeurs pour éviter les memoery leaks
typedef struct {
    void* ptr;
    size_t size;
} AllocRecord;

// typedef struct {
//     size_t size;      // Size for the memory allocation
//     long thread_id;   // Thread ID (or any other data you want to pass)
// } ThreadData;

typedef struct {
    size_t size;
    void* (*alloc_func)(size_t, int);
    void (*free_func)(void*, int);
    int verbose;
    int num_allocations;
} ThreadData;




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
void* thread_function(void *arg);



double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t,int), void (*free_func)(void*, int), int verbose);
double measure_allocations_default(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void*));
double measure_allocations_thread(int num_threads, int num_allocations, size_t size, void* (*alloc_func)(size_t, int), void (*free_func)(void*, int), int verbose);


#endif