
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"
#include "config.h"

typedef struct BoardMenu {
    Menu** menu_list;
    int menu_count;
    int selected;
} BoardMenu;

BoardMenu* create_board_menu(Board* board);
int set_selected_menu(BoardMenu* boardmenu, int index);

MenuItem** todolist_to_menuitem(TodoItem** item_list, int list_length);
Menu** make_menus(Board* board, int todolist_length);

int
main(int argc, char** argv)
{
    /* read from todo file */
    Board* board;
    board = begin_parse("test_board.md");
    /* log_todo(board); */

    /* init curses */
    init_tscurses();

    BoardMenu* boardmenu;
    boardmenu = create_board_menu(board);

    // this is temp
    for (int i = 0; i < boardmenu->menu_count; i++) {
        render_menu(boardmenu->menu_list[i]);
    }

    char ch;
    while ((ch = getch()) != BINDING_QUIT) {

        Menu* active_menu;
        active_menu = boardmenu->menu_list[boardmenu->selected];

        switch (ch) {

            case BINDING_SCROLL_UP:
                menu_driver(active_menu, MENU_UP);
                break;
            case BINDING_SCROLL_DOWN:
                menu_driver(active_menu, MENU_DOWN);
                break;
            case BINDING_SCROLL_LEFT:
                if (boardmenu->selected-1 < 0) break;
                set_selected_menu(boardmenu, boardmenu->selected-1);
                break;
            case BINDING_SCROLL_RIGHT:
                if (boardmenu->selected+1 > boardmenu->menu_count-1) break;
                set_selected_menu(boardmenu, boardmenu->selected+1);
                break;
            case BINDING_JUMP_TOP:
                menu_driver(active_menu, MENU_TOP);
                break;
            case BINDING_JUMP_BOTTOM:
                menu_driver(active_menu, MENU_BOTTOM);
                break;
            case BINDING_MOVE_ITEM_UP:
                menu_driver(active_menu, MENU_MOVE_UP);
                break;
            case BINDING_MOVE_ITEM_DOWN:
                menu_driver(active_menu, MENU_MOVE_DOWN);
                break;
            case BINDING_DELETE_ITEM:
                menu_driver(active_menu, MENU_DELETE);
                break;
            case BINDING_APPEND_ITEM:
                menu_driver(active_menu, MENU_APPEND);
                break;
        }

        for (int i = 0; i < boardmenu->menu_count; i++) {
            render_menu(boardmenu->menu_list[i]);
        }


    }

    exit_tscurses();
    return 0;    
}

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
    new_pos = (get_selected_item(old_menu) > get_menu_length(new_menu)-1) ?
        get_menu_length(new_menu)-1 : get_selected_item(old_menu);
    set_selected_item(new_menu, new_pos);

    boardmenu->selected = index;

    return 0;
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

Menu**
make_menus(Board* board, int todolist_length)
{
    // this is temp
    #define MENU_WIDTH 40

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
