
#include "headers/render.h"
#include <string.h>
#include "config.h"

#define POPUP_MENU_PAD_TOP 4
#define POPUP_MENU_PAD_BOTTOM 2
#define POPUP_MENU_PAD_LEFT 2
#define POPUP_MENU_PAD_RIGHT 1

int init_tscolors(void);
int create_todowin(void);

/* menu render callbacks */
void render_menuitem(Menu* menu, int item_index, int start_y);
int menuitem_height(MenuItem* menuitem);

void render_popup_menuitem(Menu* menu, int item_index, int start_y);
int popup_menuitem_height(MenuItem* menuitem);


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
    init_pair(TS_ITEMCOUNT, item_count_color, COLOR_BLACK);

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
    new_boardmenu->popupmenu = NULL;
    new_boardmenu->popup_open = 0;

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

        /* set menu render callbacks */
        set_menu_renderitem(new_menu, *render_menuitem);
        set_menu_itemheight(new_menu, *menuitem_height);

        /* refresh */
        refresh();
        wrefresh(win);

        menu_list[i] = new_menu;
    }

    return menu_list;
}

void
render_menuitem(Menu* menu, int item_index, int start_y)
{
    MenuItem* curitem;
    WINDOW* menu_win;
    TodoItem* item_data;
    int hlcolor;

    curitem = get_menu_item(menu, item_index);
    menu_win = get_menu_win(menu);
    item_data = (TodoItem*)get_menuitem_userdata(curitem);

    /* color selected item */
    hlcolor = COLOR_PAIR((item_index == get_selected_item(menu) && get_menu_focused(menu) == true) ? TS_SELECTED : TS_NONSELECTED);
    wattron(menu_win, hlcolor);
    mvwprintw(menu_win, start_y, 0, get_menuitem_title(curitem));
    wattroff(menu_win, hlcolor);

    /* item tag line */
    wmove(menu_win, start_y+1, 0);
    wattron(menu_win, COLOR_PAIR(TS_ITEMCOUNT));
    /* display tiny character to indicate item has a descrip */
    if (strlen(item_data->description) > 0) {
        wprintw(menu_win, "# "); 
    }
    /* display number of complete tasks */
    if (item_data->subtask_count > 0) {
        int tasks_complete = 0;
        for (int i = 0; i < item_data->subtask_count; i++) {
            if (item_data->subtask_list[i]->done == SubTaskState_done)
                tasks_complete += 1;
        }
        wprintw(menu_win, "[%d/%d]", tasks_complete, item_data->subtask_count);
    }
    wattroff(menu_win, COLOR_PAIR(TS_ITEMCOUNT));
}

int
menuitem_height(MenuItem* menuitem)
{
    TodoItem* item_data;
    int lines;

    item_data = get_menuitem_userdata(menuitem);
    lines = 1;
    if (item_data->subtask_count > 0 || strlen(item_data->description) > 0) {
        lines += 1;
    }

    return lines;
}

MenuItem** 
todolist_to_menuitem(TodoItem** item_list, int list_length)
{
    MenuItem** items;

    items = malloc((list_length+1)*sizeof(MenuItem*));
    for (int i = 0; i < list_length; i++) {
        MenuItem* new_menuitem;
        new_menuitem =  create_menuitem(item_list[i]->item_name);
        /* using same struct, careful if it gets freed */
        set_menuitem_userdata(new_menuitem, item_list[i]);

        items[i] = new_menuitem;
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
            TodoItem* itemdata = get_menuitem_userdata(curmenuitem);

            TodoItem* new_todoitem = malloc(sizeof(TodoItem));
            SubTask** new_subtask_list = malloc(itemdata->subtask_count*sizeof(SubTask*));

            new_todoitem->item_name = strdup(get_menuitem_title(curmenuitem));
            new_todoitem->description = strdup(itemdata->description);
            new_todoitem->due = strdup(itemdata->due);
            new_todoitem->subtask_count = itemdata->subtask_count;
            for (int k = 0; k < itemdata->subtask_count; k++) {
                SubTask* new_subtask = malloc(sizeof(SubTask));

                new_subtask->subtask_name = itemdata->subtask_list[k]->subtask_name;
                new_subtask->done = itemdata->subtask_list[k]->done;

                new_subtask_list[k] = new_subtask;
            }

            new_todoitem->subtask_list = new_subtask_list;

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
render_board(Board* board)
{

    return 0;
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
    if (new_pos < 0) new_pos = 0;
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
PopupMenu*
make_popupmenu(TodoItem* itemdata)
{
    PopupMenu* new_popupmenu;
    MenuItem** subtask_menuitems;
    Menu* popupmenu_menu;
    WINDOW* popupmenu_win;
    WINDOW* popupmenu_menu_win;

    new_popupmenu = malloc(sizeof(PopupMenu));

    subtask_menuitems = subtasklist_to_menuitem(itemdata->subtask_list, itemdata->subtask_count);
    popupmenu_menu = create_menu(strdup(""), subtask_menuitems);

    /* popup win */
    int maxheight, maxwidth;
    getmaxyx(stdscr, maxheight, maxwidth);
    popupmenu_win = newwin(
        maxheight-2*POPUP_BORDER,
        maxwidth-2*2*POPUP_BORDER,
        POPUP_BORDER,
        POPUP_BORDER*2
    );

    int popup_maxheight, popup_maxwidth;
    getmaxyx(popupmenu_win, popup_maxheight, popup_maxwidth);
    popupmenu_menu_win = derwin(
        popupmenu_win,
        popup_maxheight-POPUP_MENU_PAD_TOP-POPUP_MENU_PAD_BOTTOM,
        popup_maxwidth-POPUP_MENU_PAD_LEFT-POPUP_MENU_PAD_RIGHT,
        POPUP_MENU_PAD_TOP,
        POPUP_MENU_PAD_LEFT
    );

    set_menu_win(popupmenu_menu, popupmenu_menu_win);
    set_menu_userdata(popupmenu_menu, itemdata);
    set_menu_focus(popupmenu_menu, 1);
    set_menu_renderitem(popupmenu_menu, render_popup_menuitem);
    set_menu_itemheight(popupmenu_menu, popup_menuitem_height);
    box(popupmenu_win, 0, 0);

    /* move this stuff to render phase later? */
    mvwprintw(popupmenu_win, 1, 2, itemdata->item_name);
    mvwprintw(popupmenu_win, 2, 2, (strlen(itemdata->description) > 0) ? itemdata->description : "no description");

    /* don't forget to free popupmenu after */
    new_popupmenu->win = popupmenu_win;
    new_popupmenu->menu = popupmenu_menu;

    return new_popupmenu;
}

void
render_popup_menuitem(Menu* menu, int item_index, int start_y)
{
    MenuItem* curitem;
    WINDOW* menu_win;
    int hlcolor;

    curitem = get_menu_item(menu, item_index);
    menu_win = get_menu_win(menu);

    /* color selected item */
    hlcolor = COLOR_PAIR((item_index == get_selected_item(menu) && get_menu_focused(menu) == true) ? TS_SELECTED : TS_NONSELECTED);
    wattron(menu_win, hlcolor);

    wmove(menu_win, start_y, 0);
    /* print subtask done indicator */
    if (strlen(get_menuitem_title(curitem)) > 0)
        wprintw(
            menu_win,
            (((SubTask*)get_menuitem_userdata(curitem))->done == SubTaskState_done) ? "[X] " : "[ ] "
            /* (((TodoItem*)get_menu_userdata(menu))->subtask_list[item_index]->done == SubTaskState_done) ? "[X] " : "[ ] " */
        );
    wprintw(menu_win, get_menuitem_title(curitem));
    /* wprintw(menu_win, ((TodoItem*)get_menu_userdata(menu))->subtask_list[item_index]->subtask_name); */

    wattroff(menu_win, hlcolor);
}

int
popup_menuitem_height(MenuItem* menuitem)
{
    return 1; // account for wrap later
}

/* save menu state to userdata */
/* sorta duct tape rn */
/* find a way to tie TodoItem and MenuItem together better in the future */
void
close_popupmenu(Menu* popupmenu_menu)
{
    TodoItem* item_data;
    SubTask** updated_subtasklist;

    item_data = (TodoItem*)get_menu_userdata(popupmenu_menu);
    updated_subtasklist = menuitem_to_subtasklist(
        get_menu_itemlist(popupmenu_menu),
        get_menu_length(popupmenu_menu)
    );

    /* remember to free old subtask list */
    item_data->subtask_list = updated_subtasklist;
    item_data->subtask_count = get_menu_length(popupmenu_menu);

    /* this is messy but since subtask rendering uses menuitem title instead
     * of userdata subtask title, this needs to be done :( */ 
    /* for (int i = 0; i < get_menu_length(popupmenu_menu); i++) { */
    /*     set_menuitem_title( */
    /*         get_menu_item(popupmenu_menu, i), */
    /*         strdup(item_data->subtask_list[i]->subtask_name) */
    /*     ); */
    /* } */
}

/* this is copy paste of other, prob abstract */
MenuItem**
subtasklist_to_menuitem(SubTask** subtask_list, int list_length)
{
    MenuItem** items;

    items = malloc((list_length+1)*sizeof(MenuItem*));
    for (int i = 0; i < list_length; i++) {
        MenuItem* new_menuitem;
        new_menuitem =  create_menuitem(subtask_list[i]->subtask_name);
        /* using same struct, careful if it gets freed */
        set_menuitem_userdata(new_menuitem, subtask_list[i]);

        items[i] = new_menuitem;
    }

    items[list_length] = 0; //null terminate
    return items;
}

SubTask**
menuitem_to_subtasklist(MenuItem** menuitem_list, int list_length)
{
    SubTask** new_subtasklist;

    new_subtasklist = malloc(list_length*sizeof(SubTask*));

    for (int i = 0; i < list_length; i++) {
        SubTask* cur_subtask;
        SubTask* new_subtask;

        cur_subtask = (SubTask*)get_menuitem_userdata(menuitem_list[i]);
        new_subtask = malloc(sizeof(SubTask));

        new_subtask->subtask_name = strdup(cur_subtask->subtask_name);
        new_subtask->done = cur_subtask->done;

        new_subtasklist[i] = new_subtask;
    }

    /* new_subtasklist[list_length] = 0; */

    return new_subtasklist;
}


/* helpers */
int
ungetstr(char* str)
{
    // ignore null character (it's fine even if strlen = 0)
    for (int i = strlen(str)-1; i >= 0; i--) {
        ungetch(str[i]);
    }

    return 0; 
}

