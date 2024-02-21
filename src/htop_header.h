#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"

// Déclaration de la fonction get_processus pour éviter les avertissements d'implicite
void get_processus(WINDOW *win);

// Déclaration préalable de la fonction actualiser_processus
void *actualiser_processus(void *args);

void kill_processus(int pid);
WINDOW *win; // Déclaration globale de la fenêtre
