#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_TODO_LENGTH 200

void winch_handler(int sig); 

char** read_todo(FILE* file);

WINDOW* create_list_win(int height, int width, int y, int x);

#include "config.h"

int 
main(int argc, char** argv) 
{
    int flag;
    FILE* board_file;
    int height, width;
    int x, y;
    int ch;
    WINDOW* todo_win;

    signal(SIGWINCH, winch_handler);

    // read command line args
    flag = getopt(argc, argv, "o:n:");
    switch (flag) {
        case 'o':

            // read from task file (might need to check for read and write permissions)
            board_file = fopen(optarg, "r");
            if (!board_file) {
                printf("%s does not exist\n", optarg);
                return 1;
            }   
            
            char** todos = read_todo(board_file);
            printf(todos[0]);
            fclose(board_file);

            break;

        case 'n':

            // make sure file does not exist
            // however, it maybe be possible that an different error has occured (besides the file not existing)
            if (access(optarg, F_OK) == 0) { 
                printf("%s already exists\n", optarg);
                return 1;
            }
            // create a file here
            printf("Successfully created %s\n", optarg);
            break;

        case -1:
        case '?':
            printf("Help string\n");
            return 2;
    }



    return 0;

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


char**
read_todo(FILE* file) 
{ // apparently getline isn't rly that portable, so consider other options
    char** out_arr;
    int out_len;
    char* lineptr;
    size_t len;
    ssize_t nread;

    out_arr = NULL;
    out_len = 0;
    lineptr = NULL;
    len = 0;

    while ((nread = getline(&lineptr, &len, file)) != -1) {
        out_len++;
        out_arr = realloc(out_arr, (sizeof(char*))*out_len); // bad to keep resizing?
        printf("Pointer set to: %p\n", lineptr);
        out_arr[out_len-1] = lineptr;

        lineptr = NULL;
        len = 0;
    }

    return out_arr;
}

WINDOW* 
create_list_win(int height, int width, int y, int x)
{
    WINDOW* new_win = newwin(height, width, y, x);
    box(new_win, 0, 0);
    wrefresh(new_win);
    return new_win;
}
