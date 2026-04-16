#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

void dessiner_barre(int y, int x, int largeur, float pourcentage, char *label) {
    int remplissage = (int)(pourcentage * largeur / 100);
    mvprintw(y, x, "%s [", label);
    for (int i = 0; i < largeur; i++) {
        if (i < remplissage) addch('|');
        else addch(' ');
    }
    printw("] %.1f%%", pourcentage);
}

void afficher_stats_systeme() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return;

    long total = 0, libre = 0;
    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f)) {
        if (strncmp(ligne, "MemTotal:", 9) == 0) sscanf(ligne, "MemTotal: %ld", &total);
        if (strncmp(ligne, "MemAvailable:", 13) == 0) sscanf(ligne, "MemAvailable: %ld", &libre);
    }
    fclose(f);

    float utilise_pct = (float)(total - libre) / total * 100;
    dessiner_barre(0, 0, 30, utilise_pct, "RAM");
}

void afficher_processus() {
    struct dirent *entree;
    DIR *dossier = opendir("/proc");
    int ligne = 4;

    attron(A_BOLD);
    mvprintw(3, 0, "%-8s | %-20s", "PID", "NOM");
    attroff(A_BOLD);

    while ((entree = readdir(dossier)) != NULL && ligne < LINES - 1) {
        if (isdigit(entree->d_name[0])) {
            char chemin[512], nom[256];
            snprintf(chemin, sizeof(chemin), "/proc/%s/comm", entree->d_name);
            FILE *f = fopen(chemin, "r");
            if (f) {
                if (fgets(nom, sizeof(nom), f)) {
                    nom[strcspn(nom, "\n")] = 0;
                    mvprintw(ligne++, 0, "%-8s | %-20s", entree->d_name, nom);
                }
                fclose(f);
            }
        }
    }
    closedir(dossier);
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    timeout(500);

    while (getch() != 'q') {
        clear();
        afficher_stats_systeme();
        afficher_processus();
        refresh();
        usleep(500000);
    }

    endwin();
    return 0;
}
