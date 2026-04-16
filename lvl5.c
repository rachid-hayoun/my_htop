#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int pid;
    char name[256];
} Process;

Process *list = NULL;
int count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int running = 1;

void* background_scan(void* arg) {
    while (running) {
        DIR *d = opendir("/proc");
        struct dirent *e;
        Process *tmp_list = malloc(sizeof(Process) * 1024);
        int tmp_count = 0;

        while ((e = readdir(d)) != NULL && tmp_count < 1024) {
            if (isdigit(e->d_name[0])) {
                tmp_list[tmp_count].pid = atoi(e->d_name);
                char path[512];
                snprintf(path, sizeof(path), "/proc/%s/comm", e->d_name);
                FILE *f = fopen(path, "r");
                if (f) {
                    fgets(tmp_list[tmp_count].name, 256, f);
                    tmp_list[tmp_count].name[strcspn(tmp_list[tmp_count].name, "\n")] = 0;
                    fclose(f);
                    tmp_count++;
                }
            }
        }
        closedir(d);

        pthread_mutex_lock(&lock);
        if (list) free(list);
        list = tmp_list;
        count = tmp_count;
        pthread_mutex_unlock(&lock);

        usleep(1000000); 
    }
    return NULL;
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    timeout(100);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, background_scan, NULL);

    while (getch() != 'q') {
        clear();
        mvprintw(0, 0, "--- Niveau 5 ---");
        mvprintw(1, 0, "Appuyez sur 'q' pour quitter");

        pthread_mutex_lock(&lock);
        for (int i = 0; i < count && i < LINES - 3; i++) {
            mvprintw(i + 3, 0, "PID: %-8d | Nom: %s", list[i].pid, list[i].name);
        }
        pthread_mutex_unlock(&lock);

        refresh();
        usleep(100000);
    }

    running = 0;
    pthread_join(thread_id, NULL);
    if (list) free(list);
    endwin();
    return 0;
}
