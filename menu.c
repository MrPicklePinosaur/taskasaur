
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

#define MAX_CONTENTS_LENGTH 256

typedef struct MenuItem {
    char* title;
    char* description;
    void* user_data;
} MenuItem;

typedef struct Menu {
    char* menu_name;
    MenuItem** menu_items;
    int menu_length;
    int selected_item;
    int scroll_offset;
    bool focused;
    WINDOW* menu_win;
    WINDOW* sub_win;
    int max_height;
    int max_width;
    void* user_data;
} Menu;

int swap_item(Menu* menu, int src_index, int dest_index);

/* insert mode */
int menu_insert_mode(Menu* menu, int insert_index);

/* prob temp for now */
MenuItem* create_blank_menuitem(void);


MenuItem*
create_menuitem(char* title)
{
    MenuItem* new_menuitem;

    new_menuitem = malloc(sizeof(MenuItem));
    new_menuitem->title = title;
    new_menuitem->description = strdup(""); //TEMP FOR NOW

    return new_menuitem;
}

MenuItem*
create_blank_menuitem(void)
{
    return create_menuitem(strdup(""));
}

Menu* 
create_menu(char* menu_name, MenuItem** item_list)
{
    Menu* new_menu;

    new_menu = malloc(sizeof(Menu));
    new_menu->menu_name = menu_name;
    new_menu->menu_items = item_list;
    new_menu->menu_length = array_length(MenuItem*, item_list);
    new_menu->selected_item = 0;
    new_menu->scroll_offset = 0;
    new_menu->focused = false;
    set_menu_win(new_menu, stdscr);

    return new_menu;
}

/* getters */
WINDOW*
get_menu_win(Menu* menu)
{
    return menu->menu_win;
}

WINDOW*
get_menu_subwin(Menu* menu)
{
    return menu->sub_win;
}

MenuItem*
get_menu_item(Menu* menu, int index)
{
    if (index < 0 || index >= menu->menu_length) return NULL;

    return menu->menu_items[index];
}

int
get_selected_item(Menu* menu)
{
    return menu->selected_item;
}

int
get_menu_length(Menu* menu)
{
    return menu->menu_length;
}

char*
get_menu_name(Menu* menu)
{
    return menu->menu_name;
}

char*
get_menuitem_title(MenuItem* menuitem)
{
    return menuitem->title;
}

char*
get_menuitem_descrip(MenuItem* menuitem)
{
    return menuitem->description;
}


/* setters */
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

int
set_selected_item(Menu* menu, int selected_item)
{
    menu->selected_item = selected_item;

    return 0;
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
    ar_swap_item((void**)menu->menu_items, src_index, dest_index);

    return 0;
}

int
delete_item(Menu* menu, int index)
{
    if (index < 0 || index > menu->menu_length-1) return -1;

    for (int i = index; i <= menu->menu_length-1; i++) {
        menu->menu_items[i] = menu->menu_items[i+1];
    }

    menu->menu_items = realloc(menu->menu_items, menu->menu_length*sizeof(MenuItem*)); 
    menu->menu_items[menu->menu_length-1] = 0; // preserve null at end

    menu->menu_length -= 1;

    /* also move the current selected position if it's last */
    if (menu->selected_item > menu->menu_length-1) {
        menu->selected_item = menu->menu_length-1;
    }

    wclear(menu->sub_win);

    return 0;
}

int 
insert_item(Menu* menu, MenuItem* menuitem, int index)
{ // note, this func does not validate index

    /* resize array and insert */
    menu->menu_items = realloc(menu->menu_items, (menu->menu_length+2)*sizeof(MenuItem*));

    for (int i = menu->menu_length; i > index; i--) {
        menu->menu_items[i] = menu->menu_items[i-1];
    }

    menu->menu_items[index] = menuitem;
    menu->menu_items[menu->menu_length+1] = 0; // remember null at end   
    menu->menu_length += 1;

    /* move cursor pos */
    menu->selected_item = index;

    return 0;
}

int
menu_insert_mode(Menu* menu, int insert_index)
{
    char temp[MAX_CONTENTS_LENGTH+1]; // remember null
    char* new_contents;

    /* this is bad */
    wclear(menu->sub_win);
    render_menu(menu);

    curs_on();

    /* move cursor to right spot */
    ungetstr(menu->menu_items[insert_index]->title);
    mvwgetnstr(menu->sub_win,
        insert_index, // account for wrap later too
        0,
        temp,
        MAX_CONTENTS_LENGTH
    );
    curs_off();

    /* copy out */
    new_contents = strdup(temp);
    menu->menu_items[insert_index]->title = new_contents;

    /* delete if empty - maybe move this to a cleanup stage */
    if (strlen(new_contents) == 0) {
        delete_item(menu, insert_index);
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
            break;

        case MENU_APPEND:
            insert_item(menu, create_blank_menuitem(), menu->menu_length);
            menu_insert_mode(menu, menu->selected_item);
            break;

        case MENU_INSERT_ABOVE:
            insert_item(menu, create_blank_menuitem(), menu->selected_item);
            menu_insert_mode(menu, menu->selected_item);
            break;

        case MENU_INSERT_BELOW:
            insert_item(menu, create_blank_menuitem(), menu->selected_item+1);
            menu_insert_mode(menu, menu->selected_item); // inserted item is cur now
            break;

        case MENU_EDIT:
            menu_insert_mode(menu, menu->selected_item);
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
    mvwprintw(menu->menu_win, 0, MENU_PAD_LEFT, menu->menu_name);
    wattroff(menu->menu_win, COLOR_PAIR(0));

    /* draw inner menu */
    int cur_line = 0;
    for (int i = 0; i < menu->menu_length-menu->scroll_offset; i++) {

        int wrapped_lines;
        char* wrapped_text;
        
        /* wrap text by inserting newlines (maxwidth-1 for newline char)*/
        wrapped_text = wrap_text(menu->menu_items[i]->title, menu->max_width-1, &wrapped_lines); 

        /* color selected item */
        wattron(menu->sub_win, COLOR_PAIR(
           (i == menu->selected_item && menu->focused == true) ? 
           TS_SELECTED : TS_NONSELECTED
        ));
        mvwprintw(menu->sub_win, cur_line, 0, wrapped_text);
        wattroff(menu->sub_win, COLOR_PAIR(0));

        cur_line += wrapped_lines;

        /* display number of items */
        /* if (menu->menu_items[i]->) */

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

