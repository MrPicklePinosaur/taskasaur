#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

void winch_handler(int sig); 

WINDOW* create_list_win(int height, int width, int y, int x);

#include "config.h"

int 
main(int argc, char** argv) 
{
    FILE* board_file;
    int height, width;
    int x, y;
    int ch;
    WINDOW* todo_win;

    signal(SIGWINCH, winch_handler);

    // read from task file
    board_file = fopen(default_board_file, "r");
    if (!board_file) {
        printf("File does not exist\n");
        return 1;
    }   
    fclose(board_file);

    // start ncurses 
    initscr();
    cbreak();
    /* raw(); */
    noecho();
    start_color();
    
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_BLACK, COLOR_CYAN); 

    getmaxyx(stdscr, height, width);
    x = y = 0;
    refresh();

    todo_win = create_list_win(20, 20, 5, 5);
    
    move(y,x);
    while ((ch = getch()) != 113) { // while not q
        
        // ofc the first thing we need is vim keys 
        switch (ch) {
            case 104: // h
                x -= 1;
                break;
            case 106: // j
                y += 1;
                break;
            case 107: // k
                y -= 1;
                break;
            case 108: // l
                x += 1;
                break;
        } 

        move(y,x);
        refresh();
        /* clear(); */
    }

    endwin();
    return 0;
}

void 
winch_handler(int sig) 
{
    endwin();
    refresh();
}

WINDOW* 
create_list_win(int height, int width, int y, int x)
{
    WINDOW* new_win = newwin(height, width, y, x);
    box(new_win, 0, 0);
    wrefresh(new_win);
    return new_win;
}
