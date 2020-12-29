
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <md4c.h>

typedef struct SubTask {
    char* subtask_name;
    int done;
} SubTask;

typedef struct TodoItem {
    char* item_name;
    char* description;
    char* due;
    SubTask* subtask_list;
    int subtask_count;
} TodoItem;

typedef struct TodoList {
    char* list_name;
    TodoItem* item_list;
    int item_count;
} TodoList;

typedef struct Board {
    char* board_name;
    TodoList* todolist_list;
    int todolist_count;
} Board;


extern Board* begin_parse(char* board_path);

