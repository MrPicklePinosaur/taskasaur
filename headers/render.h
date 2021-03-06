
#ifndef __RENDER_H__
#define __RENDER_H__

#include <ncurses.h>
#include "parser.h"
#include "menu.h"
#include "utils.h"

// this is temp
#define MENU_WIDTH 40
#define POPUP_BORDER 2

enum TaskasaurColors {
    TS_SELECTED = 1,
    TS_NONSELECTED,
    TS_MENU_SELECTED,
    TS_MENU_NONSELECTED,
    TS_ITEMCOUNT
};

typedef struct PopupMenu {
    WINDOW* win;
    Menu* menu;
} PopupMenu;

typedef struct BoardMenu {
    Menu** menu_list;
    int menu_count;
    int selected;
    PopupMenu* popupmenu;
    int popup_open;
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
extern int render_board(Board* board);

extern int set_selected_menu(BoardMenu* boardmenu, int index);
extern int swap_menu(BoardMenu* boardmenu, int src_index, int dest_index);

/* menuitem stuff */
extern int update_menuitem_descrip(MenuItem* menuitem);

/* popup menu for menu item */
extern PopupMenu* make_popupmenu(TodoItem* itemdata);
extern WINDOW* create_popup_win(TodoItem* item_info);
extern void close_popupmenu(Menu* popupmenu_menu);
extern MenuItem** subtasklist_to_menuitem(SubTask** subtask_list, int list_length);
extern SubTask** menuitem_to_subtasklist(MenuItem** menuitem_list, int list_length);

/* some helpers */
extern int ungetstr(char* str);


#endif
