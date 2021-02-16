
#include "headers/render.h"
#include <string.h>
#include "config.h"

int init_tscolors(void);
int create_todowin(void);

/* init stuff */
int
init_tscurses(void)
{
    initscr();
    cbreak();
    curs_off();
    keypad(stdscr, TRUE);

    /* need to error check this */
    if (has_colors() == FALSE) {
        fprintf(stderr, "Your terminal does not support color.\n");

        /* maybe just return 1 */
        /* exit_tscurses(); */
        /* exit(1); */
        return 1;
    }
    start_color();
    init_tscolors();

    return 0;
}

int
exit_tscurses(void)
{
    endwin();

    return 0;
}

int
init_tscolors(void)
{
    init_pair(TS_SELECTED, selected_color, COLOR_BLACK);
    init_pair(TS_NONSELECTED, non_selected_color, COLOR_BLACK);
    init_pair(TS_MENU_SELECTED, menu_selected_color, COLOR_BLACK);
    init_pair(TS_MENU_NONSELECTED, menu_non_selected_color, COLOR_BLACK);

    return 0;
}

/* cursor */
int
curs_on(void)
{
    echo();
    curs_set(1);
    return 0;
}

int
curs_off(void)
{
    noecho();
    curs_set(0);
    return 0;
}

/* wins */
int
create_todowin(void)
{
    return 0;
}

/* board menu */
BoardMenu*
create_board_menu(Board* board)
{
    BoardMenu* new_boardmenu;

    new_boardmenu = malloc(sizeof(BoardMenu));

    new_boardmenu->menu_list = make_menus(board, board->todolist_count);
    new_boardmenu->menu_count = board->todolist_count;
    new_boardmenu->selected = 0;

    return new_boardmenu;
}

Menu**
make_menus(Board* board, int todolist_length)
{

    Menu** menu_list;

    menu_list = malloc(todolist_length*sizeof(Menu*));

    for (int i = 0; i < todolist_length; i++) {

        /* read from parsed */
        TodoList* todo_list = board->todolist_list[i];
        MenuItem** item_list = todolist_to_menuitem(todo_list->item_list, todo_list->item_count);

        Menu* new_menu = create_menu(todo_list->list_name, item_list);

        /* make window */
        WINDOW* win = newwin(20, MENU_WIDTH, 1, 1+MENU_WIDTH*i);
        box(win, 0, 0);

        /* some menu settings */
        set_menu_win(new_menu, win);
        set_menu_focus(new_menu, i == 0); // make first win focused

        /* refresh */
        refresh();
        wrefresh(win);

        menu_list[i] = new_menu;
    }

    return menu_list;
}

MenuItem** 
todolist_to_menuitem(TodoItem** item_list, int list_length)
{
    MenuItem** items;

    items = malloc((list_length+1)*sizeof(MenuItem*));
    for (int i = 0; i < list_length; i++) {
        items[i] = create_menuitem(item_list[i]->item_name);
    }

    items[list_length] = 0; //null terminate
    return items;
}

Board*
boardmenu_to_board(BoardMenu* boardmenu)
{ // STRINGS are sharing the same address as the one in MENU
  // and MENUITEM, this may break something if u free this board
  // consider copying the string

    Board* newboard = malloc(sizeof(Board));
    TodoList** new_todolist_list = malloc(sizeof(TodoList*));

    for (int i = 0; i < boardmenu->menu_count; i++) {
        Menu* curmenu = boardmenu->menu_list[i];

        TodoList* new_todolist = malloc(sizeof(TodoList));
        TodoItem** new_item_list = malloc(sizeof(TodoItem*));
        new_todolist->list_name = strdup(get_menu_name(curmenu));
        new_todolist->item_count = get_menu_length(curmenu);

        for (int j = 0; j < get_menu_length(curmenu); j++) {
            MenuItem* curmenuitem = get_menu_item(curmenu, j);

            TodoItem* new_todoitem = malloc(sizeof(TodoItem));

            new_todoitem->item_name = strdup(get_menuitem_title(curmenuitem));
            new_todoitem->description = strdup(get_menuitem_descrip(curmenuitem));
            new_todoitem->due = strdup(""); //TEMP! 
            new_todoitem->subtask_list = malloc(0); //TEMP!
            new_todoitem->subtask_count = 0; //TEMP!

            new_item_list[j] = new_todoitem;
        }

        new_todolist->item_list = new_item_list;
        new_todolist_list[i] = new_todolist;

    }

    newboard->board_name = strdup("");
    newboard->todolist_list = new_todolist_list;
    newboard->todolist_count = boardmenu->menu_count;

    return newboard;
}

int
set_selected_menu(BoardMenu* boardmenu, int index)
{
    Menu* old_menu;
    Menu* new_menu;
    int new_pos;
    
    old_menu = boardmenu->menu_list[boardmenu->selected];
    new_menu = boardmenu->menu_list[index];

    set_menu_focus(old_menu, false);
    set_menu_focus(new_menu, true);

    /* also try to jump to a similar position if possible */
    /* rn theres a bug if old menu is empty */
    new_pos = min(get_selected_item(old_menu), get_menu_length(new_menu)-1);
    set_selected_item(new_menu, new_pos);

    boardmenu->selected = index;

    return 0;
}

int
swap_menu(BoardMenu* boardmenu, int src_index, int dest_index)
{
    /* reposition menus */
    mvwin(get_menu_win(boardmenu->menu_list[src_index]),
        1, 1+MENU_WIDTH*dest_index
    );
    mvwin(get_menu_win(boardmenu->menu_list[dest_index]),
        1, 1+MENU_WIDTH*src_index
    );
    refresh();
    wrefresh(get_menu_win(boardmenu->menu_list[src_index]));
    wrefresh(get_menu_win(boardmenu->menu_list[dest_index]));
    /* wclear(get_menu_win(boardmenu->menu_list[src_index])); */
    /* wclear(get_menu_win(boardmenu->menu_list[dest_index])); */
    /* touchwin(get_menu_win(boardmenu->menu_list[src_index])); */
    /* touchwin(get_menu_win(boardmenu->menu_list[dest_index])); */
    clear();

    /* swap in array */
    ar_swap_item((void*)boardmenu->menu_list, src_index, dest_index);

    return 0;
}

/* popup */
WINDOW*
create_popup_win()
{
    return NULL;
}


int
ungetstr(char* str)
{
    // ignore null character (it's fine even if strlen = 0)
    for (int i = strlen(str)-1; i >= 0; i--) {
        ungetch(str[i]);
    }

    return 0; 
}

