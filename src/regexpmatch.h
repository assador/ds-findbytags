#include <pcre.h>

typedef struct Reresult {
	int* indexes;
	int count;
} Reresult;
Reresult* regexpmatch(
	const char *string, const char *pattern,
	const int options, const int reoptions, const int indexeslength
);
Reresult* regexpmatch_compiled(
	const char *string, pcre *re,
	const int options, const int indexeslength
);
char** strsplit(
	const char *string, const char *pattern,
	const int options, const int reoptions, const int indexeslength, int *count
);
pcre* regexpcompile(const char *pattern, const int reoptions);
