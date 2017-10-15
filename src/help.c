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
 * Dmitry Sokolov <dmitry@sokolov.website>
 **/

#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <stdio.h>
#include "help.h"

void help() {
	char *help_text = _(
		"\n"
		"ds-findbytags, v2.0.1, CLI / GTK+ 2\n"
		"Copyright © 2016-2017 Dmitry Sokolov\n"
		"\n"
		"This program is free software: you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation, either version 3 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
		"\n"
		"C / GTK+ 2 program, that searches in the specified directories the images\n"
		"with keywords (tags) specified in XMP or IPTC metadata and satisfy\n"
		"the specified AND, OR, NOT conditions. Optionally the found images will open\n"
		"in the specified program. Along the way, you can massively add, remove\n"
		"or replace tags in the found images and to save symlinks to these images\n"
		"in the specified directory with original or random names.\n"
		"Read more: http://sokolov.website/programs/ds-utils/ds-findbytags\n"
		"\n"
		"Depends: exiv2, libxml2-dev\n"
		"Recommends: gtk2\n"
		"\n"
		"ds-findbytags [-h] [-g] [-k] [-t path_to_tags_file] [-e encoding]\n"
		"              [-a \"tag, tag, ...\"] [-o \"tag, tag, ...\"] [-n \"tag, tag, ...\"]\n"
		"              [-i \"tag, tag, ...\"] [-d \"tag, tag, ...\"]\n"
		"              [-c \"tag, tag, [tag, tag], ...\"] [-l viewer]\n"
		"              [-s path_the_symlinks_are_saved] path(s)_where_to_search\n"
		"\n"
		"-h  — (help)      — Show help and synopsis information and exit the program.\n"
		"-g  — (GUI)       — If the key is present, the program runs\n"
		"                    in the GTK interface instead of the CLI interface.\n"
		"-k  — (keep)      — If the key is present, the symlinks are named by\n"
		"                    the names of the files found. In the absence of the key,\n"
		"                    the symlinks are named by a random character set.\n"
		"-t  — (tags)      — Path to the XML file with the tags tree in the Geeqie\n"
		"                    format (by default — $HOME/.config/geeqie/geeqierc.xml).\n"
		"                    Everything in this file, that is not belong to tags,\n"
		"                    is ignored. If the path is incorrect, the tags tree is\n"
		"                    not displayed. Tags from the tree are added by draggging\n"
		"                    to the required field, or — to the field “AND” —\n"
		"                    by double-click of the right mouse button.\n"
		"-e  — (encoding)  — Not implemented yet. Encoding of the Geeqie-formatted XML\n"
		"                    file with tags (by default — * — autodetect with enca).\n"
		"-a  — (and)       — File must contain all the tags specified in this key.\n"
		"-o  — (or)        — File must contain at least one of the specified tags.\n"
		"-n  — (not)       — File must not contain any of the specified tags.\n"
		"-i  — (insert)    — Add the tags specified in this key to the found files.\n"
		"-d  — (delete)    — Delete the specified tags from the found files.\n"
		"-c  — (change)    — Replace the specified tags in the found files, in pairs.\n"
		"-l  — (look)      — The program in which the directory with the found files\n"
		"                    is opened; the default is Geeqie. If you do not need\n"
		"                    to open it at all, the key value must be “no”.\n"
		"-s  — (save)      — The directory (absolute path), in which the symlinks to\n"
		"                    the found files are saved after the end of the program.\n"
		"                    If the key is not specified, a temporary directory\n"
		"                    is created at the location of the program start,\n"
		"                    which after the end of the program is deleted.\n"
		"\n"
		"Example: ds-findbytags -g -k -t \"~/.config/geeqie/geeqierc.xml\" \\\n"
		"                       -a \"sea, gloomy clouds, stones\" -o \"birds, dolphins\" \\\n"
		"                       -n \"sharks, bees\" -i \"wow\" -d \"sea\" -c \"birds, cats\" \\\n"
		"                       -l \"no\" -s \"~/chosen\" ~/photos1 ~/photos2\n"
		"\n"
		"Dmitry Sokolov <dmitry@sokolov.website>\n"
	);
	printf("%s\n", help_text);
}
