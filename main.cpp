#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <ncurses.h>
using namespace std;

bool file_exists(const char* file_name) {
    ifstream test_file(file_name);
    return (bool)test_file;
}

int main(int argc, char** argv) {
    
    // read command line args
    if (argc < 2) {
        cout << "Taskasaur options\n-o [board_name]\n-n [new_board+name]";
        return 1;
    }
   
    int flag; 
    while ((flag = getopt(argc, argv, "o:n:")) != -1) {
        if (flag == 'o') {

            char* file_name = optarg;
            printf("Opening %s\n", file_name);

            // check if file exists
            if (!file_exists(file_name)) {
                printf("%s does not exist.\n", file_name);
                return 1;
            }

        } else if (flag == 'n') {

            char* new_file_name = optarg;
            printf("Creating %s\n", new_file_name);

            if (file_exists(new_file_name)) {
                printf("The board %s already exist.\n", new_file_name);
                return 1;
            }

            ofstream new_file;
            new_file.open(new_file_name);
            new_file << "# Taskasaur\n";
            new_file.close();

        }
    }   
    return 0;

   
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

    string todo_list[3] = {"Contemplate life", "Question life", "Ponder about life"};

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

        for (int i = 0; i < sizeof(todo_list)/sizeof(string); i++) {
            mvwprintw(todo_win, 2*i, 0, todo_list[i].c_str());
        }
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


