#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <md4c.h>

typedef struct TodoItem {
    char* name;
    char* description;
    char* due;
    char** items;
} TodoItem;

/* callbacks to parser */
int enter_block(MD_BLOCKTYPE type, void* detail, void* userdata);
int leave_block(MD_BLOCKTYPE type, void* detail, void* userdata);
int enter_span(MD_SPANTYPE type, void* detail, void* userdata);
int leave_span(MD_SPANTYPE type, void* detail, void* userdata);
int text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);
void debug_log(const char* msg, void* userdata);
void syntax(void);

static MD_PARSER parser = {
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

static char task_md[] = "###";
int
main(int argc, char** argv)
{
    FILE* input_file;
    int todo_length;

    input_file = fopen("test_board.md", "r");     
    if (!input_file) {
        printf("Something went wrong opening file");
        return 1;
    }

    return 0;    
}

int
enter_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    return 0;
}

int
leave_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
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
    return 0;
}

void
debug_log(const char* msg, void* userdata)
{
    return;
}

void 
syntax(void)
{
    return;
}
