#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TodoItem {
    char* name;
    char* description;
    char* due;
    char** items;
} TodoItem;

void parse(FILE* file, int* length);


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

    parse(input_file, &todo_length);

    return 0;    
}

/* TodoItem** */ 
void
parse(FILE* file, int* length) 
{
    TodoItem** out_arr;
    int out_len;
    char* lineptr;
    size_t len;
    ssize_t nread;

    out_arr = NULL;
    out_len = 0;
    lineptr = NULL;
    len = 0;

    while ((nread = getline(&lineptr, &len, file)) != -1) {

        lineptr[strcspn(lineptr, "\n")] = 0; // remove newline
        
        if (strcmp(lineptr, "") == 0) {
            printf("found empty line\n");
            lineptr = NULL;
            continue;
        } 

        // found a task
        if (strlen(lineptr) > 3 && strncmp(lineptr, task_md, 3) == 0) {
            printf("found_task\n");
            lineptr = NULL;
            continue;
        }


        

        /* out_arr = realloc(out_arr, (sizeof(char*))*out_len); // bad to keep resizing? */

        /* out_arr[out_len-1] = lineptr; */

        lineptr = NULL;
    }
    
    *length = out_len;
    /* return out_arr; */
}
