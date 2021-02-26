
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
    WINDOW* sub_win;
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
int items_visible(Menu* menu);

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
    mvwgetnstr(menu->sub_win,
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
    /* draw outer menu (prob dont need this every render) */ 
    /* wclear(menu->menu_win); */
    wattron(menu->menu_win, COLOR_PAIR(
        (menu->focused == true) ?
        TS_MENU_SELECTED: TS_MENU_NONSELECTED       
    ));
    mvwprintw(menu->menu_win, 0, MENU_PAD_LEFT, menu->menu_name);
    wattroff(menu->menu_win, COLOR_PAIR(0));

    /* char buf[20]; */
    /* sprintf(buf, "%d", items_visible(menu)); */
    /* mvprintw(20, 20, buf); */

    /* calculate scroll */
    int visible;

    visible = items_visible(menu);

    if (menu->selected_item > menu->scroll_offset+visible) {
        // may be dangerous, assumes render after every action
        menu->scroll_offset += 1;
    } else if (menu->selected_item < menu->scroll_offset) {
        menu->scroll_offset = menu->scroll_offset-1;
        if (menu->scroll_offset < 0) menu->scroll_offset = 0;
    }

    /* draw inner menu */
    wclear(menu->sub_win);

    int curline = 0;
    for (int i = menu->scroll_offset; i < menu->menu_length; i++) {
        curline += render_item(menu, i, curline);
    }

    wrefresh(menu->sub_win);
    wrefresh(menu->menu_win);

    return 0;
}

int
render_item(Menu* menu, int item_index, int start_y)
{
    MenuItem* curitem;
    curitem = menu->menu_items[item_index];

    /* color selected item */
    wattron(menu->sub_win, COLOR_PAIR(
       (item_index == menu->selected_item && menu->focused == true) ? 
       TS_SELECTED : TS_NONSELECTED
    ));
    mvwprintw(menu->sub_win, start_y, 0, curitem->title);
    wattroff(menu->sub_win, COLOR_PAIR(0));

    /* display number of items */
    if (strlen(curitem->description) > 0) {
        mvwprintw(menu->sub_win, start_y+1, 0, curitem->description); 
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
items_visible(Menu* menu)
{
    int maxheight;
    int maxwidth; // unused

    getmaxyx(menu->sub_win, maxheight, maxwidth);

    int i = menu->scroll_offset;
    int lines = 0;
    for (; i < menu->menu_length; i++) {

        lines += item_height(menu->menu_items[i]);

        if (lines >= maxheight) break;

    }

    return i;
}

int
free_menu(Menu* menu)
{
    return 0;
}

