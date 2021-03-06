/**
 * regexpmatch.c
 * Copyright © 2017 Dmitry Sokolov
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
#include <string.h>
#include "regexpmatch.h"

Reresult* regexpmatch(
	const char *string, const char *pattern,
	const int options, int reoptions, const int indexeslength
) {
	Reresult *reresult = (Reresult*) malloc(sizeof(Reresult));
	if(!reresult) {
		fprintf(
			stderr,
			_("regexpmatch(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	pcre *re;
	if(!(re = regexpcompile(pattern, reoptions))) return NULL;
	reresult->indexes = (int*) malloc(indexeslength);
	if(!reresult->indexes) {
		fprintf(
			stderr,
			_("regexpmatch(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	reresult->count = pcre_exec(
		re, NULL, string, strlen(string), 0, options,
		reresult->indexes, indexeslength
	);
	pcre_free((void*) re);
	return reresult;
}
Reresult* regexpmatch_compiled(
	const char *string, pcre *re,
	const int options, const int indexeslength
) {
	Reresult *reresult = (Reresult*) malloc(sizeof(Reresult));
	if(!reresult) {
		fprintf(
			stderr,
			_("regexpmatch_compiled(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	reresult->indexes = (int*) malloc(indexeslength);
	if(!reresult->indexes) {
		fprintf(
			stderr,
			_("regexpmatch_compiled(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	reresult->count = pcre_exec(
		re, NULL, string, strlen(string), 0, options,
		reresult->indexes, indexeslength
	);
	return reresult;
}
char** strsplit(
	const char *string, const char *pattern,
	const int options, const int reoptions, const int indexeslength, int *splitedcount
) {
	int count = 0;
	char** substrings = (char**) malloc(sizeof(char*));
	if(!substrings) {
		fprintf(
			stderr,
			_("strsplit(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	pcre *re;
	if(!(re = regexpcompile(pattern, reoptions))) return NULL;
	int next_dlmtr_count = 0;
	int next_dlmtr_offset = 0;
	int *next_dlmtr_indexes = (int*) malloc(indexeslength);
	if(!next_dlmtr_indexes) {
		fprintf(
			stderr,
			_("strsplit(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	do {
		next_dlmtr_count = pcre_exec(
			re, NULL, string, strlen(string), next_dlmtr_offset, options,
			next_dlmtr_indexes, indexeslength
		);
		if(next_dlmtr_count != -1) {
			substrings[count] =
				(char*) malloc(next_dlmtr_indexes[0] - next_dlmtr_offset + 1);
			if(!substrings[count]) {
				fprintf(
					stderr,
					_("strsplit(): malloc() failed: insufficient memory.\n")
				);
				exit(EXIT_FAILURE);
			}
			*substrings[count] = '\0';
			strncat(
				substrings[count],
				string + next_dlmtr_offset,
				next_dlmtr_indexes[0] - next_dlmtr_offset
			);
			next_dlmtr_offset = next_dlmtr_indexes[1];
			substrings =
				(char**) realloc(substrings, (count + 2) * sizeof(char*));
			if(!substrings) {
				fprintf(
					stderr,
					_("strsplit(): realloc() failed: insufficient memory.\n")
				);
				exit(EXIT_FAILURE);
			}
		} else {
			substrings[count] =
				(char*) malloc(strlen(string) - next_dlmtr_offset + 1);
			if(!substrings[count]) {
				fprintf(
					stderr,
					_("strsplit(): malloc() failed: insufficient memory.\n")
				);
				exit(EXIT_FAILURE);
			}
			*substrings[count] = '\0';
			strncat(
				substrings[count],
				string + next_dlmtr_offset,
				strlen(string) - next_dlmtr_offset
			);
		}
		count++;
	} while(next_dlmtr_count != -1);
	free(next_dlmtr_indexes);
	*splitedcount = count;
	pcre_free((void*) re);
	return substrings;
}
pcre* regexpcompile(const char *pattern, const int reoptions) {
	const unsigned char *tables = NULL;
	setlocale(LC_CTYPE, (const char*) "ru.");
	tables = pcre_maketables();
	pcre *re;
	const char *error;
	int erroffset;
	re = pcre_compile(pattern, reoptions, &error, &erroffset, NULL);
	if(!re) {
		fprintf(
			stderr,
			_("regexpcompile(): Regexp compile failed at offset %d: %s\n"),
			erroffset,
			error
		);
		pcre_free((void*) re);
		pcre_free((void*) tables);
		return NULL;
	}
	pcre_free((void*) tables);
	return re;
}
