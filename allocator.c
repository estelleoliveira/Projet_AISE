#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>

#include "allocator.h"
//liste globale des blocs libres classés par tailles
BlockHeader* free_lists[NUM_CLASSES] = {NULL};
//trouver la classe de taille pour une taille donnée
int get_class_index(size_t size, size_t* class_size) {
    int index = 0;
    size_t current_size = MIN_BLOCK_SIZE;
    while (size > current_size && index < NUM_CLASSES - 1) {
        current_size *= 2;
        index++;
    }
    if (class_size) {
        *class_size = current_size; // Taille finale de la classe
    }
    return index;
}

/*Goulots d'étranglements:

Chaque my_alloc() demande un changement de l'espace d'adressage 
Les petites allocations fragmentent la mémoire avec des segments disjoints
Pas de réutilisation de blocs, chaque appel créer un nouveau bloc
Verification des erreurs à chaque appel même si les erreurs sont rares

*/

void recycle_block(BlockHeader* block){
    int class_index;
    size_t class_size;
    class_index = get_class_index(block->size - HEADER_SIZE, &class_size);
    block->next =free_lists[class_index];
    free_lists[class_index] =block;
}
//recuperer dans la liste de blocs libres pour une taille donnée
BlockHeader* get_free_block(size_t size){
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);

    BlockHeader* block = free_lists[class_index];
    if (block){
        free_lists[class_index] = block->next;
        return block;
    }
    return NULL;
}
//recherche le meilleur bloc libre 
BlockHeader* get_best_fit_block(size_t size) {
    // printf("Getting best fit block...");
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);

    BlockHeader* best_fit = NULL;
    BlockHeader* prev = NULL;
    BlockHeader* current = free_lists[class_index];

    // Parcourir la liste des blocs libres et trouver le meilleur bloc
    while (current) {
        if (current->size >= size && (!best_fit || current->size < best_fit->size)) {
            best_fit = current;
        }
        current = current->next;
    }

    if (best_fit) {
        // Retirer le bloc de la liste des blocs libres
        if (prev) {
            prev->next = best_fit->next;
        } else {
            free_lists[class_index] = best_fit->next;
        }
    }
    return best_fit;
}


// void* my_malloc(size_t size) {
//     size_t total_size = size + HEADER_SIZE;
//     BlockHeader* header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);

//     if (header == MAP_FAILED) {
//         perror("mmap failed");
//         return NULL;
//     }

//     header->size = total_size;
//     return (void*)(header + 1);
// }

//Version  non optimisé avec recherche de bloc libres

void* my_malloc(size_t size) {
    

    size_t total_size = size + HEADER_SIZE;
    //On cherche d'abord un bloc disponible ds le cache
    BlockHeader* header = get_best_fit_block(size);

    if (!header){
        header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
        //vérifie si mmap a réussi
        if (header == MAP_FAILED) {
            perror("mmap failed");
            return NULL;
        }
    }
    header->size = total_size;
    return (void*)(header + 1);
    
}

//optimisation pour fusionner avec le bloc suivant si il est libre

void coalesce_blocks(BlockHeader* block){
    if (block->next && (char*)block + block->size == (char*)block->next){
        block->size += block->next->size;
        block->next = block->next->next;
    }
}


void my_free(void* ptr) {
    if (ptr==NULL) return;
    
    BlockHeader* header = (BlockHeader*)ptr - 1;
    coalesce_blocks(header);
    recycle_block(header);
    if (munmap(header, header->size) == -1) {
        perror("munmap failed");
    }
}

// Fonction pour mesurer le temps d'exécution d'une série d'allocations et de libérations
double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void*)) {
    struct timeval start, end;
    void* ptrs[num_allocations];

    // Démarre le chronomètre
    gettimeofday(&start, NULL);

    // Allocation
    for (int i = 0; i < num_allocations; i++) {
       
        ptrs[i] = alloc_func(size);
        if (ptrs[i] == NULL) {
            perror("Allocation failed");
            return -1.0;
        }
    }

    // Libération
    for (int i = 0; i < num_allocations; i++) {
        free_func(ptrs[i]);
    }

    // Arrête le chronomètre
    gettimeofday(&end, NULL);

    // Calcul du temps écoulé en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    return elapsed_time;
}