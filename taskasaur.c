#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>

void winch_handler(int sig); 

char** read_todo(FILE* file, int* length);

WINDOW* create_win(int height, int width, int y, int x);
MENU* create_todo_menu(char** todo_list, int todo_length);

#include "config.h"

int 
main(int argc, char** argv) 
{
    int flag;
    FILE* board_file;
    char** todos;
    int todo_length;

    int height, width;
    int ch;

    WINDOW* todo_win;
    MENU* todo_menu;

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
            
            todos = read_todo(board_file, &todo_length);
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
            board_file = fopen(optarg, "w");
            // write init stuff here
            fclose(board_file);
            printf("Successfully created %s\n", optarg);

            todos = malloc(0);
            todo_length = 0;

            break;

        case -1:
        case '?':
            printf("Help string\n");
            return 2;
    }

    /* for (int i = 0; i < todo_length; i++) { */
    /*     printf("%p\n", todos[i]); */
    /*     printf(todos[i]); */
    /* } */    
    /* return 0; */

    // start ncurses 
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();

    getmaxyx(stdscr, height, width);
    refresh();

    /* todo_win = create_win(20, 40, 5, 5); */
    char* test_todo[] = {"1lmao", "2pee", "3poo", "4feces"};
    /* todo_menu = create_todo_menu(test_todo, 4); */
    todo_menu = create_todo_menu(todos, todo_length);
    post_menu(todo_menu);
    
    while ((ch = getch()) != 'q') {
        
        switch (ch) {
            case 'j':
                menu_driver(todo_menu, REQ_DOWN_ITEM);
                break;
            case 'k':
                menu_driver(todo_menu, REQ_UP_ITEM);
                break;
            case 'G': // try to implement gg too
                menu_driver(todo_menu, REQ_LAST_ITEM);
                break;
        } 

    }

    endwin();

    /* Free mem */
    free(todos); // prob need to free each string in list too

    return 0;
}

void 
winch_handler(int sig) 
{
    endwin();
    refresh();
}

char**
read_todo(FILE* file, int* length) 
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

        lineptr[strcspn(lineptr, "\n")] = 0; // remove newline

        out_arr[out_len-1] = lineptr;

        lineptr = NULL;
    }
    
    *length = out_len;
    return out_arr;
}

WINDOW* 
create_win(int height, int width, int y, int x)
{
    WINDOW* new_win = newwin(height, width, y, x);
    wrefresh(new_win);
    return new_win;
}

MENU*
create_todo_menu(char** todo_list, int todo_length)
{
    MENU* todo_menu;
    ITEM** item_list;
    ITEM* cur_item;

    item_list = malloc((todo_length+1)*sizeof(ITEM*));
    for (int i = 0; i < todo_length; i++) {
        printf(todo_list[i]);
        item_list[i] = new_item(todo_list[i], "");
    }
    item_list[todo_length] = NULL; // last item needs to be a null pointer for some reason?

    todo_menu = new_menu(item_list);

    return todo_menu;
}
