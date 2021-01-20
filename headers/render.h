
#include <ncurses.h>

enum TaskasaurColors {
    TS_SELECTED,
    TS_NONSELECTED
};

/* the ts is used to not conflict with builtin names */
extern int init_tscurses(void);
extern int exit_tscurses(void);

