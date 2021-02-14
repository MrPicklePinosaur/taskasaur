
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

extern MenuItem* create_menuitem(char* contents);

extern Menu* create_menu(char* menu_name, MenuItem** item_list);
extern WINDOW* get_menu_win(Menu* menu);
extern int set_menu_win(Menu* menu, WINDOW* win);
extern WINDOW* get_menu_subwin(Menu* menu);
extern MenuItem* get_menu_item(Menu* menu, int index);
extern int set_menu_focus(Menu* menu, bool focus);
extern int get_selected_item(Menu* menu);
extern int set_selected_item(Menu* menu, int selected_item);
extern int get_menu_length(Menu* menu);
extern char* get_menu_name(Menu* menu);

extern int insert_item(Menu* menu, MenuItem* menuitem, int index);
extern int delete_item(Menu* menu, int index);

extern int menu_driver(Menu* menu, MenuAction action);

extern int render_menu(Menu* menu);

extern int free_menu(Menu* menu);

#endif
