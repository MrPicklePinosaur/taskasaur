
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"
#include "config.h"

int
main(int argc, char** argv)
{
    char ch;
    /* Board* board; */
    /* board = begin_parse("test_board.md"); */
    /* log_todo(board); */

    /* init curses */
    init_tscurses();

    MenuItem** item_list = malloc(5*sizeof(MenuItem*));   
    item_list[0] = create_menuitem("Many of you are probably feeling a little sad."); 
    item_list[1] = create_menuitem("This is ok. Sadness is a normal human emotion."); 
    item_list[2] = create_menuitem("I encourage you to watch the movie Inside Out"); 
    item_list[3] = create_menuitem("one of the best movies of all time."); 
    item_list[4] = 0;

    Menu* menu = create_menu(item_list);
    WINDOW* win = newwin(20, 40, 5, 5);
    set_menu_win(menu, win);
    set_menu_focus(menu, true);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    render_menu(menu);

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

