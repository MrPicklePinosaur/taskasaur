/* ncurses doesn't support multiline menu items,
 * so i wrote my own menu lib */

typedef struct Menu Menu;
typedef struct MenuItem MenuItem;

extern Menu* create_menu(MenuItem* item_list);
extern int set_menu_items(Menu* menu, MenuItem* item_list);
extern MenuItem* get_menu_items(Menu* menu);
