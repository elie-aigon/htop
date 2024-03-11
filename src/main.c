#include "include/main_header.h"
#include "include/pid_ressources_header.h"

int main() {
    int ch;
    int current_scroll_position = 0;
    int current_row = 0;

    WINDOW *pid_window;
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    timeout(1000);
    do{
        clear();
        int inverse_current_row = current_row * -1;
        pid_window = subwin(stdscr, LINES - TOP_PID_WIN, COLS, TOP_PID_WIN, 0);

        display_processus(pid_window, current_scroll_position, &current_row);
        read_memory_info();
        read_cpu_info();

        box(pid_window, 0, 0);
        display_default_table();
        wrefresh(pid_window);
        refresh();
        ch = getch();
        switch (ch) {
            case KEY_DOWN:
                if (current_scroll_position > inverse_current_row + LINES - 12) current_scroll_position--;
                break;
            case KEY_UP:
                if (current_scroll_position < 0) current_scroll_position++;
                break;
            case KEY_F(1):
                int width, height;
                getmaxyx(pid_window, height, width);
                height++;
                kill_processus(width);
                break;
        }
    } while(ch != KEY_F(10));
    delwin(pid_window);
    endwin();
    return 0;

}
