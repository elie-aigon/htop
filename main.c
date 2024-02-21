#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"

// Déclaration de la fonction get_processus pour éviter les avertissements d'implicite
void get_processus(WINDOW *win);

// Déclaration préalable de la fonction actualiser_processus
void *actualiser_processus(void *args);

WINDOW *win; // Déclaration globale de la fenêtre

void *update_processus() {
    while (true) {
        wclear(win);
        get_processus(win);
        wrefresh(win);
        usleep(1000000); // Pause de 3 secondes
    }
    return NULL;
}

void get_processus(WINDOW *win) {
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // Vérifie si le nom du répertoire est un nombre (PID)
            if (dir->d_type == DT_DIR && strtol(dir->d_name, NULL, 10) > 0) {
                char chemin[256];
                snprintf(chemin, sizeof(chemin), "/proc/%s/status", dir->d_name); // Chemin vers le fichier status

                FILE *f = fopen(chemin, "r");
                if (f) {
                    char ligne[256];
                    // Lit la première ligne qui contient le nom du processus
                    if (fgets(ligne, sizeof(ligne), f)) {
                        // Extrayez le nom du processus ici
                        // La première ligne du fichier status est généralement "Name:   [NomDuProcessus]"
                        char *nom_processus = strchr(ligne, ':') + 2;
                        nom_processus[strlen(nom_processus) - 1] = '\0'; // Supprime le saut de ligne
                        wprintw(win, "PID: %s, Nom: %s\n", dir->d_name, nom_processus);
                    }
                    fclose(f);
                }
            }
        }
        closedir(d);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);

    win = newwin(height, width, 0, 0);
    scrollok(win, TRUE); // Active le défilement dans la fenêtre

    pthread_t thread;
    pthread_create(&thread, NULL, update_processus, NULL); // Crée le thread pour actualiser les processus

    int ch;
    while (1) {
        ch = getch();
        if(ch == KEY_F(10)){ // Closing state
            wprintw(win, "F10 key pressed, closing htop...");
            wrefresh(win);
            pthread_cancel(thread);
            usleep(2000000);
            delwin(win);
            endwin();
            break;
        }
        switch (ch) {
            case KEY_F(1): // Si l'utilisateur appuie sur F1
                clear(); // Nettoie l'écran
                wprintw(win, "Refreshed\n");
                get_processus(win); // Affiche les processus dans la fenêtre
                break;
            case KEY_F(2): // Si l'utilisateur appuie sur F2
                clear(); // Nettoie l'écran
                wprintw(win, "F2 Pressé");
                break;
            case KEY_F(10): // Si l'utilisateur appuie sur F3
                endwin(); // Termine la session Ncurses
                return 0; // Quitte le programme
            default:
                wprintw(win, "%d est préssée", ch);
                wrefresh(win);
                break;
        }
    }

    delwin(win);
    endwin();
    pthread_cancel(thread); // Annule le thread lorsque l'application se termine
    return 0;
}


//    while (true) {
//        ch = getch();
//        switch (ch) {
//            case KEY_F(1): // Si l'utilisateur appuie sur F1
//                clear(); // Nettoie l'écran
//                wprintw(win, "Refreshed\n");
//                get_processus(win); // Affiche les processus dans la fenêtre
²²²         //                break;
//            case KEY_F(2): // Si l'utilisateur appuie sur F2
//                clear(); // Nettoie l'écran
//                wprintw(win, "F2 Pressé");
//                break;
//            case KEY_F(10): // Si l'utilisateur appuie sur F3
//                endwin(); // Termine la session Ncurses
//                return 0; // Quitte le programme
//            default:
//                clear();
//                wprintw(win, "%d est préssée", ch);
//                break;
//        }
//        wrefresh(win); // Rafraîchit la fenêtre après chaque opération
//    }


