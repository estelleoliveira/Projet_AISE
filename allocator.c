#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "allocator.h"
//liste globale des blocs libres classés par tailles
BlockHeader* free_lists[NUM_CLASSES] = {NULL};
//trouver la classe de taille pour une taille donnée
// int get_class_index(size_t size, size_t* class_size) {
//     int index = 0;
//     size_t current_size = MIN_BLOCK_SIZE;
//     while (size > current_size && index < NUM_CLASSES - 1) {
//         current_size *= 2;
//         index++;
//     }
//     if (class_size) {
//         *class_size = current_size; // Taille finale de la classe
//     }
//     return index;
// }



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

int get_class_index(size_t size, size_t* class_size) {
    int index = 0;
    size_t current_size = MIN_BLOCK_SIZE;
    while (size > current_size && index < NUM_CLASSES - 1) {
        current_size *= 2;
        index++;
    }
    if (index >= NUM_CLASSES) {
        fprintf(stderr, "Erreur : index de classe invalide : %d\n", index);
        return -1; // Retourne un index invalide si nécessaire
    }
    if (class_size) {
        *class_size = current_size; // Taille finale de la classe
    }


    return index;
}


    


BlockHeader* get_free_block(size_t size) {
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);
    printf("class index: %d", class_index);
    // Vérifier si l'index de classe est valide
    if (class_index == -1) {
        fprintf(stderr, "Erreur : index de classe invalide : %d\n", class_index);
        return NULL;  // Retourner NULL si l'index est invalide
    }

    // Vérifier que la liste des blocs pour cette classe est initialisée
    if (free_lists[class_index] == NULL) {
        printf("Aucun bloc libre trouvé pour la taille %zu\n", size);
    }

    // Récupérer le premier bloc de la liste pour cette classe
    BlockHeader* block = free_lists[class_index];
    
    if (block) {
        // Enlever le premier bloc de la liste
        
        free_lists[class_index] = block->next;
        block->next = NULL;  // S'assurer qu'il n'a plus de lien vers un autre bloc
    }

    return block;
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




//optimisation pour fusionner avec le bloc suivant si il est libre
void coalesce_blocks(BlockHeader* block){
    if (block->next && (char*)block + block->size == (char*)block->next){
        block->size += block->next->size;
        block->next = block->next->next;
    }
}



// void* my_malloc(size_t size) {
//     printf("Tentative d'allocation de %zu octets\n", size);

//     size_t total_size = size + HEADER_SIZE;
//     BlockHeader* header = get_free_block(size);
    
//     if (!header) {
//         printf("Aucun bloc libre trouvé, allocation via mmap\n");
//         header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
//         if (header == MAP_FAILED) {
//             perror("mmap failed");
//             return NULL;
//         }
//         printf("Bloc alloué avec mmap à l'adresse : %p\n", header);  // Ajout pour le débogage
//     }

//     header->size = total_size;
//     return (void*)(header + 1);
// }

void my_free(void* ptr) {
    if (ptr == NULL) {
        printf("Libération d'un bloc NULL\n");
        return;
    }

    BlockHeader* header = (BlockHeader*)ptr - 1;
    //printf("Libération du bloc à l'adresse %p, taille %zu\n", header, header->size);  // Ajout pour le débogage

    coalesce_blocks(header);
    recycle_block(header);

    if (munmap(header, header->size) == -1) {
        perror("munmap failed");
    }
}


double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void*)) {
    struct timeval start, end;
    void* ptrs[num_allocations];

    // Démarre le chronomètre
    gettimeofday(&start, NULL);

    // Allocation
    for (int i = 0; i < num_allocations; i++) {
        //printf("Allocation #%d de taille %zu\n", i + 1, size);
        ptrs[i] = alloc_func(size);
        if (ptrs[i] == NULL) {
            perror("Allocation failed");
            return -1.0;
        }
        //printf("Bloc alloué à l'adresse %p\n", ptrs[i]);  // Affiche l'adresse allouée
    }

    // Libération
    for (int i = 0; i < num_allocations; i++) {
        //printf("Libération du bloc à l'adresse %p\n", ptrs[i]);
        free_func(ptrs[i]);
    }

    // Arrête le chronomètre
    gettimeofday(&end, NULL);

    // Calcul du temps écoulé en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    return elapsed_time;
}

void* my_malloc(size_t size) {
    //printf("Tentative d'allocation de %zu octets avec mmap\n", size);

    // Calculer la taille totale nécessaire pour l'allocation
    size_t total_size = size + HEADER_SIZE;

    // Allouer un bloc de mémoire via mmap
    BlockHeader* header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (header == MAP_FAILED) {
        perror("mmap failed");
        return NULL;  // Retourner NULL si mmap échoue
    }

    // Initialiser la taille du bloc alloué
    header->size = total_size;

    // Afficher l'adresse du bloc alloué pour le débogage
    //printf("Bloc alloué avec mmap à l'adresse : %p\n", header);

    // Retourner le pointeur vers la mémoire après l'en-tête
    return (void*)(header + 1);  // Le retour se fait après l'en-tête du bloc
}

// Fonction pour allouer de la mémoire avec alignement
void* my_malloc_align(size_t size) {
    // Calculer la taille totale nécessaire pour l'allocation (avec l'en-tête et l'alignement)
    size_t total_size = size + HEADER_SIZE;

    // Allouer un bloc de mémoire via mmap
    BlockHeader* header = mmap(NULL, total_size + ALIGNMENT - 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (header == MAP_FAILED) {
        perror("mmap failed");
        return NULL;  // Retourner NULL si mmap échoue
    }

    // Aligner le bloc mémoire
    void* aligned_ptr = align_memory((void*)(header + 1), ALIGNMENT);  // Alignement après l'en-tête

    // Calculer la taille totale du bloc alloué (incluant l'en-tête)
    header->size = total_size;

    // Retourner le pointeur vers la mémoire après l'en-tête aligné
    return aligned_ptr;
}

// Fonction pour ajuster l'adresse à l'alignement souhaité
void* align_memory(void* ptr, size_t alignment) {
    __intptr_t addr = (__intptr_t)ptr;
    __intptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    return (void*)aligned_addr;
}

