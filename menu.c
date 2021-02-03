
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "headers/menu.h"
#include "headers/render.h"
#include "headers/utils.h"

#define MENU_PAD_TOP 2
#define MENU_PAD_BOTTOM 1
#define MENU_PAD_LEFT 2
#define MENU_PAD_RIGHT 1

typedef struct MenuItem {
    char* contents;
} MenuItem;

typedef struct Menu {
    MenuItem** menu_items;
    int menu_length;
    int selected_item;
    int scroll_offset;
    bool focused;
    WINDOW* menu_win;
    WINDOW* sub_win;
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
    new_menu->scroll_offset = 0;
    new_menu->focused = false;
    set_menu_win(new_menu, stdscr);

    return new_menu;
}

int
set_menu_win(Menu* menu, WINDOW* win)
{
    int height, width;

    menu->menu_win = win;
    getmaxyx(menu->menu_win, height, width);

    /* create a subwin (also prob free old subwin?) */
    menu->max_height = height-MENU_PAD_TOP-MENU_PAD_BOTTOM;
    menu->max_width = width-MENU_PAD_LEFT-MENU_PAD_RIGHT;
    menu->sub_win = derwin(
            menu->menu_win, 
            menu->max_height,
            menu->max_width,
            MENU_PAD_TOP, 
            MENU_PAD_LEFT
    );

    return 0;
}

MenuItem*
get_menu_items(Menu* menu)
{
    return NULL;
}

int
set_menu_focus(Menu* menu, bool focus)
{
    menu->focused = focus;
    
    return 0;
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
            wclear(menu->sub_win);
            break;

        default: // This is here for debug, disable later
            fprintf(stderr, "Invalid menu action");
    }

    return 0;
}

int
render_menu(Menu* menu)
{
    /* draw outer menu (prob dont need this every render) */ 
    int menu_header_color;

    /* menu_header_color = */ 
    wattron(menu->menu_win, COLOR_PAIR(
        (menu->focused == true) ?
        TS_MENU_SELECTED: TS_MENU_NONSELECTED       
    ));
    mvwprintw(menu->menu_win, 0, MENU_PAD_LEFT, "TODO");
    wattroff(menu->menu_win, COLOR_PAIR(0));

    /* draw inner menu */
    int cur_line = 0;
    for (int i = 0; i < menu->menu_length-menu->scroll_offset; i++) {

        int wrapped_lines;
        char* wrapped_text;
        
        /* wrap text by inserting newlines (maxwidth-1 for newline char)*/
        wrapped_text = wrap_text(menu->menu_items[i]->contents, menu->max_width-1, &wrapped_lines); 

        /* color selected item */
        wattron(menu->sub_win, COLOR_PAIR(
           (i == menu->selected_item && menu->focused == true) ? 
           TS_SELECTED : TS_NONSELECTED
        ));
        mvwprintw(menu->sub_win, cur_line, 0, wrapped_text);
        wattroff(menu->sub_win, COLOR_PAIR(0));

        cur_line += wrapped_lines;

        free(wrapped_text);

    }

    wrefresh(menu->sub_win);
    wrefresh(menu->menu_win);

    return 0;
}

int
free_menu(Menu* menu)
{
    return 0;
}

