
#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <md4c.h>

typedef enum SubTaskState {
    SubTaskState_todo,
    SubTaskState_done
} SubTaskState;

typedef struct SubTask {
    char* subtask_name;
    SubTaskState done;
} SubTask;

typedef struct TodoItem {
    char* item_name;
    char* description;
    char* due;
    SubTask** subtask_list;
    int subtask_count;
} TodoItem;

typedef struct TodoList {
    char* list_name;
    TodoItem** item_list;
    int item_count;
} TodoList;

typedef struct Board {
    char* board_name;
    TodoList** todolist_list;
    int todolist_count;
} Board;

extern Board* begin_parse(char* board_path);

extern void log_todo(Board* board);

#endif
