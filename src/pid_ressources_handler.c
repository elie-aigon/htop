//
// Created by eliea on 11/03/2024.
//
#include "include/pid_ressources_header.h"

void display_default_table() {
    mvprintw(TOP_PID_WIN, PID_COL, " PID ");
    mvprintw(TOP_PID_WIN, CPU_COL, " CPU%% ");
    mvprintw(TOP_PID_WIN, MEM_COL, " MEM%% ");
    mvprintw(TOP_PID_WIN, TIME_COL, " TIME+ ");
    mvprintw(TOP_PID_WIN, COMMAND_COL, " Command ");
}

void get_process_stat(long pid, ProcessInfo *process) {
    int column = 1;
    char *token;
    char path[256];
    char buffer[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%ld/stat", pid);
    file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    fgets(buffer, sizeof(buffer), file);
    token = strtok(buffer, " ");
    while (token) {
        if (column == 3) process->state = token[0];
        if (column == 14) process->utime = atol(token);
        if (column == 15) process->stime = atol(token);
        if (column == 18) process->priority = atoi(token);
        if (column == 19) process->nice = atoi(token);
        if (column == 23) process->virt = atoll(token);
        token = strtok(NULL, " ");
        column++;
    }
    fclose(file);
    return;
}

void get_process_statm(long pid, ProcessInfo *process) {
    char path[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%ld/statm", pid);
    file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    fscanf(file, "%ld %ld %ld", &process->size, &process->resident, &process->shared);
    fclose(file);
    process->size *= 4;
    process->resident *= 4;
    process->shared *= 4;
    return;
}

long get_process_total_memory() {
    FILE *file;
    char buffer[256];
    long total_memory = -1;

    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier /proc/meminfo");
        return total_memory;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (strncmp(buffer, "MemTotal:", 9) == 0) {
            sscanf(buffer, "%*s %ld", &total_memory);
            break;
        }
    }

    fclose(file);
    return total_memory * 1024;
}

void get_process_command(long pid, ProcessInfo *process) {
    char path[256];
    char buffer[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%ld/cmdline", pid);
    file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        process->command[0] = '\0';
        return;
    }

    fclose(file);
    strcpy(process->command, buffer);
    return;
}

void kill_processus(int width) {
    int pid;
    char pid_str[10];
    WINDOW *input_window = newwin(TOP_PID_WIN, width/2, 0, width/2);
    box(input_window, 0, 0);

    werase(input_window);
    mvwprintw(input_window, 1, 1, "(F2 for cancel and ENTER to valid PID)");
    mvwprintw(input_window, 2, 1, "Enter the PID you wan't to kill :");
    wrefresh(input_window);
    int ch;
    int i = 0;
    while(i < 9){
        ch = getch();
        if (ch == '\n') {
            break;
        } else if (ch == KEY_F(2)) {
            i = 0;
            break;
        }
        if (ch == KEY_BACKSPACE && i > 0) {
            --i;
            mvwdelch(input_window, 1, 35 + i);
            wrefresh(input_window);
            continue;
        }
        if (isdigit(ch)) {
            pid_str[i++] = ch;
            waddch(input_window, ch);
            wrefresh(input_window);
        }
    }
    pid_str[i] = '\0';
    pid = atoi(pid_str);

    if (i == 0) {
        mvprintw(LINES - 2, 2, " Action cancelled. ");
        refresh();
        getch();
        return;
    } else {
        if (kill(pid, SIGTERM) == 0) {
            mvprintw(LINES - 2, 2, " Process %d killed successfully. ", pid);
        } else {
            mvprintw(LINES - 2, 2, " Failed to kill process %d. ", pid);
            sleep(2000)
        }
    }
    getch();
    delwin(input_window);
    endwin();
    return;
}

void display_processus(WINDOW *window, int current_scroll_position, int *current_row){
    DIR *dir;
    if ((dir = opendir("/proc")) == NULL) {
        mvwprintw(window, 1, 1, "Error: Could not open directory (src/pid_info.c:display_processes:directory)");
        return;
    }

    int row = 0;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR){
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            long total_memory = get_process_total_memory();
            int row_position = current_scroll_position + row + 1;

            ProcessInfo processInfo;
            processInfo.pid = pid;
            get_process_stat(pid, &processInfo);
            get_process_statm(pid, &processInfo);
            get_process_command(pid, &processInfo);
            if (*endptr == '\0' && processInfo.command[0] != '\0'){
                mvwprintw(window, row_position, PID_COL, "%ld", processInfo.pid);
                mvwprintw(window, row_position, CPU_COL, "%.1f", (float) (processInfo.utime + processInfo.stime) / processInfo.size * 100.0);
                mvwprintw(window, row_position, MEM_COL, "%.1f", (float) (processInfo.resident + processInfo.shared + processInfo.virt) / (processInfo.resident + processInfo.shared + processInfo.virt + total_memory) * 100.0);
                mvwprintw(window, row_position, TIME_COL, "%ld:%02ld.%02ld",
                          (processInfo.utime + processInfo.stime) / 6000,
                          ((processInfo.utime + processInfo.stime) % 6000) / 100,
                          (processInfo.utime + processInfo.stime) % 100);
                mvwprintw(window, row_position, COMMAND_COL, "%.*s", COLS - COMMAND_COL - 1, processInfo.command);
                row++;
            }
        }
    }
    closedir(dir);
    if (current_row != NULL ) {
        *current_row = row;
    }
    return;
}

void read_cpu_info() {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL) {
        mvprintw(2, 2, "Error: Could not open file (src/memory_cpu_info.c:read_cpu_info:file)");
        return;
    }

    char line[256];
    int cpu_number;
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    int row = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line,"cpu%d %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                   &cpu_number, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice) == 11) {

            unsigned long total_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
            unsigned long user_time = user + nice + system;
            float cpu_usage = 100.0 * ((float) user_time / total_time);

            if (row >= 4) {
                mvprintw(row - 3, 20, "CPU%d", cpu_number);
                mvprintw(row - 3, 24, "[%05.2f%%]", cpu_usage);
            } else {
                mvprintw(row + 1, 2, "CPU%d", cpu_number);
                mvprintw(row + 1, 6, "[%05.2f%%]", cpu_usage);
            }
            row++;
        }
    }
    fclose(file);
    return;
}

void read_memory_info() {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        mvprintw(5, 2, "Error: Could not open file (src/memory_cpu_info.c:read_memory_info:file)");
        return;
    }

    char unit[3];
    char field_name[256];
    unsigned long mem_used;
    unsigned long mem_total, mem_available;
    unsigned long swap_total, swap_free;

    while (fscanf(file, "%255s %lu %2s\n", field_name, &mem_used, unit) == 3) {
        if (strcmp(field_name, "MemTotal:") == 0) mem_total = mem_used;
        if (strcmp(field_name, "MemAvailable:") == 0) mem_available = mem_used;
        if (strcmp(field_name, "SwapTotal:") == 0) swap_total = mem_used;
        if (strcmp(field_name, "SwapFree:") == 0) swap_free = mem_used;
    }
    fclose(file);

    unsigned long mem_current_used = mem_total - mem_available;
    unsigned long swap_used = swap_total - swap_free;
    float mem_current_used_percent = (float) mem_current_used / mem_total * 100;

    mvprintw(5, 2, "Memory");
    mvprintw(6, 2, "Swap");

    mvprintw(5, 6, "[%05.2fG/%.2fG] in percent: %.2f%%", mem_current_used / 1024.0 / 1024.0, mem_total / 1024.0 / 1024.0, mem_current_used_percent);
    mvprintw(6, 6, "[%.2fG/%.2fG]", swap_used / 1024.0 / 1024.0, swap_total / 1024.0 / 1024.0);
    return;
}