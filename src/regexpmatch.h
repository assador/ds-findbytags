#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <stdio.h>
#include <string.h>
#include <pcre.h>

typedef struct Reresult {
	int* indexes;
	int count;
} Reresult;
Reresult* regexpmatch(
	const char *string, const char *pattern,
	const int options, const int indexeslength
);
Reresult* regexpmatch_compiled(
	const char *string, pcre *re,
	const int options, const int indexeslength
);
char** strsplit(
	const char *string, const char *pattern,
	const int options, const int indexeslength, int *count
);
pcre* regexpcompile(const char *pattern, const int options);
