#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 32
#define MAX_NAME 128

int is_only_space(char *line);

/* Create an array of arguments suitable for passing into execvp 
   from "line". line is expected to start with a tab and contain a
   action from a Makefile. Remember to ensure that the last element
   of the array is NULL.

   It is fine to use MAX_ARGS to allocate enough space for the arguments
   rather than iterating through line twice. You may want to use strtok to
   split the line into separate tokens.

   Return NULL if there are only spaces and/or tabs in the line. No memory
   should be allocated and the return value will be NULL.
 */

char **build_args(char *line)
{
    if (is_only_space(line))
        return NULL;
    char **args = malloc(sizeof(char *) * MAX_ARGS);
    char *token;
    token = strtok(line, "\t \n");
    int i = 0;
    while (token != NULL)
    {
        args[i] = malloc(strlen(token) + 1);
        strncpy(args[i], token, strlen(token) + 1);
        i++;
        token = strtok(NULL, "\t \n");
    }
    args[i] = NULL;
    return args;
}

/* Return 1 if the line contains only spaces or a comment (#)
   Return 0 if the line contains any other character before a #

   We want to ignore empty lines or lines that contain only a comment.  This
   helper function identifies such lines.
 */
int is_comment_or_empty(char *line)
{

    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == '#')
        {
            return 1;
        }
        if (line[i] != '\t' && line[i] != ' ')
        {
            return 0;
        }
    }
    return 1;
}

/* Return 1 if the line contains only spaces and '' and tabs
   Return 0 if the line contains any other characters

   We want to ignore lines with only spaces.  This
   helper function identifies such lines.
*/
int is_only_space(char *line)
{
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ' && line[i] != '\n' && line[i] != '\t')
        {
            return 0;
        }
    }
    return 1;
}

/* Return 1 if the line contains an action
   Return 0 otherwise
*/
int is_action(char *line)
{
    if (line[0] == '\t' && line[1] != '\t' && line[1] != '\n' && line[1] != ' ')
    {
        return 1;
    }
    return 0;
}

/* Convert an array of args to a single space-separated string in buffer.
   Returns buffer.  Note that memory for args and buffer should be allocted
   by the caller.
 */
char *args_to_string(char **args, char *buffer, int size)
{
    buffer[0] = '\0';
    int i = 0;
    while (args[i] != NULL)
    {
        strncat(buffer, args[i], size - strlen(buffer));
        strncat(buffer, " ", size - strlen(buffer));
        i++;
    }
    return buffer;
}
