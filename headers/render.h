
#ifndef __RENDER_H__
#define __RENDER_H__

#include <ncurses.h>
#include "parser.h"
#include "menu.h"
#include "utils.h"

// this is temp
#define MENU_WIDTH 40

enum TaskasaurColors {
    TS_SELECTED = 1,
    TS_NONSELECTED,
    TS_MENU_SELECTED,
    TS_MENU_NONSELECTED,
};

typedef struct BoardMenu {
    Menu** menu_list;
    int menu_count;
    int selected;
} BoardMenu;

/* the ts is used to not conflict with builtin names */
extern int init_tscurses(void);
extern int exit_tscurses(void);

extern int curs_on(void);
extern int curs_off(void);

/* board menu */
extern BoardMenu* create_board_menu(Board* board);
extern Menu** make_menus(Board* board, int todolist_length);
extern MenuItem** todolist_to_menuitem(TodoItem** item_list, int list_length);
extern Board* boardmenu_to_board(BoardMenu* boardmenu);

extern int set_selected_menu(BoardMenu* boardmenu, int index);
extern int swap_menu(BoardMenu* boardmenu, int src_index, int dest_index);

/* popup menu for menu item */
extern WINDOW* create_popup_win();

/* some helpers */
extern int ungetstr(char* str);


#endif
