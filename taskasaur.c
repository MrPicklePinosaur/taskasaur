
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


    MenuItem** item_list = malloc(4*sizeof(MenuItem*));   
    for (int i = 0; i < 3; i++) {
        item_list[i] = create_menuitem("Many of you are probably feeling a little sad."); 
    }

    Menu* menu = create_menu(item_list);

    render_menu(menu);

    getch();

    exit_tscurses();

    /* int out; */
    /* printf("%s", wrap_text("ayylmaooooxdxdxdxdxd", 3, &out)); */
    /* printf("%d\n", out); */
    /* wrap_text("ayylmaooooxdxdxdxdxd", 3, &out); */
    

    return 0;    
}

