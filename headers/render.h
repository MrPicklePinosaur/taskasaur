
#ifndef __RENDER_H__
#define __RENDER_H__

#include <ncurses.h>

enum TaskasaurColors {
    TS_SELECTED = 1,
    TS_NONSELECTED,
    TS_MENU_SELECTED,
    TS_MENU_NONSELECTED,
};

/* the ts is used to not conflict with builtin names */
extern int init_tscurses(void);
extern int exit_tscurses(void);

#endif
