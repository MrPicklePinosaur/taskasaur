
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
void enter_todolist();
void exit_todolist();

void enter_todoitem();
void exit_todoitem();

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
    MD_DIALECT_COMMONMARK,
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

    /* read entire file */
    input_buffer = read_file(board_path, &input_size);

    /* setup state */
    state = malloc(sizeof(State));
    state->cur_todolist = NULL;
    state->cur_todoitem = NULL;

    state->board = malloc(sizeof(Board));
    state->board->todolist_list = malloc(0);
    state->board->todolist_count = 0;

    md_parse(input_buffer, input_size, &parser, state);

    free((char*)input_buffer);

    return state->board;
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

        case MD_BLOCK_H:

            switch(((MD_BLOCK_H_DETAIL*)detail)->level) {

                case 1:
                    printf("leave h1, %s\n", state->last_block_text);
                
                case 2:
                    printf("leave h2, %s\n", state->last_block_text);
                    break;
                case 3:
                    printf("leave h3, %s\n", state->last_block_text);
                    break;

            }

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
