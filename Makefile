SHELL = /bin/sh
.SUFFIXIES:
.SUFFIXIES: .c .o

CC = gcc
CFLAGS = -g -c -Wall
LDFLAGS =
CFLAGS += `pkg-config --cflags gtk+-2.0`
CFLAGS += `pkg-config --cflags libpcre`
LDLIBS += `pkg-config --libs gtk+-2.0`
LDLIBS += `pkg-config --libs libpcre`

TARGET = ds-findbytags
SRCDIR = src
OBJDIR = obj
BINDIR = bin
POTDIR = po
SOURCES := $(wildcard $(SRCDIR)/*.c)
HEADERS := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
POS := $(wildcard $(POTDIR)/*/*.po)
MOS := $(POS:$(POTDIR)/%.po=$(POTDIR)/%.mo)

.PHONY: all clean install uninstall

all: $(BINDIR)/$(TARGET) $(POTDIR)/ru/$(TARGET).mo
$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@
$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(POTDIR)/ru/$(TARGET).mo: $(POTDIR)/ru/$(TARGET).po
	msgfmt --output-file=$@ $<
$(POTDIR)/ru/$(TARGET).po: $(POTDIR)/$(TARGET).pot
	msgmerge --output-file=$@ $@ $<
$(POTDIR)/$(TARGET).pot: $(SOURCES)
	xgettext --keyword=_ --language=C --from-code=UTF-8 \
	--output $(POTDIR)/$(TARGET).pot $(SOURCES)
clean:
	rm -rf $(OBJDIR)/*.o $(BINDIR)/$(TARGET) $(POTDIR)/*.pot $(POTDIR)/*/*.mo
