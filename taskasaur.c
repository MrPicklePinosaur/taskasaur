
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"
#include "config.h"

MenuItem** todolist_to_menuitem(TodoItem** item_list, int list_length);

int
main(int argc, char** argv)
{

    /* read from todo file */
    Board* board;
    board = begin_parse("test_board.md");
    log_todo(board);

    /* init curses */
    init_tscurses();

    TodoList* todo_list = board->todolist_list[0];
    MenuItem** item_list = todolist_to_menuitem(todo_list->item_list, todo_list->item_count);
    Menu* menu = create_menu(todo_list->list_name, item_list);
    WINDOW* win = newwin(20, 40, 5, 5);
    set_menu_win(menu, win);
    set_menu_focus(menu, true);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    render_menu(menu);

    char ch;
    while ((ch = getch()) != BINDING_QUIT) {

        switch (ch) {

            case BINDING_SCROLL_UP:
                menu_driver(menu, MENU_UP);
                break;
            case BINDING_SCROLL_DOWN:
                menu_driver(menu, MENU_DOWN);
                break;
            case BINDING_JUMP_TOP:
                menu_driver(menu, MENU_TOP);
                break;
            case BINDING_JUMP_BOTTOM:
                menu_driver(menu, MENU_BOTTOM);
                break;
            case BINDING_MOVE_ITEM_UP:
                menu_driver(menu, MENU_MOVE_UP);
                break;
            case BINDING_MOVE_ITEM_DOWN:
                menu_driver(menu, MENU_MOVE_DOWN);
                break;
            case BINDING_DELETE_ITEM:
                menu_driver(menu, MENU_DELETE);
                break;

        }
        render_menu(menu);

    }


    exit_tscurses();
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



