#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>

WINDOW* create_list_win(int height, int width, int y, int x);

int 
main(int argc, char** argv) 
{
    int height, width;
    int ch;

    WINDOW* todo_win;
    char* todos[] = {"eat bread", "eat milk", "eat cheese"};
    int todo_length;
    ITEM** todo_items;
    MENU* todo_menu;
    ITEM* cur_item;

    // start ncurses 
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();

    getmaxyx(stdscr, height, width);
    refresh();

    /* init todo menu stuff */

    todo_win = create_list_win(20, 40, 5, 5);
    todo_length = sizeof(todos)/sizeof(todos[0]);

    todo_items = malloc((todo_length+1)*sizeof(ITEM*));
    for (int i = 0; i < todo_length; i++) {
        todo_items[i] = new_item(todos[i], "");
    }
    todo_items[todo_length] = NULL; // last item needs to be a null pointer for some reason?

    todo_menu = new_menu(todo_items);
    post_menu(todo_menu);

    /* todo_menu = new_menu(); */

    /* for (int i = 0; i < todo_length; i++) { */
    /*     mvwprintw(todo_win, i, 0, todos[i]); */
    /* } */
    wrefresh(todo_win);

    while ((ch = getch()) != 113) { // while not q
        
        // ofc the first thing we need is vim keys 
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

        refresh();
        /* clear(); */
    }

    endwin();

    return 0;
}

WINDOW* 
create_list_win(int height, int width, int y, int x)
{
    WINDOW* new_win = newwin(height, width, y, x);
    wrefresh(new_win);
    return new_win;
}
