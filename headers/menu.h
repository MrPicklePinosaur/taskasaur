
#ifndef __MENU_H__
#define __MENU_H__

/* ncurses doesn't support multiline menu items,
 * so i wrote my own menu lib */

typedef struct Menu Menu;
typedef struct MenuItem MenuItem;

extern MenuItem* create_menuitem(char* contents);

extern Menu* create_menu(MenuItem** item_list);
extern int set_menu_items(Menu* menu, MenuItem** item_list);
extern int set_menu_win(Menu* menu, WINDOW* win);
extern MenuItem* get_menu_items(Menu* menu);

extern int render_menu(Menu* menu);

extern int free_menu(Menu* menu);

#endif
