#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

typedef struct {
    int pid;
    char name[256];
    long rss;
} Process;

char filter_str[32] = "";

void draw_system_stats(void) {
    FILE *f_mem = fopen("/proc/meminfo", "r");
    long total_mem = 0, free_mem = 0;
    char label[32];

    if (f_mem) {
        while (fscanf(f_mem, "%31s %ld kB", label, &free_mem) != EOF) {
            if (strcmp(label, "MemTotal:") == 0) total_mem = free_mem;
            if (strcmp(label, "MemAvailable:") == 0) break; // free_mem contient maintenant MemAvailable
        }
        fclose(f_mem);
    }

    int mem_pct = (total_mem > 0) ? (int)((total_mem - free_mem) * 100 / total_mem) : 0;

    attron(COLOR_PAIR(2));
    mvprintw(0, 2, "RAM Usage: [");
    for(int i = 0; i < 20; i++) {
        if (i < mem_pct / 5) printw("|"); else printw(".");
    }
    printw("] %d%% (%ld MB / %ld MB)", mem_pct, (total_mem - free_mem) / 1024, total_mem / 1024);
    attroff(COLOR_PAIR(2));
}

int compare_pids(const void *a, const void *b) {
    return ((const Process *)a)->pid - ((const Process *)b)->pid;
}

void display_processes(void) {
    DIR *dir;
    struct dirent *entry;
    Process list[1024];
    int count = 0;

    clear();
    draw_system_stats();

    dir = opendir("/proc");
    if (!dir) return;

    while ((entry = readdir(dir)) != NULL && count < 1024) {
        if (isdigit(entry->d_name[0])) {
            char path[512], name[256];
            snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
            FILE *f = fopen(path, "r");
            if (f) {
                if (fgets(name, sizeof(name), f)) {
                    name[strcspn(name, "\n")] = 0;
                    if (strlen(filter_str) == 0 || strstr(name, filter_str)) {
                        list[count].pid = atoi(entry->d_name);
                        strncpy(list[count].name, name, 255);
                        count++;
                    }
                }
                fclose(f);
            }
        }
    }
    closedir(dir);
    qsort(list, count, sizeof(Process), compare_pids);

    attron(A_BOLD | COLOR_PAIR(1));
    mvprintw(2, 0, "  %-8s %-25s", "PID", "NOM DU PROCESSUS");
    mvprintw(3, 0, "  --------------------------------------------------");
    attroff(A_BOLD | COLOR_PAIR(1));

    for (int i = 0; i < count && i < LINES - 6; i++) {
        mvprintw(i + 4, 2, "%-8d %-25s", list[i].pid, list[i].name);
    }

    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, " (q)Quit (k)Kill (f)Filter: [%s] (c)Clear ", filter_str);
    attroff(A_REVERSE);
    refresh();
}

int main(void) {
    initscr(); start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    noecho(); curs_set(0); timeout(1000);

    int ch = 0;
    while (ch != 'q') {
        display_processes();
        ch = getch();
        if (ch == 'f') {
            echo(); curs_set(1);
            mvprintw(LINES - 2, 0, "Filtre: ");
            getnstr(filter_str, 31);
            noecho(); curs_set(0);
        }
        if (ch == 'c') filter_str[0] = '\0';
        if (ch == 'k') {
            int p_kill;
            echo(); curs_set(1);
            mvprintw(LINES - 2, 0, "PID a tuer: ");
            if (scanw("%d", &p_kill) != ERR) kill(p_kill, SIGTERM);
            noecho(); curs_set(0);
        }
    }
    endwin();
    return 0;
}
