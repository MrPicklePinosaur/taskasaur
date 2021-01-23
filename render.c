
#include "headers/render.h"
#include "config.h"

int init_tscolors(void);

int create_todowin(void);

/* init stuff */
int
init_tscurses(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
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

    return 0;
}

/* wins */
int
create_todowin(void)
{



    return 0;
}
