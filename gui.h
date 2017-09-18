#include "structopts.h"

typedef struct Wdgt {
	GtkWidget *widget;
	gchar *string;
	struct Wdgt *prev;
} Wdgt;

static Wdgt* wdgt_push(Wdgt **head, GtkWidget *widget, gchar* string);
static int wdgt_remove_by_widget(Wdgt **head, GtkWidget *widget);
static Wdgt* wdgt_get_by_n(Wdgt *head, int n);
static Wdgt* wdgt_get_by_widget(Wdgt *head, GtkWidget *widget);

void gui();
static char* entry_text(GtkEntry *entry);
static void findin_p_clicked(void);
static void findin_b_clicked(GtkWidget *b);
static void findin_m_clicked(GtkWidget *b, GtkWidget *m);
static gchar* choose_path(gchar *initial);
static void add_path(gchar *path);
static Wdgt* was_path(Wdgt *head, gchar *path);
static void entry_drag_data_received();
static void paths_from_buttons(Opts *o);
static void begin_savecb_toggled(GtkWidget *widget);
static void gtk_begin();
void show_message(GtkMessageType type, gchar *message);
