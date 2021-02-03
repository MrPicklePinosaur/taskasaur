
#ifndef __MENU_H__
#define __MENU_H__

#include <stdbool.h>

/* ncurses doesn't support multiline menu items,
 * so i wrote my own menu lib */

typedef enum {
    MENU_UP = 0,
    MENU_DOWN,
    MENU_TOP,
    MENU_BOTTOM,
    MENU_MOVE_UP,
    MENU_MOVE_DOWN,
    MENU_DELETE
} MenuAction;

typedef struct Menu Menu;
typedef struct MenuItem MenuItem;

extern MenuItem* create_menuitem(char* contents);

extern Menu* create_menu(char* menu_name, MenuItem** item_list);
extern int set_menu_win(Menu* menu, WINDOW* win);
extern MenuItem* get_menu_items(Menu* menu);
extern int set_menu_focus(Menu* menu, bool focus);

extern int menu_driver(Menu* menu, MenuAction action);

extern int render_menu(Menu* menu);

extern int free_menu(Menu* menu);

#endif
