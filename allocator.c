#include <sys/mman.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>

#include "allocator.h"
//liste globale des blocs libres classés par tailles
BlockHeader* free_lists[NUM_CLASSES] = {NULL};
//trouver la classe de taille pour une taille donnée
int get_class_index(size_t size, size_t* class_size) {
    int index = 0;
    size_t current_size = 16;
    while (size > current_size && index < NUM_CLASSES - 1) {
        current_size *= 2;
        index++;
    }
    if (class_size) {
        *class_size = current_size; // Taille finale de la classe
    }
    return index;
}

void* my_malloc(size_t size) {
    size_t total_size = size + HEADER_SIZE;;
    BlockHeader* header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);

    //vérifie si mmap a réussi
    if (header == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }

    header->size = total_size;
    return (void*)(header + 1);
}

void my_free(void* ptr) {
    
    BlockHeader* header = (BlockHeader*)ptr - 1;
   
    if (munmap(header, header->size) == -1) {
        perror("munmap failed");
    }
}