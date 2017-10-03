#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)
#include <gtk/gtk.h>
#include "structopts.h"

typedef struct Wdgt {
	GtkWidget *widget;
	gchar *string;
	struct Wdgt *prev;
} Wdgt;

void gui();
void show_message(GtkMessageType type, gchar *message);
