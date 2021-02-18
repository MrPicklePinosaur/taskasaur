
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"
#include "config.h"

void render_step(BoardMenu* boardmenu);

int
main(int argc, char** argv)
{
    char* boardfile_name = "test_board.md"; 
    printf("%c]0;%s - %s%c", '\033', "taskasaur", boardfile_name, '\007'); // need to reset after program exits

    /* read from todo file */
    Board* board;
    board = begin_parse(boardfile_name);
    /* log_todo(board); */

    /* init curses */
    init_tscurses();

    BoardMenu* boardmenu;
    boardmenu = create_board_menu(board);

    /* need to render before user presses anything */
    render_step(boardmenu);

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
            case BINDING_MOVE_ITEM_LEFT:
                if (boardmenu->selected-1 < 0) break;
                {
                    Menu* from_menu;
                    Menu* to_menu;

                    from_menu = boardmenu->menu_list[boardmenu->selected],
                    to_menu = boardmenu->menu_list[boardmenu->selected-1],

                    insert_item(
                        to_menu,
                        get_menu_item(
                            from_menu,
                            get_selected_item(from_menu)
                        ),
                        min(
                            get_selected_item(from_menu),
                            get_menu_length(to_menu)
                        )
                    );
                    delete_item(
                        from_menu,
                        get_selected_item(from_menu) 
                    );
                    set_selected_menu(boardmenu, boardmenu->selected-1);
                }

                break;
            case BINDING_MOVE_ITEM_RIGHT:
                if (boardmenu->selected >= boardmenu->menu_count-1) break;
                // this is legit cpy paste please fix this
                {
                    Menu* from_menu;
                    Menu* to_menu;

                    from_menu = boardmenu->menu_list[boardmenu->selected],
                    to_menu = boardmenu->menu_list[boardmenu->selected+1],

                    insert_item(
                        to_menu,
                        get_menu_item(
                            from_menu,
                            get_selected_item(from_menu)
                        ),
                        min(
                            get_selected_item(from_menu),
                            get_menu_length(to_menu)
                        )
                    );
                    delete_item(
                        from_menu,
                        get_selected_item(from_menu) 
                    );
                    set_selected_menu(boardmenu, boardmenu->selected+1);
                }

                break;
            case BINDING_DELETE_ITEM:
                menu_driver(active_menu, MENU_DELETE);
                break;
            case BINDING_APPEND_ITEM:
                menu_driver(active_menu, MENU_APPEND);
                break;
            case BINDING_INSERT_ABOVE:
                menu_driver(active_menu, MENU_INSERT_ABOVE);
                break;
            case BINDING_INSERT_BELOW:
                menu_driver(active_menu, MENU_INSERT_BELOW);
                break;
            /* case BINDING_MOVE_MENU_LEFT: */
            /*     if (boardmenu->selected-1 < 0) break; */

            /*     swap_menu(boardmenu, boardmenu->selected, boardmenu->selected-1); */
                /* boardmenu->selected -= 1; */
                /* set_selected_menu(boardmenu, boardmenu->selected); */

                /* break; */
            /* case BINDING_MOVE_MENU_RIGHT: */
                /* if (boardmenu->selected >= boardmenu->menu_count-1) break; */
                /* swap_menu(boardmenu, boardmenu->selected, boardmenu->selected+1); */
                /* boardmenu->selected += 1; */
                /* set_selected_menu(boardmenu, boardmenu->selected); */

                /* break; */
            case BINDING_EDIT_ITEM:
                menu_driver(active_menu, MENU_EDIT);
                break;
            case BINDING_SELECT:
                
                break;
            case BINDING_WRITE:
                {
                    Board* writeboard;
                    writeboard = boardmenu_to_board(boardmenu);

                    begin_write(boardfile_name, writeboard);
                    free_board(writeboard);
                }

                break;
        }

        render_step(boardmenu);

    }

    exit_tscurses();
    return 0;    
}

void
render_step(BoardMenu* boardmenu)
{
        for (int i = 0; i < boardmenu->menu_count; i++) {

            Menu* curmenu = boardmenu->menu_list[i];

            /* update the descriptions - maybe not do this here */ 
            for (int j = 0; j < get_menu_length(curmenu); j++) {
                update_menuitem_descrip(get_menu_item(curmenu, j));
            }

            render_menu(curmenu);
        }
}
