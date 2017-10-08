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

#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "gui.h"
#include "common.h"
#include "structopts.h"
#include "regexpmatch.h"
#include "help.h"


typedef struct Widget_aa {
	GtkWidget *widget;
	char *action;
	int append;
	GtkWidget *recipient;
} Widget_aa;

static Wdgt* wdgt_push(Wdgt **head, GtkWidget *widget, gchar* string);
static int wdgt_remove_by_widget(Wdgt **head, GtkWidget *widget);
static Wdgt* wdgt_get_by_n(Wdgt *head, int n);
static Wdgt* wdgt_get_by_widget(Wdgt *head, GtkWidget *widget);

static char* entry_text(GtkEntry *entry);
static void findin_p_clicked(void);
static void findin_b_clicked(GtkWidget *b);
static void findin_m_clicked(GtkWidget *b, GtkWidget *m);
static gchar* choose_path(gchar *initial);
static void add_path(gchar *path);
static Wdgt* was_path(Wdgt *head, gchar *path);
static xmlDoc* tree(char *path, char *root);
static GtkWidget* build_tree(xmlDoc* tree);
static int treeview_clicked(GtkWidget *widget, GdkEventButton *event);
static void append_children(
	GtkTreeModel *model,
	xmlNodePtr child,
	GtkTreeIter *parent,
	int columns_count
);
static void entry_drag_data_received(
	GtkWidget *widget,
	GdkDragContext *context,
	gint x,
	gint y,
	GtkSelectionData *data,
	guint info,
	guint time,
	gpointer user_data
);
static void entry_drag_data(
	GtkWidget *widget,
	char *action,
	int append,
	GtkWidget *recipient
);
static void treeview_foreach_selected(
	GtkTreeModel *model,
	GtkTreePath *path,
	GtkTreeIter *iter,
	gpointer data
);
static void paths_from_buttons(Opts *o);
static void begin_savecb_toggled(GtkWidget *widget);
static void gtk_begin();

extern Opts *opts_v;
extern void begin();

static Wdgt *findin_b_head = NULL, *findin_m_head = NULL;
static int findin_b_count = 0;
static gchar *last_path = ".";
static GtkWidget
	*window, *hpaned, *hbox2, *vbox, *findin_frame,
	*findin_table, *findin_p, *search_frame,
	*search_table, *search_a_l, *search_a, *search_o_l, *search_o,
	*search_n_l, *search_n, *action_frame, *action_table, *action_i_l,
	*action_i, *action_d_l, *action_d, *action_c_l, *action_c,
	*begin_savecb, *begin_savefc, *begin_keepcb, *begin_button,
	*scrolledwindow, *treeview;

void gui() {
	gtk_init(NULL, NULL);
/* Окно */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(
		G_OBJECT(window),
		"destroy",
		G_CALLBACK(gtk_main_quit),
		NULL
	);
	gtk_window_set_title(
		GTK_WINDOW(window),
		_("ds-findbytags — "
		"Search images by tags and modify tags of found images")
	);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 100);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
/* Горизонтальный контейнер */
	hpaned = gtk_hpaned_new();
	gtk_container_add(GTK_CONTAINER(window), hpaned);
/* Левый контейнер */
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_widget_set_size_request(vbox, 385, -1);
	gtk_paned_pack1(GTK_PANED(hpaned), vbox, TRUE, FALSE);
/* Правый контейнер */
	treeview = build_tree(tree(opts_v->t, "keyword_tree"));
	if(treeview) {
		static GtkTargetEntry entries[] = {{"text/plain", 0, 0}};
		gtk_tree_view_enable_model_drag_source(
			GTK_TREE_VIEW(treeview),
			GDK_BUTTON1_MASK,
			entries,
			G_N_ELEMENTS(entries),
			GDK_ACTION_COPY
		);
		g_signal_connect(
			treeview,
			"button-press-event",
			G_CALLBACK(treeview_clicked),
			NULL
		);
		scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_set_size_request(scrolledwindow, 130, -1);
		gtk_scrolled_window_set_policy(
			GTK_SCROLLED_WINDOW(scrolledwindow),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC
		);
		gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);
		gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow, TRUE, FALSE);
	}
/* Фрэйм «Искать в» */
	findin_frame = gtk_frame_new(_(" Search in "));
	gtk_box_pack_start(GTK_BOX(vbox), findin_frame, FALSE, TRUE, 0);
/* Таблица с кнопками путей для поиска */
	findin_table = gtk_table_new(1, 3, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(findin_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(findin_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(findin_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(findin_frame), findin_table);
/* Кнопка добавления пути для поиска */
	findin_p = gtk_button_new_with_label("+");
	gtk_widget_set_tooltip_text(findin_p, _("Add new path to search"));
	g_signal_connect(findin_p, "clicked", G_CALLBACK(findin_p_clicked), NULL);
	gtk_table_attach(
		GTK_TABLE(findin_table),
		findin_p,
		0, 1, 0, 1,
		GTK_SHRINK, GTK_FILL,
		0, 0
	);
/* Поиск по тэгам */
	search_frame = gtk_frame_new(_(" Search by tags "));
	gtk_box_pack_start(GTK_BOX(vbox), search_frame, FALSE, TRUE, 0);
/* Таблица с полями ввода тэгов для поиска */
	search_table = gtk_table_new(3, 2, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(search_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(search_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(search_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(search_frame), search_table);
/* Строка ввода тэгов для поиска И */
	search_a_l = gtk_label_new(_("Contain all of these tags:"));
	gtk_misc_set_alignment(GTK_MISC(search_a_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_a_l,
		0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0
	);
	search_a = gtk_entry_new();
	if(opts_v->a) gtk_entry_set_text(GTK_ENTRY(search_a), opts_v->a);
	gtk_widget_set_tooltip_text(
		search_a,
		_("The search is performed for files, which\nCONTAIN ALL\n"
		"of these tags (separated by commas)")
	);
	g_signal_connect(
		search_a, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_a, 1, 2, 0, 1);
/* Строка ввода тэгов для поиска ИЛИ */
	search_o_l = gtk_label_new(_("Contain at least one of these tags:"));
	gtk_misc_set_alignment(GTK_MISC(search_o_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_o_l,
		0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0
	);
	search_o = gtk_entry_new();
	if(opts_v->o) gtk_entry_set_text(GTK_ENTRY(search_o), opts_v->o);
	gtk_widget_set_tooltip_text(
		search_o,
		_("The search is performed for files, which\nCONTAIN AT LEAST ONE\n"
		"of these tags (separated by commas)")
	);
	g_signal_connect(
		search_o, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_o, 1, 2, 1, 2);
/* Строка ввода тэгов для поиска НЕ */
	search_n_l = gtk_label_new(_("Contain no one of these tags:"));
	gtk_misc_set_alignment(GTK_MISC(search_n_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_n_l,
		0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0
	);
	search_n = gtk_entry_new();
	if(opts_v->n) gtk_entry_set_text(GTK_ENTRY(search_n), opts_v->n);
	gtk_widget_set_tooltip_text(
		search_n,
		_("The search is performed for files, which\nCONTAIN NO ONE\n"
		"of these tags (separated by commas)")
	);
	g_signal_connect(
		search_n, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_n, 1, 2, 2, 3);
/* Действия с найденным */
	action_frame = gtk_frame_new(_(" Actions with found "));
	gtk_box_pack_start(GTK_BOX(vbox), action_frame, FALSE, TRUE, 0);
/* Таблица с полями ввода тэгов для поиска */
	action_table = gtk_table_new(3, 2, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(action_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(action_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(action_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(action_frame), action_table);
/* Строка ввода тэгов для добавления */
	action_i_l = gtk_label_new(_("Add these tags to found files:"));
	gtk_misc_set_alignment(GTK_MISC(action_i_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_i_l,
		0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0
	);
	action_i = gtk_entry_new();
	if(opts_v->i) gtk_entry_set_text(GTK_ENTRY(action_i), opts_v->i);
	gtk_widget_set_tooltip_text(
		action_i,
		_("ADD these tags (separated by commas) to found files")
	);
	g_signal_connect(
		action_i, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_i, 1, 2, 0, 1);
/* Строка ввода тэгов для удаления */
	action_d_l = gtk_label_new(_("Delete these tags from found files:"));
	gtk_misc_set_alignment(GTK_MISC(action_d_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_d_l,
		0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0
	);
	action_d = gtk_entry_new();
	if(opts_v->d) gtk_entry_set_text(GTK_ENTRY(action_d), opts_v->d);
	gtk_widget_set_tooltip_text(
		action_d,
		_("DELETE these tags (separated by commas) from found files")
	);
	g_signal_connect(
		action_d, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_d, 1, 2, 1, 2);
/* Строка ввода тэгов для замены */
	action_c_l = gtk_label_new(_("Replace tags in found files:"));
	gtk_misc_set_alignment(GTK_MISC(action_c_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_c_l,
		0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0
	);
	action_c = gtk_entry_new();
	if(opts_v->c) gtk_entry_set_text(GTK_ENTRY(action_c), opts_v->c);
	gtk_widget_set_tooltip_text(
		action_c,
		_("REPLACE in found files some tags with others. Tags in this field "
		"are listed pairs with commas. For example, if you enter in this field "
		"[sushi, meat, cadillac, motorcycle], then in found files tag “sushi” "
		"will be replaced with tag “meat” and tag “cadillac” will be replaced "
		"with tag “motorcycle”.")
	);
	g_signal_connect(
		action_c, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), NULL
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_c, 1, 2, 2, 3);
/* Нижняя строка с флагами и пр. */
	hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_end(GTK_BOX(vbox), hbox2, FALSE, TRUE, 0);
/* Сохранять ссылки в (CheckButton) */
	begin_savecb = gtk_check_button_new_with_label(_("Save symlinks to:"));
	g_signal_connect(
		GTK_CHECK_BUTTON(begin_savecb), "toggled",
		G_CALLBACK(begin_savecb_toggled), NULL
	);
	gtk_widget_set_tooltip_text(
		begin_savecb,
		_("If checked, symlinks to found files are saved to the directory, "
		"selected righter. It can be useful for albums compilation.")
	);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_savecb, FALSE, FALSE, 0);
/* Сохранять ссылки в (FileChooserButton) */
	begin_savefc = gtk_file_chooser_button_new(
		_("Save symlinks to…"),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
	);
	gtk_widget_set_sensitive(begin_savefc, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_savefc, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(
		begin_savefc,
		_("Directory in which the symlinks to found files are saved")
	);
/* Сохранять имена (CheckButton) */
	begin_keepcb = gtk_check_button_new_with_label(_("Save names"));
	gtk_widget_set_tooltip_text(
		begin_keepcb,
		_("If checked, symlinks to found files are saved with names of files "
		"themselves. Otherwise symlinks get random names. Set this option only "
		"if you are sure that all the files have unique names.")
	);
	if(opts_v->k) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(begin_keepcb), TRUE);
	}
	gtk_widget_set_sensitive(begin_keepcb, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_keepcb, FALSE, FALSE, 0);
/* Кнопка «Начать» */
	begin_button = gtk_button_new_with_label(_("Start"));
	gtk_widget_set_tooltip_text(
		begin_button,
		_("Start search (and operations with found files if they "
		"are specified). The search can take quite a long time, "
		"if there is a large number of files.")
	);
	g_signal_connect(
		begin_button, "clicked", G_CALLBACK(gtk_begin), NULL
	);
	gtk_box_pack_end(GTK_BOX(hbox2), begin_button, FALSE, FALSE, 0);
/* Создание кнопок путей по аргументам */
	for(int i = 0; i < opts_v->args_count; i++) {
		add_path(opts_v->args[i]);
	}
	if(!!opts_v->s) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(begin_savecb), TRUE);
		gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(begin_savefc), opts_v->s
		);
	}
/* Запустить всю эту петрушку */
	gtk_widget_show_all(window);
	gtk_main();
}
static void gtk_begin() {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(begin_keepcb)))
		opts_v->k = 1;
	else opts_v->k = 0;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(begin_savecb)))
		opts_v->s =
			gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(begin_savefc));
	else opts_v->s = NULL;
	opts_v->a = entry_text(GTK_ENTRY(search_a));
	opts_v->o = entry_text(GTK_ENTRY(search_o));
	opts_v->n = entry_text(GTK_ENTRY(search_n));
	opts_v->i = entry_text(GTK_ENTRY(action_i));
	opts_v->d = entry_text(GTK_ENTRY(action_d));
	opts_v->c = entry_text(GTK_ENTRY(action_c));
	paths_from_buttons(opts_v);
	if(opts_v->args_count > 0) begin();
}
static char* entry_text(GtkEntry *entry) {
	char *entry_text = (char*) gtk_entry_get_text(GTK_ENTRY(entry));
	Reresult *reresult = regexpmatch(entry_text, "\\S", 0, 0, 8);
	if(reresult->count != -1) {
		free(reresult->indexes);
		free(reresult);
		return entry_text;
	} else {
		free(reresult);
		return NULL;
	}
}
static void paths_from_buttons(Opts *o) {
	Wdgt *wdgt_b;
	char *path;
	for(int i = 0; i < o->args_count; i++) free(o->args[i]);
	o->args_count = 0;
	free(o->args);
	if(findin_b_count > 0) {
		o->args = (char**) malloc(sizeof(char*));
		if(!o->args) {
			fprintf(
				stderr,
				_("paths_from_buttons(): malloc() failed: "
				"insufficient memory.\n")
			);
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < findin_b_count; i++) {
		wdgt_b = wdgt_get_by_n(findin_b_head, i);
		path = (char*) gtk_button_get_label(GTK_BUTTON(wdgt_b->widget));
		o->args[i] = (char*) malloc(strlen(path) + 1);
		if(!o->args[i]) {
			fprintf(
				stderr,
				_("paths_from_buttons(): malloc() failed: "
				"insufficient memory.\n")
			);
			exit(EXIT_FAILURE);
		}
		strcpy(o->args[i], path);
		o->args_count++;
	}
}
static void findin_p_clicked(void) {
	gchar *p_chosen = choose_path(last_path);
	if(p_chosen) {
		add_path(p_chosen);
		last_path = p_chosen;
	}
}
static void findin_b_clicked(GtkWidget *b) {
	Wdgt *wdgt = wdgt_get_by_widget(findin_b_head, b);
	gchar *p_chosen = choose_path(wdgt->string);
	if(p_chosen) {
		if(!was_path(findin_b_head, p_chosen)) {
			gtk_button_set_label(GTK_BUTTON(b), p_chosen);
			wdgt->string = last_path = p_chosen;
		}
	}
}
static void findin_m_clicked(GtkWidget *m, GtkWidget *b) {
	Wdgt *wdgt_b, *wdgt_m;
	int p = wdgt_remove_by_widget(&findin_b_head, b);
	wdgt_remove_by_widget(&findin_m_head, m);
	gtk_widget_destroy(b);
	gtk_widget_destroy(m);
	findin_b_count--;
	for(int i = findin_b_count - p; i < findin_b_count; i++) {
		wdgt_b = wdgt_get_by_n(findin_b_head, findin_b_count - i - 1);
		wdgt_m = wdgt_get_by_n(findin_m_head, findin_b_count - i - 1);
		g_object_ref(wdgt_b->widget);
		g_object_ref(wdgt_m->widget);
		gtk_container_remove(GTK_CONTAINER(findin_table), wdgt_b->widget);
		gtk_container_remove(GTK_CONTAINER(findin_table), wdgt_m->widget);
		gtk_table_attach_defaults(
			GTK_TABLE(findin_table),
			wdgt_b->widget,
			1, 2, i, i + 1
		);
		gtk_table_attach(
			GTK_TABLE(findin_table),
			wdgt_m->widget,
			2, 3, i, i + 1,
			GTK_SHRINK, GTK_FILL,
			0, 0
		);
		g_object_unref(wdgt_b->widget);
		g_object_unref(wdgt_m->widget);
	}
	if(findin_b_count > 0) {
		gtk_table_resize(GTK_TABLE(findin_table), findin_b_count, 3);
	}
}
static xmlDoc* tree(char *path, char *root) {
	FILE *xml_file = fopen(path, "rt");
	if(!xml_file) {
		fprintf(stderr, _("tree(): Cannot open %s.\n"), path);
		return NULL;
	}
	size_t xml_text_size_step = 256, xml_text_size = xml_text_size_step, i = 0;
	char *xml_text = (char*) malloc(xml_text_size);
	if(!xml_text) {
		fprintf(
			stderr,
			_("tree(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	while((xml_text[i++] = fgetc(xml_file)) != EOF) {
		if(strlen(xml_text) > xml_text_size - 1) {
			xml_text =
				(char*) realloc(xml_text, xml_text_size += xml_text_size_step);
			if(!xml_text) {
				fprintf(
					stderr,
					_("tree(): realloc() failed: insufficient memory.\n")
				);
				return NULL;
			}
		}
	}
	xml_text[i] = '\0';
	fclose(xml_file);
	char *re = (char*) malloc(strlen(root) * 2 + 9);
	if(!re) {
		fprintf(stderr, _("tree(): malloc() failed: insufficient memory.\n"));
		return NULL;
	}
	re[0] = '\0';
	strcat(re, "<");
	strcat(re, root);
	strcat(re, ">.*?</");
	strcat(re, root);
	strcat(re, ">\0");
	Reresult *reresult =
		regexpmatch(xml_text, re, 0, PCRE_DOTALL | PCRE_MULTILINE, 8);
	if(reresult->count < 1) {
		free(reresult);
		fprintf(stderr, _("tree(): Cannot find tags tree in %s.\n"), path);
		return NULL;
	}
	free(re);
	xml_text += reresult->indexes[0];
	xml_text[reresult->indexes[1] - reresult->indexes[0]] = '\0';
	free(reresult->indexes);
	free(reresult);
	xmlDoc *tree = xmlParseDoc((xmlChar*) xml_text);
	return tree;
}
static GtkWidget* build_tree(xmlDoc* tree) {
	if(!tree) return NULL;
	int columns_count = 2;
	char *column_names[] = {"name", "kw"};
	GtkTreeViewColumn *columns[columns_count];
	GtkTreeStore *treestore =
		gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	GtkWidget *treeview =
		gtk_tree_view_new_with_model(GTK_TREE_MODEL(treestore));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
	for(int i = 0; i < columns_count; i++) {
		columns[i] = gtk_tree_view_column_new_with_attributes(
			column_names[i],
			gtk_cell_renderer_text_new(),
			"text", i,
			NULL
		);
		gtk_tree_view_append_column(
			GTK_TREE_VIEW(treeview),
			columns[i]
		);
	}
	gtk_tree_view_column_set_visible(columns[1], FALSE);
	for(
		xmlNodePtr node = xmlDocGetRootElement(tree)->children;
		node;
		node = node->next
	) {
		append_children(
			gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)),
			node,
			NULL,
			columns_count
		);
	}
	return treeview;
}
static void append_children(
	GtkTreeModel *model,
	xmlNodePtr child,
	GtkTreeIter *parent,
	int columns_count
) {
	if(child->properties) {
		GtkTreeIter iter;
		gtk_tree_store_append(GTK_TREE_STORE(model), &iter, parent);
		parent = &iter;
		gtk_tree_store_set(
			GTK_TREE_STORE(model),
			&iter,
			0, child->properties->children->content,
			1, child->properties->next->children->content,
			-1
		);
		for(child = child->children; child; child = child->next) {
			append_children(model, child, parent, columns_count);
		}
	}
}
static int treeview_clicked(GtkWidget *widget, GdkEventButton *event) {
	if(event->type == GDK_2BUTTON_PRESS && event->button == 1) {
		entry_drag_data(widget, "expand", 0, NULL);
	}
	if(event->type == GDK_2BUTTON_PRESS && event->button == 3) {
		entry_drag_data(widget, NULL, 1, search_a);
	}
	return 0;
}
static void entry_drag_data_received(
	GtkWidget *widget,
	GdkDragContext *context,
	gint x,
	gint y,
	GtkSelectionData *data,
	guint info,
	guint time,
	gpointer user_data
) {
	Widget_aa d;
	if(user_data) {
		d = *(Widget_aa*) data;
	} else {
		d.widget = treeview;
		d.action = NULL;
		d.append = 1;
		d.recipient = widget;
	}
	entry_drag_data(d.widget, d.action, d.append, d.recipient);
}
static void entry_drag_data(
	GtkWidget *widget,
	char *action,
	int append,
	GtkWidget *recipient
) {
	Widget_aa d;
	d.widget = widget;
	d.action = action;
	d.append = append;
	d.recipient = recipient;
	gtk_tree_selection_selected_foreach(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(d.widget)),
		treeview_foreach_selected,
		&d
	);
}
static void treeview_foreach_selected(
	GtkTreeModel *model,
	GtkTreePath *path,
	GtkTreeIter *iter,
	gpointer data
) {
	Widget_aa *d = (Widget_aa*) data;
	if(d->action && strcmp(d->action, "expand") == 0) {
		gtk_tree_view_row_expanded(GTK_TREE_VIEW(d->widget), path)
			? gtk_tree_view_collapse_row(GTK_TREE_VIEW(d->widget), path)
			: gtk_tree_view_expand_row(GTK_TREE_VIEW(d->widget), path, FALSE);
	} else {
		gchar *tagname, *involved;
		gtk_tree_model_get(model, iter, 0, &tagname, 1, &involved, -1);
		if(strcmp(involved, "true") == 0) {
			gchar *new;
			int allocated = 0;
			if(d->append) {
				new = entry_text(GTK_ENTRY(d->recipient));
				if(new) {
					new = strconcat((const char*[]) {new, ", ", tagname}, 3);
					allocated = 1;
				} else new = tagname;
			} else new = tagname;
			gtk_entry_set_text(GTK_ENTRY(d->recipient), new);
			if(allocated) free(new);
		}
		free(tagname);
		free(involved);
	}
}
static void begin_savecb_toggled(GtkWidget *widget) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_widget_set_sensitive(begin_savefc, TRUE);
		gtk_widget_set_sensitive(begin_keepcb, TRUE);
	} else {
		gtk_widget_set_sensitive(begin_savefc, FALSE);
		gtk_widget_set_sensitive(begin_keepcb, FALSE);
	}
}
static gchar* choose_path(gchar *initial) {
	gchar *path;
	GtkWidget *c = gtk_file_chooser_dialog_new(
		_("Search in…"), NULL,
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL
	);
	if(!gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(c), initial)) {
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(c), last_path);
	};
	path = gtk_dialog_run(GTK_DIALOG(c)) == GTK_RESPONSE_ACCEPT
		? gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(c))
		: NULL;
	gtk_widget_destroy(c);
	return path;
}
static void add_path(gchar *path) {
	if(was_path(findin_b_head, path)) {
		show_message(
			GTK_MESSAGE_ERROR,
			_("This path is already added. Choose another.")
		);
		return;
	}
	GtkWidget *n_findin_b =
		wdgt_push(
			&findin_b_head, gtk_button_new_with_label(path), path
		)->widget;
	GtkWidget *n_findin_m =
		wdgt_push(
			&findin_m_head, gtk_button_new_with_label("-"), path
		)->widget;
	findin_b_count++;
	g_signal_connect(
		n_findin_b, "clicked", G_CALLBACK(findin_b_clicked), n_findin_b
	);
	g_signal_connect(
		n_findin_m, "clicked", G_CALLBACK(findin_m_clicked), n_findin_b
	);
	gtk_button_set_alignment(GTK_BUTTON(n_findin_b), 0, 0);
	gtk_table_attach_defaults(
		GTK_TABLE(findin_table),
		n_findin_b,
		1, 2, findin_b_count - 1, findin_b_count
	);
	gtk_table_attach(
		GTK_TABLE(findin_table),
		n_findin_m,
		2, 3, findin_b_count - 1, findin_b_count,
		GTK_SHRINK, GTK_FILL,
		0, 0
	);
	gtk_widget_set_tooltip_text(n_findin_b, _("Change the path to search"));
	gtk_widget_set_tooltip_text(n_findin_m, _("Remove the path to search"));
	gtk_widget_show(n_findin_b);
	gtk_widget_show(n_findin_m);
}
static Wdgt* was_path(Wdgt *head, gchar *path) {
	while(head) {
		if(!strcmp(head->string, path)) return head;
		head = head->prev;
	}
	return NULL;
}
void show_message(GtkMessageType type, gchar *message) {
	GtkWidget *dialog = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		type,
		GTK_BUTTONS_CLOSE,
		message
	);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
static Wdgt* wdgt_push(Wdgt **head, GtkWidget *widget, gchar* string) {
	Wdgt *tmp = (Wdgt*) malloc(sizeof(Wdgt));
	if(!tmp) {
		fprintf(
			stderr,
			_("wdgt_push(): malloc() failed: insufficient memory.\n")
		);
		return NULL;
	}
	tmp->widget = widget;
	tmp->string = string;
	tmp->prev = (*head);
	(*head) = tmp;
	return (*head);
}
static int wdgt_remove_by_widget(Wdgt **head, GtkWidget *widget) {
	Wdgt *removing, *first;
	int isfirst = 1, position = 0;
	while(*head) {
		if(isfirst) {
			first = *head;
			if((*head)->widget == widget) {
				removing = (*head);
				(*head) = (*head)->prev ? (*head)->prev : NULL;
				free(removing);
				return position;
			}
			isfirst = 0;
		}
		if((*head)->prev->widget == widget) {
			removing = (*head)->prev;
			(*head)->prev = (*head)->prev->prev ? (*head)->prev->prev : NULL;
			free(removing);
			*head = first;
			return position + 1;
		}
		(*head) = (*head)->prev;
		position++;
	}
	return -1;
}
static Wdgt* wdgt_get_by_widget(Wdgt *head, GtkWidget *widget) {
	while(head) {
		if(head->widget == widget) return head;
		head = head->prev;
	}
	return NULL;
}
static Wdgt* wdgt_get_by_n(Wdgt *head, int n) {
	int i = 0;
	while(n > i++ && head) {
		head = head->prev;
	}
	return head;
}
