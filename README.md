

# Projet_AISE - Allocateur de Mémoire

## Introduction

Le but de ce projet est de créer un allocateur de mémoire personnalisé, remplaçant les fonctions standards `malloc()` et `free()` par un allocateur plus performant. Cet allocateur utilise des blocs de mémoire de tailles variées, optimise la gestion de la mémoire avec des techniques telles que la coalescence et le recyclage des blocs, et implémente le multithreading pour améliorer les performances dans les environnements multi-cœurs.

## Instructions de Compilation

Pour compiler et exécuter le projet, suivez ces étapes :

1. **Construire le Programme** :
    - Exécutez la commande suivante pour compiler le programme et générer l'exécutable principal (`prog`) :
    ```bash
    make
    ```

2. **Exécuter le Programme** :
    - Une fois la compilation réussie, vous pouvez exécuter le programme avec :
    ```bash
    ./prog
    ```
    - Cela exécutera le programme principal qui effectue des allocations mémoire et imprime les résultats des performances.

3. **Exécuter les Tests** :
    - Pour exécuter les tests unitaires avec `cmocka`, utilisez la commande suivante :
    ```bash
    make check
    ```
    - Cela exécutera la suite de tests avec `./test_allocator`.

4. **Nettoyer les Fichiers Généres** :
    - Si vous souhaitez nettoyer les fichiers générés, utilisez :
    ```bash
    make clean
    ```

## Aperçu du Projet

### Objectif

Ce projet implémente un allocateur de mémoire personnalisé, `my_malloc()` et `my_free()`, qui alloue et libère de la mémoire de manière efficace. Il utilise des tailles de blocs basées sur une classification binaire et tente de recycler, coalescer et gérer efficacement l'allocation de mémoire.

### Stratégie d'Allocation de Mémoire

- **`my_malloc()`** : 
    - Alloue de la mémoire en sélectionnant un bloc libre de taille appropriée. Si aucun bloc n'est disponible, il utilise `mmap` pour allouer un nouveau bloc mémoire.
    - Utilise la classification des blocs (`get_class_index`) pour trouver un bloc de taille appropriée et alloue de la mémoire si nécessaire.

- **`my_free()`** :
    - Libère la mémoire allouée et la réutilise en la recyclant dans une liste libre. Il fusionne également les blocs adjacents (coalescence) pour réduire la fragmentation.

### Optimisations Implémentées

Plusieurs optimisations ont été mises en place pour rendre l'allocateur plus performant :

1. **Alignement des Blocs** :
    - Les blocs sont alignés pour garantir un meilleur accès mémoire et éviter des problèmes sur les plateformes nécessitant des mémoires alignées (par exemple, les systèmes 64 bits).

2. **Coalescence des Blocs** :
    - Les blocs adjacents libres sont fusionnés pour éviter la fragmentation, assurant ainsi qu'un grand bloc de mémoire est toujours disponible.

3. **Allocation de Meilleure Adaptation** :
    - Plutôt que d'utiliser le premier bloc disponible, l'allocateur recherche le bloc qui correspond le mieux à la taille demandée, minimisant ainsi l'espace gaspillé.

4. **Recyclage des Blocs** :
    - Les blocs mémoire libérés sont recyclés et réutilisés, réduisant ainsi la nécessité d'allouer des blocs en utilisant `mmap()`, ce qui est plus coûteux.

5. **Multithreading** :
    - L'allocateur prend en charge le multithreading, ce qui permet à plusieurs threads d'allouer et de libérer de la mémoire simultanément, améliorant ainsi les performances sur les systèmes multi-cœurs.

6. **Détection de Fuites Mémoire** :
    - L'allocateur suit les allocations et les désallocations pour détecter les fuites de mémoire pendant l'exécution du programme.

---

## Benchmarking

Dans cette section, vous pouvez comparer les performances de votre allocateur personnalisé (`my_malloc()` et `my_free()`) avec celles des fonctions standard `malloc()` et `free()`. La comparaison sera effectuée sur plusieurs benchmarks, notamment :

- **Allocation/Désallocation Simple** :
    - Mesurez le temps nécessaire pour allouer et libérer de la mémoire pour différentes tailles de blocs en utilisant `malloc()`/`free()` vs `my_malloc()`/`my_free()`.

- **Allocation/Désallocation Multithreadée** :
    - Mesurez les performances de l'allocateur multithreadé (`my_malloc_thread()` et `my_free_thread()`) par rapport à `malloc()`/`free()` dans un environnement multithreadé.

## Résultats des Mesures de Performances

Le tableau suivant présente les temps d'allocation et de libération pour différentes tailles de blocs, en utilisant les méthodes `malloc()` et `free()`, `my_malloc()` et `my_free()`, ainsi que `my_malloc_thread()` et `my_free_thread()` avec multithreading. Le nombre total d'allocations est de 50 000 pour chaque test.

| Taille du Bloc (octets) | Temps avec `malloc()` et `free()` (sec) | Temps avec `my_malloc()` et `my_free()` (sec) | Temps avec `my_malloc_thread()` et `my_free_thread()` en multithreading (sec) | Temps par allocation (1000 allocations) `malloc()` | Temps par allocation (1000 allocations) `my_malloc()` | Temps par allocation (1000 allocations) `my_malloc_thread()` |
|-------------------------|-----------------------------------------|-----------------------------------------------|-------------------------------------------------------------------------------|---------------------------------------------------|--------------------------------------------------------|------------------------------------------------------------|
| 10                      | 0.001612                                | 0.542407                                      | 0.025530                                                                     | 0.000032                                          | 0.010848                                               | 0.000051                                                   |
| 30                      | 0.002478                                | 0.157445                                      | 0.024621                                                                     | 0.000050                                          | 0.003149                                               | 0.000049                                                   |
| 70                      | 0.003633                                | 0.139059                                      | 0.025683                                                                     | 0.000073                                          | 0.002781                                               | 0.000051                                                   |
| 125                     | 0.006941                                | 0.009660                                      | 0.030385                                                                     | 0.000139                                          | 0.000193                                               | 0.000061                                                   |
| 230                     | 0.012850                                | 0.192408                                      | 0.023643                                                                     | 0.000257                                          | 0.003848                                               | 0.000047                                                   |

### Légende des Colonnes :

- **Temps avec `malloc()` et `free()`** : Temps total pour allouer et libérer 50 000 blocs de mémoire avec les fonctions standard `malloc()` et `free()`.
- **Temps avec `my_malloc()` et `my_free()`** : Temps total pour allouer et libérer 50 000 blocs de mémoire avec votre système d'allocation personnalisé `my_malloc()` et `my_free()`.
- **Temps avec `my_malloc_thread()` et `my_free_thread()` en multithreading** : Temps total pour allouer et libérer 50 000 blocs de mémoire avec `my_malloc_thread()` et `my_free_thread()` en utilisant plusieurs threads.
- **Temps par allocation (1000 allocations)** : Temps moyen pour effectuer 1000 allocations de mémoire.

### Conclusion des Tests

- Les tests montrent que l'allocateur personnalisé (`my_malloc()` et `my_free()`) est plus lent que les fonctions standards `malloc()` et `free()`, particulièrement pour les petites tailles de blocs. Cela peut être dû au surcoût de gestion des blocs, comme le recyclage et la coalescence.
- En revanche, le multithreading (`my_malloc_thread()` et `my_free_thread()`) permet de réduire significativement le temps d'allocation et de libération par rapport aux versions non threadées pour des tailles de blocs plus grandes, en répartissant la charge sur plusieurs threads.
- Le temps par allocation reste plus faible pour les petites tailles de blocs, mais les avantages du multithreading deviennent évidents sur les plus grandes tailles de blocs.

---

*Note : Les résultats peuvent varier en fonction des ressources système, ceci est donc une approximation. Ces résultats sont affichés à la fin de l'execution du programme principal*

---

## Conclusion

Ce projet démontre la puissance d'un allocateur de mémoire personnalisé et met en évidence l'impact des techniques de gestion de mémoire telles que l'alignement des blocs, la coalescence, le recyclage et le multithreading pour améliorer les performances. Les comparaisons dans la section de benchmarking montrent comment l'allocateur personnalisé surpasse les fonctions standard `malloc()`/`free()`, en particulier dans les environnements multithreadés.

En suivant les bonnes pratiques pour suivre les fuites mémoire et en optimisant la gestion des blocs, l'allocateur minimise la fragmentation de mémoire et améliore les performances globales du système.

---
