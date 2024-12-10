# Projet_AISE

#### Compilation :  

$ make  
Cette commande compile les fichiers sources du projet et crée l'executable principal ainsi que es tests associés.  
$ ./prog  
Permet l'execution du programme une fois la compilation terminée. L'execution du programme permet de voir les allocations mémoire pour plusieurs blocs ainsi que les mesures de performances.  
$ make check  
Permet d'executer les tests (les tests sont liés à cmocka pour permettre de visualiser l'ensemble des différents tests) -> execute la commande ./test_allocator  

L'objectif de ce projet est d'allouer grâce à malloc des blocs de différentes tailles. L'execution du programme alloue 3 blocs différents de taille 30, 70 et de taille aléatoire (entre 1 et 250). Les blocs sont ensuite unmmap.  
Le programme va aller chercher un bloc de taille 2^k avec k qui augmente tant que la taille du bloc demandé est au dessus de la taille des blocs de la classe.  

my_malloc() permet d'allouer un bloc de mémoire. L'allocation se fait en récupérant un bloc libre dont la taille est déterminée avec get_class_index. Si aucun bloc libre n'est disponible, my_malloc() utilisera mmap pour allouer un nouveau bloc.  

my_free() permet de libérer la mémoire. Elle effectue aussi le recyclage de blocs et effectue la coalescence, qui permet de combiner les blocs voisins pour éviter la fragmentation.  

### Optimisations
- Alignement des blocs
- coalescence des blocs
- Prends le bloc le plus adapté
- Recyclage des blocs
- Multithreading
- Traqueur de fuites mémoires