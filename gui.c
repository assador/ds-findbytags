/**
 * ds-findbytags, v2.0.0, CLI / GTK+ 2
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

#include <gtk/gtk.h>

void gui(int *argc, char *argv[]) {

	int *findin_b, *findin_m;
	GtkWidget
		*window, *tooltips, *hpaned, *hbox2, *vbox, *findin_frame,
		*findin_table, *findin_p, *search_frame,
		*search_table, *search_a_l, *search_a, *search_o_l, *search_o,
		*search_n_l, *search_n, *action_frame, *action_table, *action_i_l,
		*action_i, *action_d_l, *action_d, *action_c_l, *action_c,
		*begin_savecb, *begin_savefc, *begin_keepcb, *begin_button,
		*scrolledwindow;

	gtk_init(argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(
		G_OBJECT(window),
		"destroy",
		G_CALLBACK(gtk_main_quit),
		NULL
	);
	gtk_window_set_title(
		GTK_WINDOW(window),
		"FindByTags — "
		"Поиск картинок по тэгам и операции с тэгами найденных картинок"
	);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 100);

	gtk_widget_show_all(window);
	gtk_main();
}
