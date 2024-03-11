//
// Created by eliea on 11/03/2024.
//
#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

#ifndef HTOP_PID_HEADER_H
#define HTOP_PID_HEADER_H

#define TOP_PID_WIN 10
#define PID_COL 2
//#define PRIORITY_COL 10
//#define NICE_COL 17
//#define VIRT_COL 22
//#define RES_COL 32
//#define SHR_COL 40
//#define STATE_COL 50
#define CPU_COL 30
#define MEM_COL 50
#define TIME_COL 60
#define COMMAND_COL 70

typedef struct {
    long pid;
    int priority;
    int nice;
    long size;
    long total_size;
    long resident;
    long shared;
    long utime;
    long stime;
    long long virt;
    char state;
    char command[256];
} ProcessInfo;

void display_processus(WINDOW *window, int current_scroll_position, int *current_row);
void display_default_table();
void kill_processus(int width);
void read_cpu_info();
void read_memory_info();
#endif //HTOP_PID_HEADER_H