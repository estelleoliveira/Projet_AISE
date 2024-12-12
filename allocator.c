#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> //pour srand() et rand()

#include "allocator.h"
//liste globale des blocs libres classés par tailles
BlockHeader* free_lists[NUM_CLASSES] = {NULL};

AllocRecord alloc_records[1000];
int alloc_count = 0;

//Proteger les listes de blocs libres
pthread_mutex_t free_list_mutex[NUM_CLASSES] = {PTHREAD_MUTEX_INITIALIZER};

//Mutex pour l'allocation et desallocation
pthread_mutex_t alloc_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Goulots d'étranglements:

Chaque my_alloc() demande un changement de l'espace d'adressage 
Les petites allocations fragmentent la mémoire avec des segments disjoints
Pas de réutilisation de blocs, chaque appel créer un nouveau bloc
Verification des erreurs à chaque appel même si les erreurs sont rares
*/


int recycle_block(BlockHeader* block, int verbose) {
    if (!block) {
        if (verbose) {
            printf("Block null\n");
        }
        return -1;
    }

    if (block->next || block->size == 0) {
        if (verbose) {
            printf("Erreur : tentative de recycler un bloc corrompu ou déjà libéré\n");
        }
        return -1;
    }
    
    coalesce_blocks(block);
    int class_index;
    size_t class_size;
    class_index = get_class_index(block->size - HEADER_SIZE, &class_size);
    if (class_index == -1) {
        if (verbose) {
            printf("Invalid index for block recycling!\n");
        }
        return -1;
    }



    block->next = free_lists[class_index];
    free_lists[class_index] = block;


    if (verbose) {
        printf("Block recycled at address %p\n", (void*)block);
    }

    return 0;
}

int recycle_block_thread(BlockHeader* block, int verbose) {
    if (!block) {
        if (verbose) {
            printf("Block null\n");
        }
        return -1;
    }

    if (block->next || block->size == 0) {
        if (verbose) {
            printf("Erreur : tentative de recycler un bloc corrompu ou déjà libéré\n");
        }
        return -1;
    }
    
    coalesce_blocks(block);
    int class_index;
    size_t class_size;
    class_index = get_class_index(block->size - HEADER_SIZE, &class_size);
    if (class_index == -1) {
        if (verbose) {
            printf("Invalid index for block recycling!\n");
        }
        return -1;
    }

    pthread_mutex_lock(&free_list_mutex[class_index]);

    block->next = free_lists[class_index];
    free_lists[class_index] = block;

    pthread_mutex_unlock(&free_list_mutex[class_index]);

    if (verbose) {
        printf("Block recycled at address %p\n", (void*)block);
    }

    return 0;
}



//trouver la classe de taille pour une taille donnée
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

//recherche le meilleur bloc libre
//Cette fonction n'est pas utilisé car elle n'est pas optimisé, temps de recherche trop long
BlockHeader* get_best_fit_block(size_t size, int verbose) {
    // printf("Getting best fit block...");
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);

    if (class_index == -1) {
        return NULL;
    }

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
        printf("Fusion des blocs à l'adresse %p et %p\n", (void*)block, (void*)block->next);
        block->size += block->next->size;
        block->next = block->next->next;
    }
}


BlockHeader* get_free_block(size_t size, int verbose) {
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);
    
    // Vérifier si l'index de classe est valide
    if (class_index == -1) {
        fprintf(stderr, "Erreur : index de classe invalide : %d\n", class_index);
        return NULL;  // Retourner NULL si l'index est invalide
    }
    

    // Vérifier que la liste des blocs pour cette classe est initialisée
    if (!free_lists[class_index]) {
        if (verbose) printf("Aucun bloc libre trouvé pour la taille %zu\n", size);
        
        return NULL;
    }
    //  Récupérer le premier bloc de la liste pour cette classe 
        // Enlever le premier bloc de la liste
        BlockHeader* block = free_lists[class_index];
        free_lists[class_index] = block->next;
        block->next = NULL;
        if (verbose) printf("Free block found\n");
        
        return block;  // S'assurer qu'il n'a plus de lien vers un autre bloc
    
}

BlockHeader* get_free_block_thread(size_t size, int verbose) {
    int class_index;
    size_t class_size;
    class_index = get_class_index(size, &class_size);
    //printf("class index: %d", class_index);
    
    // Vérifier si l'index de classe est valide
    if (class_index == -1) {
        fprintf(stderr, "Erreur : index de classe invalide : %d\n", class_index);
        return NULL;  // Retourner NULL si l'index est invalide
    }
    pthread_mutex_lock(&free_list_mutex[class_index]);

    // Vérifier que la liste des blocs pour cette classe est initialisée
    if (!free_lists[class_index]) {
        //printf("Aucun bloc libre trouvé pour la taille %zu\n", size);
        pthread_mutex_unlock(&free_list_mutex[class_index]);
        return NULL;
    }
    //  Récupérer le premier bloc de la liste pour cette classe 
        // Enlever le premier bloc de la liste
        BlockHeader* block = free_lists[class_index];
        free_lists[class_index] = block->next;
        block->next = NULL;
        if (verbose) printf("Free block found\n");
        pthread_mutex_unlock(&free_list_mutex[class_index]);
        return block;  // S'assurer qu'il n'a plus de lien vers un autre bloc
    
}




void* my_malloc(size_t size, int verbose) {
    
    if (verbose) {
        printf("Tentative d'allocation de %zu octets\n", size);
    }

    size_t total_size = size + HEADER_SIZE;
    BlockHeader* header = get_free_block(size, verbose);
    
    if (header == NULL) {
        if (verbose) {
            printf("Aucun bloc libre trouvé, allocation via mmap\n");
        }
        header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
        if (header == MAP_FAILED) {
            perror("mmap failed");
            
            return NULL;
        }
        if (verbose) {
            printf("Bloc alloué avec mmap à l'adresse : %p\n", (void*)header);
        }
    }

    void* aligned_ptr = align_memory((void*)(header + 1), ALIGNMENT);  // Alignement après l'en-tête

    header->size = total_size;
    
    return aligned_ptr;
}

void* my_malloc_thread(size_t size, int verbose) {
    pthread_mutex_lock(&alloc_mutex);
    if (verbose) {
        printf("Tentative d'allocation de %zu octets\n", size);
    }

    size_t total_size = size + HEADER_SIZE;
    BlockHeader* header = get_free_block_thread(size, verbose);
    
    if (header == NULL) {
        if (verbose) {
            printf("Aucun bloc libre trouvé, allocation via mmap\n");
        }
        header = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
        if (header == MAP_FAILED) {
            perror("mmap failed");
            pthread_mutex_unlock(&alloc_mutex);
            return NULL;
        }
        if (verbose) {
            printf("Bloc alloué avec mmap à l'adresse : %p\n", (void*)header);
        }
    }

    void* aligned_ptr = align_memory((void*)(header + 1), ALIGNMENT);  // Alignement après l'en-tête

    header->size = total_size;
    pthread_mutex_unlock(&alloc_mutex);
    return aligned_ptr;
}

void my_free(void* ptr, int verbose) {
    if (ptr == NULL) {
        if (verbose) {
            printf("Libération d'un bloc NULL\n");
        }
        return;
    }

    track_deallocation(ptr);

    BlockHeader* header = (BlockHeader*)ptr - 1;

    if (verbose) {
        printf("Libération du bloc à l'adresse %p, taille %zu\n", (void*)header, header->size);
    }

    if (recycle_block(header,verbose) == -1) {
        if (munmap(header, header->size) == -1) {
            perror("munmap failed");
        }
    }

    
}

void my_free_thread(void* ptr, int verbose) {
    if (ptr == NULL) {
        if (verbose) {
            printf("Libération d'un bloc NULL\n");
        }
        return;
    }

    pthread_mutex_lock(&alloc_mutex);

    track_deallocation(ptr);

    BlockHeader* header = (BlockHeader*)ptr - 1;

    if (verbose) {
        printf("Libération du bloc à l'adresse %p, taille %zu\n", (void*)header, header->size);
    }

    if (recycle_block_thread(header,verbose) == -1) {
        if (munmap(header, header->size) == -1) {
            perror("munmap failed");
        }
    }

    pthread_mutex_unlock(&alloc_mutex);
}



double measure_allocations(int num_allocations, size_t size, void* (*alloc_func)(size_t,int), void (*free_func)(void* , int), int verbose) {
    struct timeval start, end;
    void* ptrs[num_allocations];

    // Démarre le chronomètre
    gettimeofday(&start, NULL);

    // Allocation
    for (int i = 0; i < num_allocations; i++) {
        //printf("Allocation #%d de taille %zu\n", i + 1, size);
        ptrs[i] = alloc_func(size, verbose);
        if (ptrs[i] == NULL) {
            perror("Allocation failed");
            return -1.0;
        }
        //printf("Bloc alloué à l'adresse %p\n", ptrs[i]);  // Affiche l'adresse allouée
    }

    // Libération
    for (int i = 0; i < num_allocations; i++) {
        //printf("Libération du bloc à l'adresse %p\n", ptrs[i]);
        free_func(ptrs[i], verbose);
    }

    // Arrête le chronomètre
    gettimeofday(&end, NULL);

    // Calcul du temps écoulé en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    return elapsed_time;
}

//Cette fonction existe car malloc a un seul argument tandis que my_alloc en a deux (+verbose)
double measure_allocations_default(int num_allocations, size_t size, void* (*alloc_func)(size_t), void (*free_func)(void* )) {
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
        // printf("Bloc alloué à l'adresse %p\n", ptrs[i]);  // Affiche l'adresse allouée
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



double measure_allocations_variable_size(int n_allocations, size_t min_size, size_t max_size, void* (*alloc_func)(size_t, int), void (*free_func)(void*, int), int verbose){
    struct timeval start, end;
    void* block;

    gettimeofday(&start, NULL);
    
    srand(time(0));
    for (int i = 0; i < n_allocations; ++i) {
        // Générer une taille aléatoire entre min_size et max_size
        size_t random_size = min_size + rand() % (max_size - min_size + 1);
        
        // Allocation de mémoire
        block = alloc_func(random_size, verbose);
        if (block == NULL) {
            perror("Allocation failed");
            return -1.0;
        }

        // Affichage de la taille générée et de l'adresse allouée si verbose est activé
        if (verbose) {
            printf("Taille générée : %ld\n", random_size);
            printf("Bloc alloué à l'adresse %p pour la taille %zu\n", block, random_size);
        }

        // Libération du bloc alloué
        free_func(block, verbose);
        if (verbose) {
            printf("Libération du bloc à l'adresse %p\n", block);
        }
    }
    gettimeofday(&end, NULL);

    // Calcul du temps écoulé en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    return elapsed_time;
}

//Cette fonction existe car malloc a un seul argument tandis que my_alloc en a deux (+verbose)
double measure_allocations_default_variable_size(int n_allocations, size_t min_size, size_t max_size, void* (*alloc_func)(size_t), void (*free_func)(void* )) {
    struct timeval start, end;
    void* block[n_allocations];

    // Démarre le chronomètre
    gettimeofday(&start, NULL);

    // Allocation
    for (int i = 0; i < n_allocations; ++i) {
        // Générer une taille aléatoire entre min_size et max_size
        size_t random_size = min_size + rand() % (max_size - min_size + 1);
        
        // Allocation de mémoire
        block[i] = alloc_func(random_size);
        if (block[i] == NULL) {
            perror("Allocation failed");
            return -1.0;
        }

        // Libération du bloc alloué
        free_func(block[i]);
    }
    gettimeofday(&end, NULL);

    // Arrête le chronomètre
    gettimeofday(&end, NULL);

    // Calcul du temps écoulé en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    return elapsed_time;
}

// Fonction pour ajuster l'adresse à l'alignement souhaité
void* align_memory(void* ptr, size_t alignment) {
    __intptr_t addr = (__intptr_t)ptr;
    __intptr_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    return (void*)aligned_addr;
}

//traquer les fuites mémoires
void track_allocation(void* ptr, size_t size) {
    alloc_records[alloc_count].ptr = ptr;
    alloc_records[alloc_count].size = size;
    alloc_count++;
}

void track_deallocation(void* ptr) {
    for (int i = 0; i < alloc_count; i++) {
        if (alloc_records[i].ptr == ptr) {
            alloc_records[i] = alloc_records[alloc_count - 1];
            alloc_count--;
            return;
        }
    }
}

void detect_leaks() {
    if (alloc_count > 0) {
        printf("Fuite détectée!\n");
        for (int i = 0; i < alloc_count; i++) {
            printf("Fuite de bloc %p de taille %zu\n", alloc_records[i].ptr, alloc_records[i].size);
        }
    } else {
        printf("Pas de fuite mémoire détécté\n");
    }
}



void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    void* ptrs[data->num_allocations];

    // Allocation
    for (int i = 0; i < data->num_allocations; i++) {
        ptrs[i] = data->alloc_func(data->size, data->verbose);
        if (ptrs[i] == NULL) {
            perror("Allocation failed in thread");
            return NULL;
        }
    }

    // Desalloc
    for (int i = 0; i < data->num_allocations; i++) {
        data->free_func(ptrs[i], data->verbose);
    }

    return NULL;
}

// Optimisé pour le multithreading
double measure_allocations_thread(int num_threads, int num_allocations, size_t size, void* (*alloc_func)(size_t, int), void (*free_func)(void*, int), int verbose) {
    struct timeval start, end;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    //Distribution égale du travail à travers les threads
    int allocations_per_thread = num_allocations / num_threads;

    // Set up the thread data and create threads
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].size = size;
        thread_data[i].alloc_func = alloc_func;
        thread_data[i].free_func = free_func;
        thread_data[i].verbose = verbose;
        thread_data[i].num_allocations = allocations_per_thread;

        // Thread qui créer les threads
        if (pthread_create(&threads[i], NULL, thread_function, (void*)&thread_data[i]) != 0) {
            perror("Failed to create thread");
            return -1.0;
        }
    }

    
    gettimeofday(&start, NULL);

    // Attendre qque toutes les threads aient terminé
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }


    gettimeofday(&end, NULL);


    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    return elapsed_time;
}
