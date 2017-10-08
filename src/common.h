#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* strconcat(const char *s[], const int count);
char* random_name(unsigned int len);
char* command_output(const char *command);
char** command_output_lines(const char *command, int *lineslength);
