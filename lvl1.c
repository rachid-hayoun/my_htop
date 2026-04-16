#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>

void afficher_processus() {
    struct dirent *entree;
    DIR *dossier = opendir("/proc");

    if (dossier == NULL) {
        perror("Impossible d'ouvrir /proc");
        return;
    }

    printf("\n--- LISTE DES PROCESSUS ---\n");
    printf("PID\t| NOM\n");
    printf("---------------------------\n");

    while ((entree = readdir(dossier)) != NULL) {
        
        if (isdigit(entree->d_name[0])) {
            char chemin[512];
            char nom[256];
            
            snprintf(chemin, sizeof(chemin), "/proc/%s/comm", entree->d_name);
            
            FILE *f = fopen(chemin, "r");
            if (f) {
                if (fgets(nom, sizeof(nom), f)) {
                    
                    printf("%s\t| %s", entree->d_name, nom);
                }
                fclose(f);
            }
        }
    }
    closedir(dossier);
    printf("---------------------------\n");
}

int main() {
    char choix;

    do {
        afficher_processus();
        
        printf("\nAppuyez sur 'r' pour rafraichir ou 'q' pour quitter : ");
        scanf(" %c", &choix); 

    } while (choix != 'q');

    printf("Au revoir !\n");
    return 0;
}
