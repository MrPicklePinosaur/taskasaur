
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

    exit_tscurses();
    

    return 0;    
}

