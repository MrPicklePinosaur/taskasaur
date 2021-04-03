
#ifndef __MENU_H__
#define __MENU_H__

#include <stdbool.h>
#include <ncurses.h>

/* ncurses doesn't support multiline menu items,
 * so i wrote my own menu lib */

typedef enum {
    MENU_UP = 0,
    MENU_DOWN,
    MENU_TOP,
    MENU_BOTTOM,
    MENU_MOVE_UP,
    MENU_MOVE_DOWN,
    MENU_DELETE,
    MENU_APPEND,
    MENU_INSERT_ABOVE,
    MENU_INSERT_BELOW,
    MENU_EDIT
} MenuAction;

typedef struct Menu Menu;
typedef struct MenuItem MenuItem;

extern MenuItem* create_menuitem(char* title);

extern Menu* create_menu(char* menu_name, MenuItem** item_list);

extern WINDOW* get_menu_win(Menu* menu);
extern MenuItem* get_menu_item(Menu* menu, int index);
extern MenuItem** get_menu_itemlist(Menu* menu);
extern int get_selected_item(Menu* menu); // rename this to selected_index or sm
extern MenuItem* get_selected_menuitem(Menu* menu);
extern bool get_menu_focused(Menu* menu);
extern int get_menu_length(Menu* menu);
extern char* get_menu_name(Menu* menu);
extern void* get_menu_userdata(Menu* menu);
extern char* get_menuitem_title(MenuItem* menuitem);
extern char* get_menuitem_descrip(MenuItem* menuitem);
extern void* get_menuitem_userdata(MenuItem* menuitem);

extern int set_menu_win(Menu* menu, WINDOW* win);
extern int set_selected_item(Menu* menu, int selected_item);
extern int set_menu_focus(Menu* menu, bool focus);
extern int set_menu_userdata(Menu* menu, void* userdata);
extern int set_menu_renderitem(Menu* menu, void (*render_item)(Menu*, int, int));
extern int set_menu_itemheight(Menu* menu, int (*item_height)(MenuItem*));
extern int set_menuitem_title(MenuItem* menuitem, char* title);
extern int set_menuitem_descrip(MenuItem* menuitem, char* descrip);
extern int set_menuitem_userdata(MenuItem* menuitem, void* userdata);

extern int insert_item(Menu* menu, MenuItem* menuitem, int index);
extern int delete_item(Menu* menu, int index);

extern int menu_driver(Menu* menu, MenuAction action);

extern int render_menu(Menu* menu);

extern int free_menu(Menu* menu);

#endif
