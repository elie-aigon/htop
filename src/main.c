#include "htop_header.h"

void kill_processus(int pid) {
    if (kill(pid, SIGTERM) == -1) { // Essayez de terminer le processus gracieusement
        perror("Erreur lors de la tentative de terminaison du processus");
    }
}

void *update_processus() {
    while (true) {
        wclear(win);
        get_processus(win);
        wrefresh(win);
        usleep(100000);
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

    win = newwin(height - 3, width, 0, 0); // Laissez un peu d'espace pour la fenêtre d'entrée
    scrollok(win, TRUE); // Active le défilement dans la fenêtre
// Création de la fenêtre pour les entrées utilisateur
    WINDOW *input_win = newwin(3, width, height - 3, 0); // Fenêtre d'entrée en bas
    box(input_win, 0, 0); // Optionnel : ajoute une bordure à la fenêtre d'entrée
    wrefresh(input_win); // Affiche la fenêtre d'entrée
    scrollok(input_win, TRUE);

    pthread_t thread;
    pthread_create(&thread, NULL, update_processus, NULL); // Crée le thread pour actualiser les processus

    int ch;
    while (1) {
        ch = getch();
//        if(ch == KEY_F(10)){ // Closing state
//            wprintw(win, "F10 key pressed, closing htop...");
//            wrefresh(win);
//            pthread_cancel(thread);
//            usleep(2000000);
//            delwin(win);
//            endwin();
//            break;
//        }
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
            case KEY_F(4): // Si l'utilisateur appuie sur F4
                wclear(input_win); // Nettoie la fenêtre d'entrée
                box(input_win, 0, 0); // Redessine la bordure
                mvwprintw(input_win, 1, 1, "Enter PID to kill: "); // Demande le PID
                wrefresh(input_win);

                echo();
                char pid_str[10];
                wgetstr(input_win, pid_str); // Utilise wgetstr pour lire à partir de la fenêtre d'entrée
                int pid = atoi(pid_str);
                if (pid > 0) {
                    kill_processus(pid);
                } else {
                    mvwprintw(input_win, 1, 1, "Invalid PID.         ");
                }
                noecho();
                wrefresh(input_win);
                break;
            case KEY_F(10): // Si l'utilisateur appuie sur F3
                wclear(input_win); // Nettoie la fenêtre d'entrée
                box(input_win, 0, 0);
                mvwprintw(input_win, 1, 1, "F10 key pressed, closing htop...");
                wrefresh(input_win);
                pthread_cancel(thread);
                usleep(2000000);
                delwin(win);
                delwin(input_win);
                endwin();
                return 0;


            default:
                wclear(input_win);
                box(input_win, 0, 0);
                mvwprintw(input_win, 1, 1, "%d est préssée", ch);
                wrefresh(input_win);
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
//                break;
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


