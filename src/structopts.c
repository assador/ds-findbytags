/**
 * ds-findbytags, v2.0.1, CLI / GTK+ 2
 * Copyright © 2017 Dmitry Sokolov
 * 
 * This file is part of ds-findbytags.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Dmitry Sokolov <dmitry@sokolov.website>
 **/

#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "structopts.h"
#include "common.h"


Opts* structopts(int argc, char **argv) {
	Opts *opts = (Opts*) malloc(sizeof(Opts));
	if(!opts) {
		fprintf(
			stderr,
			_("structopts(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	opts->e = "*";
	opts->l = "geeqie";
	opts->t = strconcat((const char*[]) {
		getenv("HOME"),
		"/.config/geeqie/geeqierc.xml"
	}, 2);
	char *unistdopts = "hgka:o:n:i:d:c:e:l:s:t:";
	int opt;
	while((opt = getopt(argc, argv, unistdopts)) != -1) {
		switch(opt) {
			case 'h' : opts->h = 1; break;
			case 'g' : opts->g = 1; break;
			case 'k' : opts->k = 1; break;
			case 'a' : opts->a = optarg; break;
			case 'o' : opts->o = optarg; break;
			case 'n' : opts->n = optarg; break;
			case 'i' : opts->i = optarg; break;
			case 'd' : opts->d = optarg; break;
			case 'c' : opts->c = optarg; break;
			case 'e' : opts->e = optarg; break;
			case 'l' : opts->l = optarg; break;
			case 's' : opts->s = optarg; break;
			case 't' :
				opts->t = optarg;
				int i = 0;
				while(opts->t[i] == ' ') i++;
				if(opts->t[i] == '~') {
					opts->t = strconcat((const char*[]) {
						getenv("HOME"),
						opts->t + 1
					}, 2);
				}
				break;
			case '?' :
				if(isprint(optopt)) {
					fprintf(
						stderr,
						_("structopts(): Unknown option `-%c'.\n"),
						optopt
					);
				} else {
					fprintf(
						stderr,
						_("structopts(): Unknown option character `\\x%x'.\n"),
						optopt
					);
				}
		}
	}
	opts->args_count = 0;
	opts->args = (char**) malloc(sizeof(char*));
	if(!opts->args) {
		fprintf(
			stderr,
			_("structopts(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	struct stat stat_buffer;
	for(int i = optind, y = 0; i < argc; i++, y++) {
		stat(argv[i], &stat_buffer);
		if(!S_ISDIR(stat_buffer.st_mode)) {
			fprintf(
				stderr,
				_("structopts(): %s does not exist or is not a directory.\n"),
				argv[i]
			);
			continue;
		}
		opts->args[y] = (char*) malloc(strlen(argv[i]) + 1);
		if(!opts->args[y]) {
			fprintf(
				stderr,
				_("structopts(): malloc() failed: insufficient memory.\n")
			);
			exit(EXIT_FAILURE);
		}
		strcpy(opts->args[y], argv[i]);
		opts->args_count++;
	}
	return opts;
}
