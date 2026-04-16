# 🚀 My Htop - Process Manager

Ce projet est une réimplémentation pédagogique de l'outil `htop` en langage C. Il permet de monitorer les processus système en temps réel en exploitant les données du système de fichiers `/proc`.

## 🛠 Outils & Technologies
* **Langage C** : Coeur du projet pour la performance et l'accès système.
* **Système /proc** : Extraction des données brutes des processus Linux (PID, CPU, RAM).
* **Ncurses** : Création d'une interface graphique interactive en mode terminal.
* **Pthreads** : Multithreading pour séparer l'affichage de la récupération des données.
* **Valgrind & GDB** : Débogage intensif et contrôle strict de l'hygiène mémoire.

## 📈 Progression par Niveaux
* **Level 1 & 2** : Parsing des fichiers `/proc/[PID]/stat`.
* **Level 3** : Interface utilisateur avec `ncurses`.
* **Level 4 & 5** : Monitoring en temps réel et parallélisation par threads.

## ⚙️ Installation
```bash
make
./manager
```
