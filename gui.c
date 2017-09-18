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

#include <malloc.h>
#include <string.h>
#include <gtk/gtk.h>
#include <pcre.h>
#include "gui.h"
#include "help.h"
#include "regexpmatch.h"

extern Opts *opts;
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
	*scrolledwindow;

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
		"FindByTags — "
		"Поиск картинок по тэгам и операции с тэгами найденных картинок"
	);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 100);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
/* Горизонтальный контейнер */
	hpaned = gtk_hpaned_new();
	gtk_container_add(GTK_CONTAINER(window), hpaned);
/* Левый контейнер */
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_widget_set_size_request(vbox, 370, -1);
	GtkTreeView *treeview;//assador
	if(0) {//assador
	} else {
		gtk_paned_pack1(GTK_PANED(hpaned), vbox, TRUE, FALSE);
	}
/* Фрэйм «Искать в» */
	findin_frame = gtk_frame_new(" Искать в ");
	gtk_box_pack_start(GTK_BOX(vbox), findin_frame, FALSE, TRUE, 0);
/* Таблица с кнопками путей для поиска */
	findin_table = gtk_table_new(1, 3, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(findin_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(findin_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(findin_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(findin_frame), findin_table);
/* Кнопка добавления пути для поиска */
	findin_p = gtk_button_new_with_label("+");
	gtk_widget_set_tooltip_text(findin_p, "Добавить новый путь для поиска");
	g_signal_connect(findin_p, "clicked", G_CALLBACK(findin_p_clicked), NULL);
	gtk_table_attach(
		GTK_TABLE(findin_table),
		findin_p,
		0, 1, 0, 1,
		GTK_SHRINK, GTK_FILL,
		0, 0
	);
/* Поиск по тэгам */
	search_frame = gtk_frame_new(" Поиск по тэгам ");
	gtk_box_pack_start(GTK_BOX(vbox), search_frame, FALSE, TRUE, 0);
/* Таблица с полями ввода тэгов для поиска */
	search_table = gtk_table_new(3, 2, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(search_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(search_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(search_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(search_frame), search_table);
/* Строка ввода тэгов для поиска И */
	search_a_l = gtk_label_new("Присутствуют все тэги из:");
	gtk_misc_set_alignment(GTK_MISC(search_a_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_a_l,
		0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0
	);
	search_a = gtk_entry_new();
	if(opts_v->a) gtk_entry_set_text(GTK_ENTRY(search_a), opts_v->a);
	gtk_widget_set_tooltip_text(
		search_a,
		"Поиск ведётся по файлам, в которых\nПРИСУТСТВУЮТ ВСЕ\n"
		"из перечисленных здесь тэгов (разделённых запятыми)"
	);
	g_signal_connect(
		search_a, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_a, 1, 2, 0, 1);
/* Строка ввода тэгов для поиска ИЛИ */
	search_o_l = gtk_label_new("Присутствует хотя бы один тэг из:");
	gtk_misc_set_alignment(GTK_MISC(search_o_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_o_l,
		0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0
	);
	search_o = gtk_entry_new();
	if(opts_v->o) gtk_entry_set_text(GTK_ENTRY(search_o), opts_v->o);
	gtk_widget_set_tooltip_text(
		search_o,
		"Поиск ведётся по файлам, в которых\nПРИСУТСТВУЕТ ХОТЯ БЫ ОДИН\n"
		"из перечисленных здесь тэгов (разделённых запятыми)"
	);
	g_signal_connect(
		search_o, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_o, 1, 2, 1, 2);
/* Строка ввода тэгов для поиска НЕ */
	search_n_l = gtk_label_new("Нет ни одного тэга из:");
	gtk_misc_set_alignment(GTK_MISC(search_n_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(search_table), search_n_l,
		0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0
	);
	search_n = gtk_entry_new();
	if(opts_v->n) gtk_entry_set_text(GTK_ENTRY(search_n), opts_v->n);
	gtk_widget_set_tooltip_text(
		search_n,
		"Поиск ведётся по файлам, в которых\nНЕТ НИ ОДНОГО\n"
		"из перечисленных здесь тэгов (разделённых запятыми)"
	);
	g_signal_connect(
		search_n, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(search_table), search_n, 1, 2, 2, 3);
/* Действия с найденным */
	action_frame = gtk_frame_new(" Действия с найденным ");
	gtk_box_pack_start(GTK_BOX(vbox), action_frame, FALSE, TRUE, 0);
/* Таблица с полями ввода тэгов для поиска */
	action_table = gtk_table_new(3, 2, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(action_table), 5);
	gtk_table_set_col_spacing(GTK_TABLE(action_table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(action_table), 1, 5);
	gtk_container_add(GTK_CONTAINER(action_frame), action_table);
/* Строка ввода тэгов для добавления */
	action_i_l = gtk_label_new("Добавить тэги в найденные файлы:");
	gtk_misc_set_alignment(GTK_MISC(action_i_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_i_l,
		0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0
	);
	action_i = gtk_entry_new();
	if(opts_v->i) gtk_entry_set_text(GTK_ENTRY(action_i), opts_v->i);
	gtk_widget_set_tooltip_text(
		action_i,
		"ДОБАВИТЬ в найденные файлы перечисленные здесь тэги "
		"(разделённые запятыми)"
	);
	g_signal_connect(
		action_i, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_i, 1, 2, 0, 1);
/* Строка ввода тэгов для удаления */
	action_d_l = gtk_label_new("Удалить тэги из найденных файлов:");
	gtk_misc_set_alignment(GTK_MISC(action_d_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_d_l,
		0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0
	);
	action_d = gtk_entry_new();
	if(opts_v->d) gtk_entry_set_text(GTK_ENTRY(action_d), opts_v->d);
	gtk_widget_set_tooltip_text(
		action_d,
		"УДАЛИТЬ из найденных файлов перечисленные здесь тэги "
		"(разделённые запятыми)"
	);
	g_signal_connect(
		action_d, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_d, 1, 2, 1, 2);
/* Строка ввода тэгов для замены */
	action_c_l = gtk_label_new("Заменить тэги в найденных файлах:");
	gtk_misc_set_alignment(GTK_MISC(action_c_l), 1, 0.5);
	gtk_table_attach(
		GTK_TABLE(action_table), action_c_l,
		0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0
	);
	action_c = gtk_entry_new();
	if(opts_v->c) gtk_entry_set_text(GTK_ENTRY(action_c), opts_v->c);
	gtk_widget_set_tooltip_text(
		action_c,
		"ЗАМЕНИТЬ в найденных файлах одни тэги на другие. Тэги перечисляются "
		"в этом поле попарно через запятые. Например, если ввести в это поле "
		"[овсянка, борщ, лада калина, мотоцикл], то в найденных файлах "
		"тэг «овсянка» заменится на «борщ», а тэг «лада калина» заменится "
		"на «мотоцикл»."
	);
	g_signal_connect(
		action_c, "drag-data-received",
		G_CALLBACK(entry_drag_data_received), treeview
	);
	gtk_table_attach_defaults(GTK_TABLE(action_table), action_c, 1, 2, 2, 3);
/* Нижняя строка с флагами и пр. */
	hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_end(GTK_BOX(vbox), hbox2, FALSE, TRUE, 0);
/* Сохранять ссылки в (CheckButton) */
	begin_savecb = gtk_check_button_new_with_label("Сохранять ссылки в:");
	g_signal_connect(
		GTK_CHECK_BUTTON(begin_savecb), "toggled",
		G_CALLBACK(begin_savecb_toggled), NULL
	);
	gtk_widget_set_tooltip_text(
		begin_savecb,
		"Если отмечено, в выбранном правее каталоге сохраняются "
		"символические ссылки на найденные файлы. Это может быть удобно "
		"для составления альбомов."
	);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_savecb, FALSE, FALSE, 0);
/* Сохранять ссылки в (FileChooserButton) */
	begin_savefc = gtk_file_chooser_button_new(
		"Сохранять ссылки в…",
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER
	);
	gtk_widget_set_sensitive(begin_savefc, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_savefc, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(
		begin_savefc,
		"Каталог, в котором сохраняются "
		"символические ссылки на найденные файлы."
	);
/* Сохранять имена (CheckButton) */
	begin_keepcb = gtk_check_button_new_with_label("Сохранять имена");
	gtk_widget_set_tooltip_text(
		begin_keepcb,
		"Если отмечено, символические ссылки на найденные файлы сохраняются "
		"с именами самих файлов. В противном случае ссылки получают "
		"случайные имена. Ставьте эту галочку только если уверены, "
		"что все найденные файлы имеют уникальные имена."
	);
	if(opts_v->k) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(begin_keepcb), TRUE);
	}
	gtk_widget_set_sensitive(begin_keepcb, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), begin_keepcb, FALSE, FALSE, 0);
/* Кнопка «Начать» */
	begin_button = gtk_button_new_with_label("Начать");
	gtk_widget_set_tooltip_text(
		begin_button,
		"Начать поиск (и операции с найденными файлами, если они заданы). "
		"Поиск может продолжаться довольно долго, если происходит "
		"по большому количеству файлов."
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
	}
/* Запустить всю эту петрушку */
	gtk_widget_show_all(window);
	gtk_main();
}
static void gtk_begin() {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(begin_keepcb)))
		opts_v->k = 1;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(begin_savecb)))
		opts_v->s =
			gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(begin_savefc));
	opts_v->a = entry_text(GTK_ENTRY(search_a));
	opts_v->o = entry_text(GTK_ENTRY(search_o));
	opts_v->n = entry_text(GTK_ENTRY(search_n));
	opts_v->i = entry_text(GTK_ENTRY(action_i));
	opts_v->d = entry_text(GTK_ENTRY(action_d));
	opts_v->c = entry_text(GTK_ENTRY(action_c));
	for(int i = 0; i < opts_v->args_count; i++) free(opts_v->args[i]);
	opts_v->args_count = 0;
	free(opts_v->args);
	paths_from_buttons(opts_v);
	if(opts_v->args_count > 0) begin();
}
static char* entry_text(GtkEntry *entry) {
	char *entry_text = (char*) gtk_entry_get_text(GTK_ENTRY(entry));
	Reresult *reresult = regexpmatch(entry_text, "\\S", 0, 8);
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
	if(findin_b_count > 0) {
		o->args = (char**) malloc(sizeof(char*));
		if(!o->args) {
			fprintf(stderr, "malloc() failed: insufficient memory.\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < findin_b_count; i++) {
		wdgt_b = wdgt_get_by_n(findin_b_head, i);
		path = (char*) gtk_button_get_label(GTK_BUTTON(wdgt_b->widget));
		o->args[i] = (char*) malloc(strlen(path) + 1);
		if(o->args[i]) o->args[i] = path;
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
static void entry_drag_data_received() {//assador
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
		"Искать в…", NULL,
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
			"Этот путь уже добавлен. Выберите другой."
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
	gtk_widget_set_tooltip_text(n_findin_b, "Изменить путь для поиска");
	gtk_widget_set_tooltip_text(n_findin_m, "Удалить путь для поиска");
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
		fprintf(stderr, "malloc() failed: insufficient memory.\n");
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
