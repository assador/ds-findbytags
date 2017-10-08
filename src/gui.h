#include <gtk/gtk.h>

typedef struct Wdgt {
	GtkWidget *widget;
	gchar *string;
	struct Wdgt *prev;
} Wdgt;

void gui();
void show_message(GtkMessageType type, gchar *message);
