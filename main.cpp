#include <ncurses.h>
using namespace std;

int main(int argc, char ** argv) {
    
    initscr();
    cbreak();
    /* raw(); */
    noecho();
    
    WINDOW * win = newwin(10,20,10,10); 
    
    refresh();
    
    int x, y;
    x = y = 0;

    while (true) {
        int ch = getch();
        
        //ofc the first thing we need is vim keys 
        switch (ch) {
            case 104: // h
                x -= 1;
                break;
            case 106: // j
                y += 1;
                break;
            case 107: // k
                y -= 1;
                break;
            case 108: // l
                x += 1;
                break;
        } 
        if (ch == 113) break; // q for quit

        move(y,x);

        box(win, 0, 0);
        mvwprintw(win, 0, 1, "lmao");
        wrefresh(win);

        refresh();
        /* clear(); */
    }

    endwin();

    return 0;
}


