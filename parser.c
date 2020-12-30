
#include "headers/parser.h"

typedef struct State {
    Board* board;
    TodoList* cur_todolist;
    TodoItem* cur_todoitem;
    /* for use during processing */
    char* last_block_text;
} State;

char* read_file(char* file_name, long* size);

/* processing */
void enter_todolist(State* state, char* list_name);
void exit_todolist(State* state);

void enter_todoitem(State* state, char* item_name);
void exit_todoitem(State* state);

void set_description(State* state, char* description);
void set_due(State* state, char* due); // make an acc date struct later
void add_subtask(State* state, char* subtask_name, SubTaskState subtask_state);

/* callbacks to parser */
int enter_block(MD_BLOCKTYPE type, void* detail, void* userdata);
int leave_block(MD_BLOCKTYPE type, void* detail, void* userdata);
int enter_span(MD_SPANTYPE type, void* detail, void* userdata);
int leave_span(MD_SPANTYPE type, void* detail, void* userdata);
int text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);
void debug_log(const char* msg, void* userdata);
void syntax(void);

const MD_PARSER parser = {
    0,
    MD_FLAG_TASKLISTS,
    &enter_block,
    &leave_block,
    &enter_span,
    &leave_span,
    &text,
    &debug_log,
    &syntax
};

char*
read_file(char* file_name, long* size) 
{ // apparently using seek isnt the greatest, may change to chunk reading later
    FILE* file;
    char* output;
    long fsize;

    file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Something went wrong opening file\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    output = malloc(fsize+1); // maybe error check malloc
    fread(output, 1, fsize, file);
    if (ferror(file)) {
        printf("Something went wrong reading file\n");
        fclose(file);
        free(output);
        return NULL;
    }
    output[fsize] = 0; // add null terminator
    
    /* clean up */
    fclose(file);

    /* return */
    *size = fsize;
    return output;
}

Board*
begin_parse(char* board_path)
{
    const char* input_buffer;
    long input_size;
    State* state;
    Board* new_board;

    /* read entire file */
    input_buffer = read_file(board_path, &input_size);

    /* setup state */
    state = malloc(sizeof(State));
    state->cur_todolist = NULL;
    state->cur_todoitem = NULL;

    new_board = malloc(sizeof(Board));
    new_board->todolist_list = malloc(0);
    new_board->todolist_count = 0;
    state->board = new_board;

    md_parse(input_buffer, input_size, &parser, state);

    /* finish calls */
    exit_todolist(state);

    free((char*)input_buffer);

    return state->board;
}

void
enter_todolist(State* state, char* list_name)
{
    TodoList* new_todolist;
    
    new_todolist = malloc(sizeof(TodoList));
    new_todolist->list_name = list_name;
    new_todolist->item_list = malloc(0);
    new_todolist->item_count = 0;

    state->cur_todolist = new_todolist;
}

void
exit_todolist(State* state)
{
    Board* board;
    TodoList** todolist_list;

    if (state->cur_todolist == NULL) { 
        return;
    }

    exit_todoitem(state);

    /* append new todolist to board */
    board = state->board;
    todolist_list = board->todolist_list;

    board->todolist_count += 1;
    todolist_list = realloc(todolist_list, board->todolist_count*sizeof(TodoList*));
    todolist_list[board->todolist_count-1] = state->cur_todolist;
    state->cur_todolist = NULL;

    /* save */
    board->todolist_list = todolist_list;

}

void
enter_todoitem(State* state, char* item_name)
{
    TodoItem* new_todoitem;

    new_todoitem = malloc(sizeof(TodoItem));
    new_todoitem->item_name = item_name;
    new_todoitem->description = NULL;
    new_todoitem->due = NULL;
    new_todoitem->subtask_list = malloc(0);
    new_todoitem->subtask_count = 0;

    state->cur_todoitem = new_todoitem;
    
}

void
exit_todoitem(State* state)
{
    TodoList* todolist;
    TodoItem** item_list;

    if (state->cur_todoitem == NULL) {
        return;
    }
    
    /* append current item to todo list */
    todolist = state->cur_todolist;
    item_list = todolist->item_list;

    todolist->item_count += 1;
    item_list = realloc(item_list, todolist->item_count*sizeof(TodoItem*));
    item_list[todolist->item_count-1] = state->cur_todoitem;
    state->cur_todoitem = NULL;

    /* save */
    todolist->item_list = item_list;

}

void
set_description(State* state, char* description)
{
    state->cur_todoitem->description = description;
}

void
set_due(State* state, char* due)
{

}

void
add_subtask(State* state, char* subtask_name, SubTaskState subtask_state)
{
    SubTask* new_subtask;
    SubTask** subtask_list;
    
    /* create new subtask */
    new_subtask = malloc(sizeof(SubTask));
    new_subtask->subtask_name = subtask_name;
    new_subtask->done = subtask_state;

    /* add it */
    subtask_list = state->cur_todoitem->subtask_list;

    state->cur_todoitem->subtask_count += 1;
    subtask_list = realloc(subtask_list, state->cur_todoitem->subtask_count*sizeof(SubTask*));
    subtask_list[state->cur_todoitem->subtask_count-1] = new_subtask;

    state->cur_todoitem->subtask_list = subtask_list;

}

int
enter_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    return 0;
}

int
leave_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    State* state;
    state = (State*)userdata;

    switch (type) {

        case MD_BLOCK_H:;

            MD_BLOCK_H_DETAIL* h_detail;
            h_detail = ((MD_BLOCK_H_DETAIL*)detail);

            switch(h_detail->level) {

                case 1:
                    break;

                case 2:
                    exit_todolist(state);
                    enter_todolist(state, state->last_block_text);
                    break;

                case 3:
                    exit_todoitem(state);
                    enter_todoitem(state, state->last_block_text);
                    break;
            }

            break;

        case MD_BLOCK_QUOTE:
            set_description(state, state->last_block_text);
            break;

        case MD_BLOCK_LI:;
            MD_BLOCK_LI_DETAIL* li_detail;
            SubTaskState done;

            li_detail = ((MD_BLOCK_LI_DETAIL*)detail);

            assert(li_detail->is_task != 0); // all lists should be task lists

            done = li_detail->task_mark == ' ' ? SubTaskState_todo : SubTaskState_done;

            add_subtask(state, state->last_block_text, done);
            break;

        // no need for default case for now :>
    }
    return 0;
}

int
enter_span(MD_SPANTYPE type, void* detail, void* userdata)
{


    return 0;
}

int
leave_span(MD_SPANTYPE type, void* detail, void* userdata)
{
    State* state;
    state = (State*)userdata;
    
    switch (type) {
        case MD_SPAN_STRONG:
            printf("date, %s\n", state->last_block_text);
            break;
    }
    return 0;
}

int
text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata){

    State* state;
    char* content;

    state = (State*)userdata;

    content = malloc(size*sizeof(char)+1);
    memcpy(content, text, size);
    content[size] = 0;

    /* printf("%s\n", content); */
    state->last_block_text = content;
    
    return 0;
}

void
debug_log(const char* msg, void* userdata)
{
    printf(msg);
    return;
}

void 
syntax(void)
{
    return;
}
