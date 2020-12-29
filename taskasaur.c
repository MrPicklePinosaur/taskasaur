#include "headers/parser.h"

int
main(int argc, char** argv)
{
    Board* board;

    board = begin_parse("test_board.md");

    printf("%d\n", board->todolist_count);

    return 0;    
}
