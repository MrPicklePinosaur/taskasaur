
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <ncurses.h>

static char font[] = "Source Code Pro:size=12";

/* COLORS */

/* Use the ncurses defined colors, here's a list of them:
 *      COLOR_BLACK
 *      COLOR_RED
 *      COLOR_GREEN
 *      COLOR_YELLOW
 *      COLOR_BLUE
 *      COLOR_MAGENTA
 *      COLOR_CYAN
 *      COLOR_WHITE
*/

static int selected_color = COLOR_MAGENTA;
static int non_selected_color = COLOR_WHITE;
static int menu_selected_color = COLOR_CYAN;
static int menu_non_selected_color = COLOR_GREEN;

/* KEYS */
#define BINDING_QUIT 'q'
#define BINDING_SCROLL_UP 'k'
#define BINDING_SCROLL_DOWN 'j'
#define BINDING_SCROLL_LEFT 'h'
#define BINDING_SCROLL_RIGHT 'l'
#define BINDING_JUMP_TOP 'g'
#define BINDING_JUMP_BOTTOM 'G'
#define BINDING_MOVE_ITEM_UP 'K'
#define BINDING_MOVE_ITEM_DOWN 'J'
#define BINDING_MOVE_ITEM_LEFT 'H'
#define BINDING_MOVE_ITEM_RIGHT 'L'
#define BINDING_DELETE_ITEM 'D'
#define BINDING_APPEND_ITEM 'a'
#define BINDING_INSERT_ABOVE 'O'
#define BINDING_INSERT_BELOW 'o'
#define BINDING_MOVE_MENU_LEFT '<'
#define BINDING_MOVE_MENU_RIGHT '>'
#define BINDING_EDIT_ITEM 'i'
#define BINDING_SELECT '\n'
#define BINDING_WRITE 'w'

#endif
