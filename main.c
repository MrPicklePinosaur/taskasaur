#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char** argv) {

    // start ncurses 
    initscr();
    cbreak();
    /* raw(); */
    noecho();
    start_color();
    
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_BLACK, COLOR_CYAN); 

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW * win = newwin(10,20,5,10); 
    WINDOW * todo_win = newwin(20,20,5,35);
    WINDOW * bottombar = newwin(1,width,height-1,0); 
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

        box(win, 0, 0);
        wattron(win,COLOR_PAIR(1));
        wattron(win, A_BOLD);
        mvwprintw(win, 0, 1, "lmao");
        wattroff(win, A_BOLD);
        wattroff(win,COLOR_PAIR(1));
        mvwprintw(win, 1, 2, "poopoopeepee");
        wrefresh(win);

        wrefresh(todo_win);

        wbkgd(bottombar, COLOR_PAIR(2));        
        mvwprintw(bottombar, 0, 2, "BOTTOM TEXT");
        wrefresh(bottombar);

        move(y,x);
        refresh();
        /* clear(); */
    }

    endwin();
    return 0;
}
