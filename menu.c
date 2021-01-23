
#include <stdlib.h>
#include <stddef.h>
#include <ncurses.h>

#include "headers/menu.h"
#include "headers/utils.h"

typedef struct MenuItem {
    char* contents;
} MenuItem;

typedef struct Menu {
    MenuItem** menu_items;
    int menu_length;
    int selected_item;
    WINDOW* menu_win;
} Menu;

int render_menu(Menu* menu);

Menu* 
create_menu(MenuItem** item_list)
{
    Menu *new_menu;

    new_menu = malloc(sizeof(Menu));
    new_menu->menu_items = item_list;
    new_menu->menu_length = array_length(MenuItem*, item_list);
    new_menu->selected_item = 0;
    WINDOW* menu_win = NULL;

    return NULL;
}

int
set_menu_items(Menu* menu, MenuItem** item_list)
{
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

    for (int i = 0; i < menu->menu_length; i++) {
        
        /* color selected item */

    }

    return 0;
}


