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

#include "common.h"

char* strconcat(const char *s1, const char *s2) {
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char *sr = malloc(len1 + len2 + 1);
	if(!sr) {
		fprintf(stderr, "malloc() failed: insufficient memory.\n");
		return NULL;
	}
	memcpy(sr, s1, len1);
	memcpy(sr + len1, s2, len2 + 1);
	return sr;
}
char* command_output(const char *command) {
	int idxc = 0, bufsize_step = 256, bufsize = bufsize_step;
	char *buffer = (char*) malloc(bufsize);
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, "Cannot into popen.\n");
		return NULL;
	}
	while((buffer[idxc] = getc(found)) != EOF) {
		if(strlen(buffer) > bufsize - 8) {
			buffer = (char*) realloc(buffer, bufsize += bufsize_step);
		}
		idxc++;
	}
	if(pclose(found) != 0) {
		fprintf(stderr, "Cannot into pclose.\n");
		return NULL;
	}
	buffer[idxc] = '\0';
	return buffer;
}
char** command_output_lines(const char *command, int *lineslength) {
	char **lines = (char**) malloc(sizeof(char*));
	if(!lines) {
		fprintf(stderr, "malloc() failed: insufficient memory.\n");
		exit(EXIT_FAILURE);
	}
	int idxc = 0, idxl = 0, bufsize_step = 256, bufsize = bufsize_step;
	char *buffer = (char*) malloc(bufsize);
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, "Cannot into popen.\n");
		return NULL;
	}
	while((buffer[idxc] = getc(found)) != EOF) {
		if(strlen(buffer) > bufsize - 8) {
			buffer = (char*) realloc(buffer, bufsize += bufsize_step);
		}
		if(buffer[idxc] == '\n') {
			lines = (char**) realloc(lines, sizeof(char*) * (idxl + 1));
			if(!lines) {
				fprintf(stderr, "malloc() failed: insufficient memory.\n");
				exit(EXIT_FAILURE);
			}
			lines[idxl] = (char*) malloc(idxc + 1);
			*lines[idxl] = '\0';
			strncat(lines[idxl], buffer, idxc);
			idxc = 0; idxl++;
			*lineslength = idxl;
		} else idxc++;
	}
	if(pclose(found) != 0) {
		fprintf(stderr, "Cannot into pclose.\n");
		return NULL;
	}
	free(buffer);
	return lines;
}
