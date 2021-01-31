
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

    MenuItem** item_list = malloc(6*sizeof(MenuItem*));   
    for (int i = 0; i < 5; i++) {
        item_list[i] = create_menuitem("Many of you are probably feeling a little sad."); 
    }
    item_list[5] = 0;

    Menu* menu = create_menu(item_list);
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
        }
        render_menu(menu);

    }


    exit_tscurses();
    return 0;    
}

