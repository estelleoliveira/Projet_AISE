#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <time.h>

#include "allocator.h"

int main() {
    size_t size = 70;
    size_t class_size;
    int class_index = get_class_index(size, &class_size);
    printf("Allocation d'un bloc de %zu octets...\n", size);
    printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n", class_index, class_size);
    void* ptr1= my_malloc(size);
    if (ptr1) {
        printf("Bloc alloué à l'adresse : %p\n", ptr1);
        strcpy((char*)ptr1, "Hello, world!");
        printf("Contenu du bloc : %s\n", (char*)ptr1);
    }

    printf("Libération du bloc...\n");
    my_free(ptr1);
    printf("Bloc 1 libéré avec succès.\n\n");


    size = 30;
    class_index = get_class_index(size, &class_size);
    printf("Allocation d'un bloc de %zu octets...\n", size);
    printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n", class_index, class_size);
    void* ptr2= my_malloc(size);
    if (ptr2) {
        printf("Bloc alloué à l'adresse : %p\n", ptr2);
        strcpy((char*)ptr2, "Hello, world!");
        printf("Contenu du bloc : %s\n", (char*)ptr2);
    }

    printf("Libération du bloc...\n");
    my_free(ptr2);
    printf("Bloc 2 libéré avec succès.\n\n");

    srand((unsigned int)time(NULL));    //permet de ne pas avoir toujours la même valeur généré
    size = 1 + rand() % 250;    //génère un bloc de taille aléatoire entre 1 et 250
    class_index = get_class_index(size, &class_size);
    printf("Taille générée aléatoirement : %zu\n", size);
    printf("Allocation d'un bloc de %zu octets...\n", size);
    printf("Classe sélectionnée : %d (Taille de classe : %zu octets)\n", class_index, class_size);
    void* ptr3= my_malloc(size);
    if (ptr3) {
        printf("Bloc alloué à l'adresse : %p\n", ptr3);
        strcpy((char*)ptr3, "Hello, world!");
        printf("Contenu du bloc : %s\n", (char*)ptr3);
    }

    printf("Libération du bloc...\n");
    my_free(ptr3);
    printf("Bloc 3 libéré avec succès.\n\n");

    
    return 0;
}