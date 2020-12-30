#include "headers/parser.h"

int
main(int argc, char** argv)
{
    Board* board;

    board = begin_parse("test_board.md");

    for (int i = 0; i < board->todolist_count; i++) {
        TodoList* todolist;
        printf("List =-=-=-=-=-==-=-=-=-=-=-\n");
        todolist = board->todolist_list[i];
        printf("List name: %s\n", todolist->list_name);
        printf("Num of items: %d\n", todolist->item_count);

        for (int j = 0; j < todolist->item_count; j++) {
            TodoItem* todoitem;
            printf("Item =-=-=-=-=-\n");
            todoitem = todolist->item_list[j];
            printf("Item name: %s\n", todoitem->item_name);
            printf("Description: %s\n", todoitem->description);
            printf("Num of subtasks: %d\n", todoitem->subtask_count);

            for (int k = 0; k < todoitem->subtask_count; k++) {
                SubTask* subtask;
                int done;

                subtask = todoitem->subtask_list[k];
                printf("Subtask: %s, %d\n", subtask->subtask_name, subtask->done);
            }
        }

    }
    /* printf("%d\n", board->todolist_count); */
    /* printf("%s\n", board->todolist_list[1]->list_name); */

    return 0;    
}
