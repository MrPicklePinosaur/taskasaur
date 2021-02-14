
#include "headers/parser.h"
#include "headers/render.h"
#include "headers/menu.h"
#include "headers/utils.h"
#include "config.h"

// this is temp
#define MENU_WIDTH 40

typedef struct BoardMenu {
    Menu** menu_list;
    int menu_count;
    int selected;
} BoardMenu;

BoardMenu* create_board_menu(Board* board);
Board* boardmenu_to_board(BoardMenu* boardmenu);
int set_selected_menu(BoardMenu* boardmenu, int index);

MenuItem** todolist_to_menuitem(TodoItem** item_list, int list_length);
Menu** make_menus(Board* board, int todolist_length);
int swap_menu(BoardMenu* boardmenu, int src_index, int dest_index);

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

Board*
boardmenu_to_board(BoardMenu* boardmenu)
{
    Board newboard = malloc(sizeof(Board));
    TodoList** todolist_list = malloc(sizeof(TodoList*));

    for (int i = 0; i < boardmenu->menu_count; i++) {
        Menu* curmenu = boardmenu->menu_list[i];

        TodoList* new_todolist = malloc(sizeof(TodoList));
        TodoItem** item_list = malloc(sizeof(TodoItem*));
        new_todolist->list_name = get_menu_name(curmenu);
        new_todolist->item_count = get_menu_length(curmenu);

        /* for (int j = 0; j < get_menu_length(curmenu); j++) { */
             
        /* } */

        new_todolist->item_list = item_list;

    }

    newboard->todolist_list = todolist_list;
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
    ar_swap_item(boardmenu->menu_list, src_index, dest_index);

    return 0;
}

