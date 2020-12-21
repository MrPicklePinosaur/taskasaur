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

char* read_file(char* file_name, long* size);

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

int
main(int argc, char** argv)
{
    char* input_buffer;
    long input_size;

    input_buffer = read_file("test_board.md", &input_size);

    md_parse(input_buffer, input_size, &parser, NULL);

    return 0;    
}

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
