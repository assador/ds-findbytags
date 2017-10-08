typedef struct Opts {
	char** args;
	int args_count;
	int g;
	int k;
	char* a;
	char* o;
	char* n;
	char* i;
	char* d;
	char* c;
	char* e;
	char* l;
	char* s;
	char* t;
} Opts;
Opts* structopts(int argc, char **argv);
