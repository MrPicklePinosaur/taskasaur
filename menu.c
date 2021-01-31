
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
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

int swap_item(Menu* menu, int src_index, int dest_index);
int delete_item(Menu* menu, int index);

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
    new_menu->menu_items = item_list;
    new_menu->menu_length = array_length(MenuItem*, item_list);
    new_menu->selected_item = 0;
    set_menu_win(new_menu, stdscr);

    return new_menu;
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
swap_item(Menu* menu, int src_index, int dest_index)
{
    MenuItem* temp;

    temp = menu->menu_items[dest_index];
    menu->menu_items[dest_index] = menu->menu_items[src_index];
    menu->menu_items[src_index] = temp;

    return 0;
}

int
delete_item(Menu* menu, int index)
{
    if (index < 0 || index > menu->menu_length-1) return -1;

    int temp_size = (menu->menu_length-index-1)*sizeof(MenuItem*);
    MenuItem* temp[temp_size];

    /* might break if last item? */
    memcpy(temp, menu->menu_items[index+1], temp_size);
    memcpy(menu->menu_items[index], temp, temp_size);

    menu->menu_items = realloc(menu->menu_items, menu->menu_length*sizeof(MenuItem*)); 
    menu->menu_items[menu->menu_length-1] = 0; // preserve null at end

    menu->menu_length -= 1;

    /* also move the current selected position if it's last */
    if (menu->selected_item > menu->menu_length-1) {
        menu->selected_item = menu->menu_length-1;
    }

    return 0;
}

int
menu_driver(Menu* menu, MenuAction action)
{

    switch (action) {
        case MENU_UP:
            menu->selected_item = menu->selected_item-1 >= 0 ? menu->selected_item-1 : 0;
            break;

        case MENU_DOWN:
            menu->selected_item = menu->selected_item+1 <= menu->menu_length-1 ? menu->selected_item+1 : menu->menu_length-1;
            break;

        case MENU_TOP:
            menu->selected_item = 0;
            break;

        case MENU_BOTTOM:
            menu->selected_item = menu->menu_length-1;
            break;

        case MENU_MOVE_UP:
            if (menu->selected_item <= 0) break;
            swap_item(menu, menu->selected_item, menu->selected_item-1);
            menu->selected_item -= 1;
            break;

        case MENU_MOVE_DOWN:
            if (menu->selected_item >= menu->menu_length-1) break;
            swap_item(menu, menu->selected_item, menu->selected_item+1);
            menu->selected_item += 1;
            break;

        case MENU_DELETE:
            delete_item(menu, menu->selected_item);
            wclear(menu->menu_win);
            break;

        default: // This is here for debug, disable later
            fprintf(stderr, "Invalid menu action");
    }

    return 0;
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

