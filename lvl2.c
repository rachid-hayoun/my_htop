#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void afficher_lvl2(char *filtre) {
    struct dirent *entree;
    DIR *dossier = opendir("/proc");

    printf("\n%-8s | %-20s | %-10s | %s\n", "PID", "NOM", "ETAT", "MEMOIRE");
    printf("------------------------------------------------------------\n");

    while ((entree = readdir(dossier)) != NULL) {
        if (isdigit(entree->d_name[0])) {
            char chemin[512], ligne[256];
            char nom[256] = "Inconnu", etat[256] = "Inconnu", memoire[256] = "0";
            
            snprintf(chemin, sizeof(chemin), "/proc/%s/status", entree->d_name);
            FILE *f = fopen(chemin, "r");
            if (f) {
                while (fgets(ligne, sizeof(ligne), f)) {
                    if (strncmp(ligne, "Name:", 5) == 0) 
                        sscanf(ligne, "Name:\t%s", nom);
                    if (strncmp(ligne, "State:", 6) == 0) 
                        sscanf(ligne, "State:\t%s", etat);
                    if (strncmp(ligne, "VmRSS:", 6) == 0) 
                        sscanf(ligne, "VmRSS:\t%s", memoire);
                }
                fclose(f);

                if (filtre == NULL || strstr(nom, filtre) != NULL) {
                    printf("%-8s | %-20s | %-10s | %s kB\n", entree->d_name, nom, etat, memoire);
                }
            }
        }
    }
    closedir(dossier);
}

int main() {
    char recherche[256];
    
    printf("Entrez un nom de processus a filtrer (ou 'all' pour tout voir) : ");
    scanf("%s", recherche);

    if (strcmp(recherche, "all") == 0) {
        afficher_lvl2(NULL);
    } else {
        afficher_lvl2(recherche);
    }

    return 0;
}
