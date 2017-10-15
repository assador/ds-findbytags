/**
 * ds-findbytags, v2.0.1, CLI / GTK+ 2
 * Copyright © 2016-2017 Dmitry Sokolov
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
 * C / GTK+ 2 program, that searches in the specified directories the images
 * with keywords (tags) specified in XMP or IPTC metadata and satisfy
 * the specified AND, OR, NOT conditions. Optionally the found images will open
 * in the specified program. Along the way, you can massively add, remove
 * or replace tags in the found images and to save symlinks to these images
 * in the specified directory with original or random names.
 * Read more: http://sokolov.website/programs/ds-utils/ds-findbytags
 * 
 * Depends: exiv2, libxml2-dev
 * Recommends: gtk2
 * 
 * ds-findbytags [-g] [-k] [-t path_to_tags_file] [-e encoding]
 *               [-a "tag, tag, ..."] [-o "tag, tag, ..."] [-n "tag, tag, ..."]
 *               [-i "tag, tag, ..."] [-d "tag, tag, ..."]
 *               [-c "tag, tag, [tag, tag], ..."] [-l viewer]
 *               [-s path_the_symlinks_are_saved] path(s)_where_to_search
 * 
 * -h  — (help)      — Show help and synopsis information and exit the program.
 * -g  — (GUI)       — If the key is present, the program runs
 *                     in the GTK interface instead of the CLI interface.
 * -k  — (keep)      — If the key is present, the symlinks are named by
 *                     the names of the files found. In the absence of the key,
 *                     the symlinks are named by a random character set.
 * -t  — (tags)      — Path to the XML file with the tags tree in the Geeqie
 *                     format (by default — $HOME/.config/geeqie/geeqierc.xml).
 *                     Everything in this file, that is not belong to tags,
 *                     is ignored. If the path is incorrect, the tags tree is
 *                     not displayed. Tags from the tree are added by draggging
 *                     to the required field, or — to the field “AND” —
 *                     by double-click of the right mouse button.
 * -e  — (encoding)  — Not implemented yet. Encoding of the Geeqie-formatted XML
 *                     file with tags (by default — * — autodetect with enca).
 * -a  — (and)       — File must contain all the tags specified in this key.
 * -o  — (or)        — File must contain at least one of the specified tags.
 * -n  — (not)       — File must not contain any of the specified tags.
 * -i  — (insert)    — Add the tags specified in this key to the found files.
 * -d  — (delete)    — Delete the specified tags from the found files.
 * -c  — (change)    — Replace the specified tags in the found files, in pairs.
 * -l  — (look)      — The program in which the directory with the found files
 *                     is opened; the default is Geeqie. If you do not need
 *                     to open it at all, the key value must be “no”.
 * -s  — (save)      — The directory (absolute path), in which the symlinks to
 *                     the found files are saved after the end of the program.
 *                     If the key is not specified, a temporary directory
 *                     is created at the location of the program start,
 *                     which after the end of the program is deleted.
 * 
 * Example: ds-findbytags -g -k -t "~/.config/geeqie/geeqierc.xml" \
 *                        -a "sea, gloomy clouds, stones" -o "birds, dolphins" \
 *                        -n "sharks, bees" -i "wow" -d "sea" -c "birds, cats" \
 *                        -l "no" -s "~/chosen" ~/photos1 ~/photos2
 * 
 * Dmitry Sokolov <dmitry@sokolov.website>
 **/

#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include "main.h"
#include "common.h"
#include "structopts.h"
#include "regexpmatch.h"
#include "gui.h"
#include "help.h"

#define EXTS "gif|jpe?g|pdf|png|tiff?"
#ifdef _WIN32
	#define TMP "C:\\WINDOWS\\Temp"
	#define FILE_SEPARATOR "\\"
#else
	#define TMP "/tmp"
	#define FILE_SEPARATOR "/"
#endif
Opts *opts, *opts_v;
Tags *tags;

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("ds-findbytags", "/usr/share/locale/");
	textdomain("ds-findbytags");
	srand(time(NULL));
	opts = structopts(argc, argv);
	if(argc == 1) {
		printf(
			_("\nAt least one path or -g key must be specified (see below).\n")
		);
		help();
		exit(EXIT_SUCCESS);
	}
	opts_v = (Opts*) malloc(sizeof(Opts));
	if(!opts_v) {
		fprintf(stderr, _("main(): malloc() failed: insufficient memory.\n"));
		exit(EXIT_FAILURE);
	}
	memcpy(opts_v, opts, sizeof(Opts));
	for(int i = 0; i < opts_v->args_count; i++) {
		opts_v->args[i] = realpath(opts->args[i], NULL);
	}
	tags = (Tags*) malloc(sizeof(Tags));
	if(!tags) {
		fprintf(stderr, _("main(): malloc() failed: insufficient memory.\n"));
		exit(EXIT_FAILURE);
	}
	if(opts_v->h) {
		help();
		exit(EXIT_SUCCESS);
	}
	if(opts_v->g) {
		gui();
	} else {
		if(!opts_v->args_count) {
			fprintf(stderr, _("main(): No paths to search in.\n"));
			exit(EXIT_SUCCESS);
		}
		begin();
	}
	exit(EXIT_SUCCESS);
}
int begin() {
/* Converting strings of comma separated tags to arrays */
	int count;
	if(opts_v->a) {
		tags->a = strsplit(opts_v->a, "\\s*,\\s*", 0, 0, 8, &count);
		tags->a_count = count;
	}
	if(opts_v->o) {
		tags->o = strsplit(opts_v->o, "\\s*,\\s*", 0, 0, 8, &count);
		tags->o_count = count;
	}
	if(opts_v->n) {
		tags->n = strsplit(opts_v->n, "\\s*,\\s*", 0, 0, 8, &count);
		tags->n_count = count;
	}
	if(opts_v->i) {
		tags->i = strsplit(opts_v->i, "\\s*,\\s*", 0, 0, 8, &count);
		tags->i_count = count;
	}
	if(opts_v->d) {
		tags->d = strsplit(opts_v->d, "\\s*,\\s*", 0, 0, 8, &count);
		tags->d_count = count;
	}
	if(opts_v->c) {
		char **tags_c_tmp = (char**) malloc(sizeof(char*));
		if(!tags_c_tmp) {
			fprintf(
				stderr,
				_("begin(): malloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		tags_c_tmp = strsplit(opts_v->c, "\\s*,\\s*", 0, 0, 8, &count);
		if(count % 2) {
			if(opts_v->g) {
				show_message(
					GTK_MESSAGE_ERROR, _("Odd number of tags to replace.")
				);
				return 0;
			} else {
				fprintf(
					stderr,
					_(
						"begin(): Odd number of elements "
						"in the value of key -c.\n"
					)
				);
				exit(EXIT_FAILURE);
			}
		}
		tags->c_count = count / 2;
		tags->c = (char***) malloc(sizeof(char**));
		if(!tags->c) {
			fprintf(
				stderr,
				_("begin(): malloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		for(int i = 0; i < tags->c_count; i += 2) {
			tags->c[i / 2] = (char**) malloc(sizeof(char*));
			if(!tags->c[i / 2]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			tags->c[i / 2][0] = (char*) malloc(strlen(tags_c_tmp[i]) + 1);
			if(!tags->c[i / 2][0]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			tags->c[i / 2][1] = (char*) malloc(strlen(tags_c_tmp[i + 1]) + 1);
			if(!tags->c[i / 2][1]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			*tags->c[i / 2][0] = '\0';
			*tags->c[i / 2][1] = '\0';
			strncat(
				tags->c[i / 2][0],
				tags_c_tmp[i],
				strlen(tags_c_tmp[i])
			);
			strncat(
				tags->c[i / 2][1],
				tags_c_tmp[i + 1],
				strlen(tags_c_tmp[i + 1])
			);
		}
		for(int i = 0; i < count; i++) free(tags_c_tmp[i]);
		free(tags_c_tmp);
	}
/* Generating a random name of directory where symlinks are saved */
	int tosave = 1;
	if(!opts_v->s) {
		tosave = 0;
		char *tmp = random_name(16);
		opts_v->s = strconcat((const char*[]) {TMP, FILE_SEPARATOR, tmp}, 3);
		free(tmp);
	}
/* Creating a list of paths to all the files to be analized */
	size_t args_len = 0, args_offset = 0;
	size_t args_lens[opts_v->args_count];
	for(int i = 0; i < opts_v->args_count; i++) {
		args_len += args_lens[i] = strlen(opts_v->args[i]);
	}
	char *argslist = malloc(args_len + 1);
	if(!argslist) {
		fprintf(
			stderr,
			_("begin(): malloc() failed: insufficient memory.\n")
		);
		return 0;
	}
	for(int i = 0; i < opts_v->args_count; i++) {
		memcpy(
			argslist + args_offset,
			opts_v->args[i],
			args_lens[i] + (i == opts_v->args_count - 1 ? 1 : 0)
		);
		args_offset += args_lens[i];
	}
	char *command = strconcat((const char*[]) {
		"find ",
		argslist,
		" -type f -regextype posix-extended -iregex '.*\\.(",
		EXTS,
		")$'"
	}, 5);
/* Analysis of each file */
	unsigned int files_count = 0, reoffset = 0;
	File **files = (File**) malloc(sizeof(File*));
	if(!files) {
		fprintf(stderr, _("begin(): malloc() failed: insufficient memory.\n"));
		return 0;
	}
	char **filetags;
	char *exiv_com, *exiv_out, *filename_qtd;
	pcre *re_s = regexpcompile("^subject\\s+(?=\\S)(.+)$", PCRE_MULTILINE);
	pcre *re_k = regexpcompile("^Keywords\\s+(?=\\S)(.+)$", PCRE_MULTILINE);
	Reresult *reresult;
	size_t filename_size_step = 256, filename_size = filename_size_step;
	char *filename = (char*) malloc(filename_size);
	if(!filename) {
		fprintf(stderr, _("begin(): malloc() failed: insufficient memory.\n"));
		return 0;
	}
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, _("begin(): Cannot into popen.\n"));
		return 0;
	}
	/* For each file to be analized */
	while(getline(&filename, &filename_size, found) != EOF) {
		if(strlen(filename) > filename_size - 1) {
			filename =
				(char*) realloc(filename, filename_size += filename_size_step);
			if(!filename) {
				fprintf(
					stderr,
					_("begin(): realloc() failed: insufficient memory.\n")
				);
				return 0;
			}
		}
		filename[strlen(filename) - 1] = '\0';
		filename_qtd = strconcat((const char*[]) {"'", filename, "'"}, 3);
		/* Collecting tags in XMP metadata */
		exiv_com = strconcat((const char*[]) {"exiv2 -PXnt ", filename_qtd}, 2);
		exiv_out = command_output(exiv_com);
		if(exiv_out) {
			reresult = regexpmatch_compiled(
				exiv_out, re_s, 0, sizeof(reresult->indexes)
			);
		}
		if(exiv_out && reresult->count > 1) {
			char tmp[reresult->indexes[3] - reresult->indexes[2]];
			tmp[0] = '\0';
			strncat(
				tmp,
				exiv_out + reresult->indexes[2],
				reresult->indexes[3] - reresult->indexes[2]
			);
			filetags = strsplit(tmp, "\\s*,\\s*", 0, 0, 8, &count);
		} else {
			/* Collecting tags in IPTC metadata if there are no tags in XMP */
			exiv_com = strconcat(
				(const char*[]) {"exiv2 -PInt ", filename_qtd}, 2
			);
			exiv_out = command_output(exiv_com);
			if(exiv_out) {
				reoffset = 0;
				count = 0;
				for(int i = 0; ; i++) {
					reresult = regexpmatch_compiled(
						exiv_out + reoffset, re_k, 0, sizeof(reresult->indexes)
					);
					if(reresult->count < 2) break;
					filetags =
						(char**) realloc(filetags, sizeof(char*) * (i + 1));
					if(!filetags) {
						fprintf(
							stderr,
							_("begin(): realloc() failed: "
							"insufficient memory.\n")
						);
						return 0;
					}
					filetags[i] = (char*) malloc(
						reresult->indexes[3] - reresult->indexes[2] + 1
					);
					if(!filetags[i]) {
						fprintf(
							stderr,
							_("begin(): malloc() failed: "
							"insufficient memory.\n")
						);
						return 0;
					}
					filetags[i][0] = '\0';
					strncat(
						filetags[i],
						exiv_out + reoffset + reresult->indexes[2],
						reresult->indexes[3] - reresult->indexes[2]
					);
					reoffset += reresult->indexes[1];
					count++;
				}
			}
		}
		if(exiv_out) {
			free(reresult->indexes);
			free(reresult);
			free(exiv_out);
			/* Checking the conformity of file tags to search tags */
			if(suitable(
				filetags, count, tags->a, tags->a_count,
				tags->o, tags->o_count, tags->n, tags->n_count
			)) {
				/* Actions on file tags */
				if(actions(
					&filetags, &count, tags->i, tags->i_count,
					tags->d, tags->d_count, tags->c, tags->c_count
				) > 0) {
					char *tmp = strconcat((const char*[]) {
						"exiv2 -M\"add Xmp.dc.subject\" "
							"-M\"del Iptc.Application2.Keywords\" ",
						filename_qtd
					}, 2);
					system(tmp);
					free(tmp);
					for(int i = 0; i < count; i++) {
						tmp = strconcat((const char*[]) {
							"exiv2 -M\"set Xmp.dc.subject XmpBag ",
							filetags[i],
							"\" -M\"add Iptc.Application2.Keywords String ",
							filetags[i],
							"\" ",
							filename_qtd
						}, 6);
						system(tmp);
						free(tmp);
					}
				}
				/* Creating an array of selected files */
				files_count++;
				files = (File**) realloc(files, sizeof(File*) * files_count);
				if(!files) {
					fprintf(
						stderr,
						_("begin(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				files[files_count - 1] = (File*) malloc(sizeof(File));
				if(!files[files_count - 1]) {
					fprintf(
						stderr,
						_("begin(): malloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				files[files_count - 1]->fullpath = strdup(filename);
				files[files_count - 1]->fullname = strdup(filename);
				files[files_count - 1]->path =
					dirname(files[files_count - 1]->fullpath);
				files[files_count - 1]->name =
					basename(files[files_count - 1]->fullname);
			}
			for(int i = 0; i < count; i++) free(filetags[i]);
		}
		free(filetags);
		free(filename); filename = NULL;
		free(filename_qtd); filename_qtd = NULL;
		free(exiv_com);
	}
	if(pclose(found) != 0) {
		fprintf(stderr, _("begin(): Cannot into pclose.\n"));
		return 0;
	}
	pcre_free((void*) re_s);
	pcre_free((void*) re_k);
	free(command);
	/* Creating a directory, if not exist, with the specified or random name */
	struct stat stat_buffer;
	stat(opts_v->s, &stat_buffer);
	if(!S_ISDIR(stat_buffer.st_mode) && mkdir(opts_v->s, 0755) == -1) {
		fprintf(stderr, _("begin(): Cannot create %s.\n"), opts_v->s);
	}
	/* Creating symlinks of selected files */
	for(int i = 0; i < files_count; i++) {
		if(opts_v->k) {
			filename = strdup(files[i]->name);
		} else {
			char *filename_base = random_name(16);
			Reresult *reresult = regexpmatch(
				files[i]->name, "\\..*?$", 0, 0, sizeof(reresult->indexes)
			);
			if(reresult->count != -1) {
				filename = (char*) malloc(
					17 + reresult->indexes[1] - reresult->indexes[0]
				);
				if(!filename) {
					fprintf(
						stderr,
						_("begin(): malloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				filename[0] = '\0';
				strncat(filename, filename_base, 16);
				strncat(
					filename,
					files[i]->name + reresult->indexes[0],
					reresult->indexes[1] - reresult->indexes[0]
				);
				free(reresult->indexes);
			} else {
				filename = strdup(filename_base);
			}
			free(reresult);
			free(filename_base);
		}
		char *fullname = strdup(opts_v->s);
		fullname = (char*) realloc(
			fullname,
			strlen(fullname) + strlen(FILE_SEPARATOR) + strlen(filename) + 1
		);
		if(!fullname) {
			fprintf(
				stderr,
				_("begin(): realloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		strncat(fullname, FILE_SEPARATOR, strlen(FILE_SEPARATOR));
		strncat(fullname, filename, strlen(filename));
		symlink(files[i]->fullname, fullname);
		free(fullname);
	}
	free(filename); filename = NULL;
	/* Running the specified program in the directory with symlinks */
	if(opts_v->l && strcmp(opts_v->l, "no") != 0) {
		char *tmp = strconcat((const char*[]) {opts_v->l, " ", opts_v->s}, 3);
		system(tmp);
		free(tmp);
	}
	/* Deleting the temporary directory with symlinks */
	if(!tosave) {
		char *tmp = strconcat((const char*[]) {"rm -rf ", opts_v->s}, 2);
		system(tmp);
		free(tmp);
	}
	/* Cleaning */
	for(int i = 0; i < files_count; i++) {
		free(files[i]->fullpath);
		free(files[i]->fullname);
		free(files[i]);
	}
	free(files);
	for(int i = 0; i < tags->a_count; i++) free(tags->a[i]);
	for(int i = 0; i < tags->o_count; i++) free(tags->o[i]);
	for(int i = 0; i < tags->n_count; i++) free(tags->n[i]);
	for(int i = 0; i < tags->i_count; i++) free(tags->i[i]);
	for(int i = 0; i < tags->d_count; i++) free(tags->d[i]);
	for(int i = 0; i < tags->c_count; i++) {
		for(int y = 0; y < 2; y++) free(tags->c[i][y]);
		free(tags->c[i]);
	}
	free(tags->a); free(tags->o); free(tags->n);
	free(tags->i); free(tags->d); free(tags->c);
	tags->a = tags->o = tags->n = tags->i = tags->d = NULL; tags->c = NULL;
	tags->a_count = tags->o_count = tags->n_count =
	tags->i_count = tags->d_count = tags->c_count = 0;
	return 1;
}
/* Checking the conformity of file tags to search tags */
int suitable(
	char **t, int tc, char **a, int ac, char **o, int oc, char **n, int nc
) {
	int s = 1;
	/* Checking the conformity to AND tags */
	for(int i = 0; i < ac; i++) {
		s = 0;
		for(int y = 0; y < tc; y++) {
			if(strcmp(a[i], t[y]) == 0) {s = 1; break;}
		}
		if(!s) return(0);
	}
	/* Checking the conformity to OR tags */
	for(int i = 0; i < oc; i++) {
		s = 0;
		for(int y = 0; y < tc; y++) {
			if(strcmp(o[i], t[y]) == 0) {s = 1; break;}
		}
		if(s) break;
	}
	if(!s) return(0);
	/* Checking the conformity to NOT tags */
	for(int i = 0; i < nc; i++) {
		for(int y = 0; y < tc; y++) {
			if(strcmp(n[i], t[y]) == 0) return(0);
		}
	}
	return s;
}
/* Actions on file tags */
int actions(
	char ***t, int *tc,
	char **in, int inc, char **de, int dec, char ***ch, int chc
) {
	int changed = 0, exists = 0;
	/* Adding new tags */
	for(int i = 0; i < inc; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], in[i]) == 0) {exists = 1; break;}
		}
		if(!exists) {
			*t = (char**) realloc(*t, (++*tc) * sizeof(char*));
			if(!t) {
				fprintf(
					stderr,
					_("actions(): realloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			(*t)[*tc - 1] = (char*) malloc(strlen(in[i]) + 1);
			if(!(*t)[*tc - 1]) {
				fprintf(
					stderr,
					_("actions(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			(*t)[*tc - 1][strlen(in[i])] = '\0';
			strncpy((*t)[*tc - 1], in[i], strlen(in[i]));
			changed++;
		}
		exists = 0;
	}
	/* Deleting existing tags */
	for(int i = 0; i < dec; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], de[i]) == 0) {
				free((*t)[y]);
				for(int z = y; z < *tc - 1; z++) (*t)[z] = (*t)[z + 1];
				*t = (char**) realloc(*t, (--*tc) * sizeof(char*));
				if(!t) {
					fprintf(
						stderr,
						_("actions(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				changed++;
				y--;
			}
		}
	}
	/* Replacing existing tags */
	for(int i = 0; i < chc; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], ch[i][0]) == 0) {
				(*t)[y] = (char*) realloc((*t)[y], strlen(ch[i][1]) + 1);
				if(!(*t)[y]) {
					fprintf(
						stderr,
						_("actions(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				(*t)[y][strlen(ch[i][1])] = '\0';
				strncpy((*t)[y], ch[i][1], strlen(ch[i][1]));
				changed++;
			}
		}
	}
	return changed;
}
