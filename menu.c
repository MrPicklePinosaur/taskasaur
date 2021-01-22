
#include <stddef.h>

#include "headers/menu.h"

typedef struct MenuItem {
    char* contents;
} MenuItem;

typedef struct Menu {
    MenuItem* menu_items;
    int menu_length;
    int selected_item;
} Menu;


Menu* 
create_menu(MenuItem* item_list)
{
    return NULL;
}

int
set_menu_items(Menu* menu, MenuItem* item_list)
{
    return 0;
}

MenuItem*
get_menu_items(Menu* menu)
{
    return NULL;
}
