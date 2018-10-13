#define NAME "ds-findbytags"
#define VERSION "v2.0.3"
#define COPYRIGHT "Copyright © 2018"
#define AUTHOR _("Dmitry Sokolov <dmitry@sokolov.website>")
#define WEBSITE "http://sokolov.website/programs/ds-utils/ds-findbytags"
#define LICENSE \
	_("This program is free software: you can redistribute it and/or modify it" \
	"\nunder the terms of the GNU General Public License as published\n" \
	"by the Free Software Foundation, either version 3 of the License,\n" \
	"or (at your option) any later version.\n\n" \
	"This program is distributed in the hope that it will be useful,\n" \
	"but WITHOUT ANY WARRANTY; without even the implied warranty\n" \
	"of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
	"See the GNU General Public License for more details.\n\n" \
	"You should have received a copy of the GNU General Public License\n" \
	"along with this program. If not, see <http://www.gnu.org/licenses/>")
#define COMMENTS \
	_("C / GTK+ 2 program, that searches in the specified directories " \
	"the images with keywords (tags) specified in XMP or IPTC metadata " \
	"and satisfy the specified AND, OR, NOT conditions. Optionally the found " \
	"images will open in the specified program. Along the way, you can " \
	"massively add, remove or replace tags in the found images and to save " \
	"symlinks to these images in the specified directory with original " \
	"or random names.\n\n" \
	"Depends: exiv2, libxml2-dev. Recommends: gtk2.\n\n" \
	"For more information run the script without any keys or arguments, " \
	"or with -h key.")
