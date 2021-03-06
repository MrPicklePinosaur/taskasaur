
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "headers/menu.h"
#include "headers/render.h"
#include "headers/utils.h"

#define MENU_PAD_TOP 2
#define MENU_PAD_BOTTOM 2
#define MENU_PAD_LEFT 2
#define MENU_PAD_RIGHT 1

#define MAX_CONTENTS_LENGTH 256

typedef struct MenuItem {
    char* title;
    char* description;
    void* userdata;
} MenuItem;

typedef struct Menu {
    char* menu_name;
    MenuItem** menu_items;
    int menu_length;
    int selected_item;
    int scroll_offset;
    bool focused;
    WINDOW* menu_win;
    int max_height;
    int max_width;
    void* userdata;
} Menu;

int swap_item(Menu* menu, int src_index, int dest_index);

/* insert mode */
int menu_insert_mode(Menu* menu, int insert_index);

/* prob temp for now */
MenuItem* create_blank_menuitem(void);

/* rendering stuff */
int render_item(Menu* menu, int item_index, int start_y);
int item_height(MenuItem* menuitem);
int items_visible(Menu* menu, int offset);
int items_visible_rev(Menu* menu, int offset);

MenuItem*
create_menuitem(char* title)
{
    MenuItem* new_menuitem;
    TodoItem* new_userdata;

    new_menuitem = malloc(sizeof(MenuItem));
    new_menuitem->title = title;
    new_menuitem->description = strdup(""); //TEMP FOR NOW

    new_userdata = malloc(sizeof(TodoItem));
    new_userdata->item_name = title;
    new_userdata->description = strdup("");
    new_userdata->due = strdup("");
    new_userdata->subtask_list = malloc(0);
    new_userdata->subtask_count = 0;
    
    new_menuitem->userdata = new_userdata;

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

void*
get_menu_userdata(Menu* menu)
{
    return menu->userdata;
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

void*
get_menuitem_userdata(MenuItem* menuitem)
{
    return menuitem->userdata;
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
set_menu_userdata(Menu* menu, void* userdata)
{
    menu->userdata = userdata;
    return 0;
}

int
set_menuitem_descrip(MenuItem* menuitem, char* descrip)
{
    menuitem->description = descrip;
    return 0;
}

int
set_menuitem_userdata(MenuItem* menuitem, void* userdata)
{
    menuitem->userdata = userdata;
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
    int insert_pos;

    curs_on();

    // account for multiline items
    insert_pos = menu->scroll_offset;
    for (int i = 0; i < insert_index; i++) {
        insert_pos += item_height(menu->menu_items[i]);
    }

    /* move cursor to right spot */
    ungetstr(menu->menu_items[insert_index]->title);
    mvwgetnstr(menu->menu_win,
        insert_pos, 
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
            render_menu(menu); // refresh after inserting
            menu_insert_mode(menu, menu->selected_item);
            break;

        case MENU_INSERT_ABOVE:
            insert_item(menu, create_blank_menuitem(), menu->selected_item);
            render_menu(menu);
            menu_insert_mode(menu, menu->selected_item);
            break;

        case MENU_INSERT_BELOW:
            insert_item(menu, create_blank_menuitem(), menu->selected_item+1);
            render_menu(menu);
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
    wclear(menu->menu_win);

    /* calculate scroll */
    int visible;

    visible = items_visible(menu, menu->scroll_offset);

    if (menu->selected_item >= menu->scroll_offset+visible) {
        menu->scroll_offset = clamp(
            menu->selected_item-items_visible_rev(menu, menu->selected_item)+1,
            0,
            floorzero(menu->menu_length-1)
        );

    } else if (menu->selected_item < menu->scroll_offset) {
        menu->scroll_offset = clamp(
            menu->selected_item,
            0,
            floorzero(menu->menu_length-1)
        );
    }

    /* render menu items */
    int curline = 0;
    for (int i = menu->scroll_offset; i < menu->menu_length; i++) {
        curline += render_item(menu, i, curline);
    }

    wrefresh(menu->menu_win);
    wrefresh(menu->menu_win);

    return 0;
}

int
render_item(Menu* menu, int item_index, int start_y)
{
    MenuItem* curitem;
    int hlcolor;
    curitem = menu->menu_items[item_index];

    /* color selected item */
    hlcolor = COLOR_PAIR((item_index == menu->selected_item && menu->focused == true) ? TS_SELECTED : TS_NONSELECTED);
    wattron(menu->menu_win, hlcolor);
    mvwprintw(menu->menu_win, start_y, 0, curitem->title);
    wattroff(menu->menu_win, hlcolor);

    /* display number of items */
    if (strlen(curitem->description) > 0) {
        wattron(menu->menu_win, COLOR_PAIR(TS_ITEMCOUNT));
        mvwprintw(menu->menu_win, start_y+1, 0, curitem->description); 
        wattroff(menu->menu_win, COLOR_PAIR(TS_ITEMCOUNT));
    }

    return item_height(curitem);
}

int
item_height(MenuItem* menuitem)
{
    int lines;

    lines = 1;
    if (strlen(menuitem->description) > 0) {
        lines += 1;
    }

    return lines;
}

int
items_visible(Menu* menu, int offset)
{
    int maxheight;
    int maxwidth; // unused

    getmaxyx(menu->menu_win, maxheight, maxwidth);

    int vis = 0;
    int lines = 0;
    for (int i = offset ; i < menu->menu_length; i++) {

        lines += item_height(menu->menu_items[i]);
        if (lines > maxheight) break;
        vis += 1;

    }

    return vis;
}

int
items_visible_rev(Menu* menu, int offset)
{
    int maxheight;
    int maxwidth; // unused

    getmaxyx(menu->menu_win, maxheight, maxwidth);

    int vis = 0;
    int lines = 0;
    for (int i = offset; i > 0; i--) {

        lines += item_height(menu->menu_items[i]);
        if (lines > maxheight) break;
        vis +=1;

    }

    return vis;
}

int
free_menu(Menu* menu)
{
    return 0;
}

