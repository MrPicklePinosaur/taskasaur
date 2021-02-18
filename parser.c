
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


/* reading */
Board*
begin_parse(char* board_path)
{
    const char* input_buffer;
    long input_size;
    State state;
    Board* new_board;

    /* read entire file */
    input_buffer = read_file(board_path, &input_size);

    /* setup state */
    state.cur_todolist = NULL;
    state.cur_todoitem = NULL;

    new_board = malloc(sizeof(Board));
    new_board->todolist_list = malloc(0);
    new_board->todolist_count = 0;
    state.board = new_board;

    md_parse(input_buffer, input_size, &parser, &state);

    /* finish calls */
    exit_todolist(&state);

    free((char*)input_buffer);

    return state.board;
}

char*
read_file(char* file_name, long* size) 
{ // apparently using seek isnt the greatest, may change to chunk reading later
    FILE* file;
    char* output;
    long fsize;

    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("read_file > fopen");
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
    #define sb state->board

    if (state->cur_todolist == NULL) return; 

    exit_todoitem(state);

    sb->todolist_count += 1;
    sb->todolist_list = realloc(sb->todolist_list, sb->todolist_count*sizeof(TodoList*));
    sb->todolist_list[sb->todolist_count-1] = state->cur_todolist;
    state->cur_todolist = NULL;

    #undef sb
}

void
enter_todoitem(State* state, char* item_name)
{
    TodoItem* new_todoitem;

    new_todoitem = malloc(sizeof(TodoItem));
    new_todoitem->item_name = item_name;
    new_todoitem->description = strdup("");
    new_todoitem->due = strdup("");
    new_todoitem->subtask_list = malloc(0);
    new_todoitem->subtask_count = 0;

    state->cur_todoitem = new_todoitem;
    
}

void
exit_todoitem(State* state)
{
    #define st state->cur_todolist

    if (state->cur_todoitem == NULL) return; 
    
    /* append current item to todo list */
    st->item_count += 1;
    st->item_list = realloc(st->item_list, st->item_count*sizeof(TodoItem*));
    st->item_list[st->item_count-1] = state->cur_todoitem;
    state->cur_todoitem = NULL;

    #undef st
}

void
set_description(State* state, char* description)
{
    state->cur_todoitem->description = description;
}

void
set_due(State* state, char* due)
{
    state->cur_todoitem->due = due;
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
            set_due(state, state->last_block_text);
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

/* writing */
int
begin_write(char* board_path, Board* board)
{ // TODO, make a backup file of board before write in case it crashes

    FILE* file;

    file = fopen(board_path, "w+");
    if (file == NULL) {
        perror("begin_write > fopen");
        return -1;
    }

    for (int i = 0; i < board->todolist_count; i++) {
        TodoList* cur_todolist = board->todolist_list[i];

        fprintf(file, "## %s\n", cur_todolist->list_name);

        for (int j = 0; j < cur_todolist->item_count; j++) {
            TodoItem* cur_todoitem = cur_todolist->item_list[j];

            fprintf(file, "### %s\n", cur_todoitem->item_name);
            
            // write the other fields later!!!

        }

    }

    fclose(file);

    return 0;
}

/* others */
void
log_todo(Board* board)
{
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
}

int
free_board(Board* board)
{ // all the fields are being leaked rn

    for (int i = 0; i < board->todolist_count; i++) {
        TodoList* cur_todolist = board->todolist_list[i];

        /* free(cur_todolist->list_name); */

        for (int j = 0; j < cur_todolist->item_count; j++) {
            TodoItem* cur_todoitem = cur_todolist->item_list[j];

            /* free(cur_todoitem->item_name); */
            /* free(cur_todoitem->description); */
            /* free(cur_todoitem->due); */

            /* free subtask later too */

            free(cur_todoitem);
        }

        free(cur_todolist);
    }

    /* free(board->board_name); */
    free(board);

    return 0;
}
