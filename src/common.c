/**
 * common.c
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
#include <stdlib.h>
#include <string.h>
#include "common.h"

char* strconcat(const char *s[], const int count) {
	size_t len = 0, offset = 0;
	size_t lens[count];
	for(int i = 0; i < count; i++) {
		len += lens[i] = strlen(s[i]);
	}
	char *sr = malloc(len + 1);
	if(!sr) {
		fprintf(
			stderr,
			_("strconcat(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	for(int i = 0; i < count; i++) {
		memcpy(sr + offset, s[i], lens[i] + (i == count - 1 ? 1 : 0));
		offset += lens[i];
	}
	return sr;
}
/* Generating a random string with a specified number of characters */
char* random_name(unsigned int len) {
	char *str = (char*) malloc(len + 1);
	if(!str) {
		fprintf(
			stderr,
			_("random_name(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	str[len] = '\0';
	unsigned int rand_char;
	for(int i = 0; i < len; i++) {
		do {
			rand_char = (unsigned int) rand() % 123;
		} while(
			rand_char < 48
			|| (rand_char > 57 && rand_char < 65)
			|| (rand_char > 90 && rand_char < 97)
		);
		str[i] = (char) rand_char;
	}
	return(str);
}
char* command_output(const char *command) {
	int idxc = 0, bufsize_step = 4096, bufsize = bufsize_step;
	char *buffer = (char*) malloc(bufsize);
	if(!buffer) {
		fprintf(
			stderr,
			_("command_output(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, _("command_output(): Cannot into popen.\n"));
		return NULL;
	}
	while((buffer[idxc] = getc(found)) != EOF) {
		if(strlen(buffer) > bufsize - 8) {
			buffer = (char*) realloc(buffer, bufsize += bufsize_step);
			if(!buffer) {
				fprintf(
					stderr,
					_("command_output(): realloc() failed: "
					"insufficient memory.\n")
				);
				return NULL;
			}
		}
		idxc++;
	}
	if(pclose(found) != 0) {
		fprintf(stderr, _("command_output(): Cannot into pclose.\n"));
		return NULL;
	}
	buffer[idxc] = '\0';
	return buffer;
}
char** command_output_lines(const char *command, int *lineslength) {
	char **lines = (char**) malloc(sizeof(char*));
	if(!lines) {
		fprintf(
			stderr,
			_("command_output_lines(): malloc() failed: insufficient memory.\n")
		);
		exit(EXIT_FAILURE);
	}
	int idxc = 0, idxl = 0, bufsize_step = 4096, bufsize = bufsize_step;
	char *buffer = (char*) malloc(bufsize);
	if(!buffer) {
		fprintf(
			stderr,
			_("command_output_lines(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, _("command_output_lines(): Cannot into popen.\n"));
		return NULL;
	}
	while((buffer[idxc] = getc(found)) != EOF) {
		if(strlen(buffer) > bufsize - 8) {
			buffer = (char*) realloc(buffer, bufsize += bufsize_step);
			if(!buffer) {
				fprintf(
					stderr,
					_("command_output_lines(): realloc() failed: "
					"insufficient memory.\n")
				);
				return NULL;
			}
		}
		if(buffer[idxc] == '\n') {
			lines = (char**) realloc(lines, sizeof(char*) * (idxl + 1));
			if(!lines) {
				fprintf(
					stderr,
					_("command_output_lines(): realloc() failed: "
					"insufficient memory.\n")
				);
				exit(EXIT_FAILURE);
			}
			lines[idxl] = (char*) malloc(idxc + 1);
			if(!lines[idxl]) {
				fprintf(
					stderr,
					_("command_output_lines(): malloc() failed: "
					"insufficient memory.\n")
				);
				return NULL;
			}
			*lines[idxl] = '\0';
			strncat(lines[idxl], buffer, idxc);
			idxc = 0; idxl++;
			*lineslength = idxl;
		} else idxc++;
	}
	if(pclose(found) != 0) {
		fprintf(stderr, _("command_output_lines(): Cannot into pclose.\n"));
		return NULL;
	}
	free(buffer);
	return lines;
}
