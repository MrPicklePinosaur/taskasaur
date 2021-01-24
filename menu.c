
#include <stdlib.h>
#include <stddef.h>
#include <ncurses.h>

#include "headers/menu.h"
#include "headers/render.h"
#include "headers/utils.h"

typedef struct MenuItem {
    char* contents;
} MenuItem;

typedef struct Menu {
    MenuItem** menu_items;
    int menu_length;
    int selected_item;
    WINDOW* menu_win;
    int max_height;
    int max_width;
} Menu;

int render_menu(Menu* menu);

MenuItem*
create_menuitem(char* contents)
{
    MenuItem* new_menuitem;

    new_menuitem = malloc(sizeof(MenuItem));
    new_menuitem->contents = contents;

    return new_menuitem;
}

Menu* 
create_menu(MenuItem** item_list)
{
    Menu* new_menu;

    new_menu = malloc(sizeof(Menu));
    set_menu_items(new_menu, item_list);
    new_menu->selected_item = 0;
    set_menu_win(new_menu, stdscr);

    return new_menu;
}

int
set_menu_items(Menu* menu, MenuItem** item_list)
{
    menu->menu_items = item_list;
    menu->menu_length = array_length(MenuItem*, item_list);

    return 0;
}

int
set_menu_win(Menu* menu, WINDOW* win)
{
    menu->menu_win = win;
    getmaxyx(menu->menu_win, menu->max_height, menu->max_width);

    return 0;
}

MenuItem*
get_menu_items(Menu* menu)
{
    return NULL;
}

int
render_menu(Menu* menu)
{
    int cur_line;

    cur_line = 0;

    for (int i = 0; i < menu->menu_length; i++) {

        int wrapped_lines;
        char* wrapped_text;
        int text_color;
        
        /* wrap text by inserting newlines */
        wrapped_text = wrap_text(menu->menu_items[i]->contents, menu->max_width, &wrapped_lines);        

        /* color selected item */
        text_color = (i == menu->selected_item) ? TS_SELECTED : TS_NONSELECTED;

        wattron(menu->menu_win, COLOR_PAIR(text_color));
        mvwprintw(menu->menu_win, cur_line, 0, wrapped_text);
        wattroff(menu->menu_win, COLOR_PAIR(text_color));

        cur_line += wrapped_lines;

        free(wrapped_text);

    }

    return 0;
}

int
free_menu(Menu* menu)
{
    return 0;
}

