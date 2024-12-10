# Projet_AISE

Pour la compilation : 
$ make          //construit les tests et l'executable du projet
$ ./prog        //execute le programme et alloue des blocs de différentes tailles
$ make check    //execute les tests (les tests sont liés à cmocka pour permettre de visualiser l'ensemble des différents tests) -> execute la commande ./test_allocator

Le but de ce projet est d'allouer grâce à malloc des blocs de différentes tailles. L'execution du programme alloue 3 blocs différents de taille 30, 70 et de taille aléatoire (entre 1 et 250). Les blocs sont ensuite unmmap. 
Le programme va aller chercher un bloc de taille 2^k avec k qui augmente tant que la taille du bloc demandé est au dessus de la taille des blocs de la classe. 

### Optimisations
- Alignement des blocs
- coalescence des blocs
- Prends le bloc le plus adapté
- Recyclage des blocs
- Multithreading
- Traqueur de fuites mémoires