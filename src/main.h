#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <libgen.h>
#include <sys/stat.h>
#include "common.h"
#include "regexpmatch.h"
#include "help.h"
#include "gui.h"

typedef struct Tags {
	char** a; int a_count;
	char** o; int o_count;
	char** n; int n_count;
	char** i; int i_count;
	char** d; int d_count;
	char*** c; int c_count;
} Tags;
typedef struct File {
	char* fullpath;
	char* fullname;
	char* path;
	char* name;
} File;
int begin();
int suitable(
	char **t, int tc, char **a, int ac, char **o, int oc, char **n, int nc
);
int actions(
	char ***t, int *tc,
	char **in, int inc, char **de, int dec, char ***ch, int chc
);