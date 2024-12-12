#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "allocator.h"

/*Les optimisations faites:
Coalescence des blocs libres dans my_free()
Alignement des blocs mémoires avec my_malloc_align()

TODO:
-> Réparer la fonction get_free_block()
//Probablement une affaire de pointeur nul


*/

int main() {
  size_t size = 70;
  size_t class_size;
  int class_index = get_class_index(size, &class_size);
  printf("Allocation d'un bloc de %zu octets...\n", size);
  printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n",
         class_index, class_size);
  printf("Test\n");
  void *ptr1 = my_malloc(size, 1);
  if (ptr1) {
    printf("Bloc alloué à l'adresse : %p\n", ptr1);
    strcpy((char *)ptr1, "Hello, world!");
    printf("Contenu du bloc : %s\n", (char *)ptr1);
  }

  printf("Libération du bloc...\n");
  my_free(ptr1, 1);
  printf("Bloc 1 libéré avec succès.\n\n");

  size = 30;
  class_index = get_class_index(size, &class_size);
  printf("Allocation d'un bloc de %zu octets...\n", size);
  printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n",
         class_index, class_size);
  void *ptr2 = my_malloc(size, 1);
  if (ptr2) {
    printf("Bloc alloué à l'adresse : %p\n", ptr2);
    strcpy((char *)ptr2, "Hello, world!");
    printf("Contenu du bloc : %s\n", (char *)ptr2);
  }

  printf("Libération du bloc...\n");
  my_free(ptr2, 1);
  printf("Bloc 2 libéré avec succès.\n\n");

  srand((unsigned int)time(
      NULL)); // permet de ne pas avoir toujours la même valeur généré
  size = 1 + rand() % 250; // génère un bloc de taille aléatoire entre 1 et 250
  // size = 126;
  class_index = get_class_index(size, &class_size);
  printf("Taille générée aléatoirement : %zu\n", size);
  printf("Allocation d'un bloc de %zu octets...\n", size);
  printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n",
         class_index, class_size);
  // My_alloc crashes when size < 128 and >64
  void *ptr3 = my_malloc(size, 1);
  if (ptr3) {
    printf("Bloc alloué à l'adresse : %p\n", ptr3);
    strcpy((char *)ptr3, "Hello, world!");
    printf("Contenu du bloc : %s\n", (char *)ptr3);
  }

  printf("Libération du bloc...\n");
  my_free(ptr3, 1);
  printf("Bloc 3 libéré avec succès.\n\n");

  printf("Mesures de performances:\n\n");
  const int n_allocations = 50000;
  int n_threads = 5;

  const size_t array_size= 5;
  size_t block_size[] = {10,30, 70, 125,230};
  for (size_t i = 0; i < array_size; ++i) {

    double time_malloc =
        measure_allocations_default(n_allocations, block_size[i], malloc, free);
    if (time_malloc < 0)
      return 1;
    printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de "
           "taille %zu avec méthodes malloc() et free(), seconds per 1000 alloc: %lf \n",
           time_malloc, n_allocations, block_size[i],
           time_malloc*1000.0 / (double)n_allocations);

    double time_mmap = measure_allocations(n_allocations, block_size[i],
                                           my_malloc, my_free, 0);
    if (time_mmap < 0)
      return 1;
    printf(
        "Temps: %lf pour l'allocation et libération de %d blocs mémoires de "
        "taille %zu avec méthodes my_malloc() et my_free(), seconds per 1000 alloc: %lf \n",
        time_mmap, n_allocations, block_size[i],
        time_mmap*1000.0 / (double)n_allocations);

     // Measure performance with my_malloc/my_free
        double time_multithread = measure_allocations_thread(n_threads, n_allocations, block_size[i], my_malloc_thread, my_free_thread, 0);
        if (time_multithread < 0) {
            return 1;  // Return on error
        }
        printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de taille %zu avec méthodes my_malloc_thread() et my_free_thread() en multithreading, seconds per 1000 alloc: %lf \n",
               time_multithread, n_allocations, block_size[i], time_multithread*1000.0 / (double)n_allocations);
  }

  /*const size_t arrayaleatoire_size = 1;
  size_t bloc_size[] = {(size_t)-1};
  srand((unsigned int)time(NULL));

  for (int i = 0; i < arrayaleatoire_size; ++i) {
    printf("\nMesures de performance avec allocations de taille aléatoire : \n");
    size_t current_block_size = bloc_size[i];
    if (current_block_size == (size_t)-1) {
      current_block_size = 1 + rand() % 250;
      printf("Génération d'une taille aléatoire : %zu\n", current_block_size);
    }

    double time_aleatoire = measure_allocations_default(n_allocations, current_block_size, malloc, free);
    if (time_aleatoire < 0) {return 1;}
    printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de "
            "taille %zu avec méthodes malloc() et free(), seconds per 1000 alloc: %lf \n",
            time_aleatoire, n_allocations, current_block_size,
            time_aleatoire*1000.0 / n_allocations);


  }*/


  printf("\nMesures de performances avec tailles variables : \n\n");
  const size_t min_random_size = 1;
  const size_t max_random_size = 250;
  const int nb_allocations = 500000;

  double time_malloc_random = measure_allocations_default_variable_size(nb_allocations, min_random_size, max_random_size, malloc, free);
  if (time_malloc_random < 0) {return 1;}

  printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de "
         "tailles variables avec malloc() et free(), seconds per 1000 alloc: %lf \n",
         time_malloc_random, nb_allocations, time_malloc_random * 1000.0 / (double)nb_allocations);
  
  double time_my_malloc_random = measure_allocations_variable_size(nb_allocations, min_random_size, max_random_size, my_malloc, my_free, 0);
  if (time_my_malloc_random < 0) {return 1;}

  printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de "
         "tailles variables avec my_malloc() et my_free(), seconds per 1000 alloc: %lf \n",
         time_my_malloc_random, nb_allocations, time_my_malloc_random * 1000.0 / (double)nb_allocations);

  
  double time_multithread_random = measure_allocations_thread_variable_size(n_threads, n_allocations, min_random_size, max_random_size, my_malloc_thread, my_free_thread, 0);
    if (time_multithread_random < 0) {
        return 1;  // Return on error
    }
    printf("Temps: %lf pour l'allocation et libération de %d blocs mémoires de taille variables avec méthodes my_malloc_thread() et my_free_thread() en multithreading, seconds per 1000 alloc: %lf \n",
            time_multithread_random, nb_allocations, time_multithread_random*1000.0 / (double)nb_allocations);
  
  return 0;

}