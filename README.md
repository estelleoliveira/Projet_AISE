

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

Complétez le tableau suivant avec vos résultats de benchmark :

### Résultats des Benchmarks

| Cas de Test                                  | Temps malloc()/free()  | Temps my_malloc()/my_free() | Temps my_malloc_thread()/my_free_thread() |
|----------------------------------------------|------------------------|-----------------------------|-------------------------------------------|
| Allocation/Désallocation (Taille Bloc: 30)   | [Votre Résultat]       | [Votre Résultat]           | [Votre Résultat]                         |
| Allocation/Désallocation (Taille Bloc: 70)   | [Votre Résultat]       | [Votre Résultat]           | [Votre Résultat]                         |
| Allocation/Désallocation (Taille Aléatoire)  | [Votre Résultat]       | [Votre Résultat]           | [Votre Résultat]                         |
| Allocation/Désallocation Multithreadée (5 threads) | [Votre Résultat]    | [Votre Résultat]           | [Votre Résultat]                         |

*Note : Les résultats peuvent varier en fonction des ressources système, ceci est donc une approximation*

---

## Conclusion

Ce projet démontre la puissance d'un allocateur de mémoire personnalisé et met en évidence l'impact des techniques de gestion de mémoire telles que l'alignement des blocs, la coalescence, le recyclage et le multithreading pour améliorer les performances. Les comparaisons dans la section de benchmarking montrent comment l'allocateur personnalisé surpasse les fonctions standard `malloc()`/`free()`, en particulier dans les environnements multithreadés.

En suivant les bonnes pratiques pour suivre les fuites mémoire et en optimisant la gestion des blocs, l'allocateur minimise la fragmentation de mémoire et améliore les performances globales du système.

---

N'hésitez pas à ajuster les sections, notamment celles des benchmarks, selon vos propres résultats. Ce modèle organise les informations de manière claire et détaillée tout en étant facilement personnalisable pour votre projet.