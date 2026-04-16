#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

void afficher_interface() {
    struct dirent *entree;
    DIR *dossier;
    int ligne = 2;

    clear();
    mvprintw(0, 0, "--- Niveau 3 ---");
    attron(A_BOLD);
    mvprintw(1, 0, "%-8s | %-20s", "PID", "NOM");
    attroff(A_BOLD);

    dossier = opendir("/proc");
    if (dossier == NULL) return;

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
    refresh();
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    timeout(500);

    while (getch() != 'q') {
        afficher_interface();
        usleep(500000);
    }

    endwin();
    return 0;
}
