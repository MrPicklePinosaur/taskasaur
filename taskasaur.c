
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"

int
main(int argc, char** argv)
{
    /* Board* board; */
    /* board = begin_parse("test_board.md"); */
    /* log_todo(board); */

    /* init curses */
    init_tscurses();

    getch();

    MenuItem** item_list = malloc(4*sizeof(MenuItem*));   
    for (int i = 0; i < 3; i++) {
        item_list[i] = create_menuitem("lmao");
    }
    item_list[3] = 0;

    Menu* menu = create_menu(item_list);

    exit_tscurses();
    

    return 0;    
}

