#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* strconcat(const char *s1, const char *s2);
char* command_output(const char *command);
char** command_output_lines(const char *command, int *lineslength);
