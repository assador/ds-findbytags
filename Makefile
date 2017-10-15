SHELL = /bin/sh
.SUFFIXIES:
.SUFFIXIES: .c .o

CC = gcc
CFLAGS = -g -c -Wall
LDFLAGS =
CFLAGS += `pkg-config --cflags gtk+-2.0 libpcre`
CFLAGS += `xml2-config --cflags`
LDLIBS += `pkg-config --libs gtk+-2.0 libpcre`
LDLIBS += `xml2-config --libs`

TARGET = ds-findbytags
PREFIX = /usr/local
LOCALE = /usr/share/locale
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
	--output $(POTDIR)/$(TARGET).pot $(SOURCES) $(SRCDIR)/constants.h
clean:
	rm -rf $(OBJDIR)/*.o $(BINDIR)/$(TARGET) $(POTDIR)/*.pot $(POTDIR)/*/*.mo
install: all
	mkdir -p $(PREFIX)/bin
	install -m 755 $(BINDIR)/$(TARGET) $(PREFIX)/bin
	mkdir -p $(LOCALE)/ru/LC_MESSAGES
	install -m 644 $(POTDIR)/ru/$(TARGET).mo $(LOCALE)/ru/LC_MESSAGES
uninstall:
	rm $(PREFIX)/bin/$(TARGET)
	rm $(LOCALE)/ru/LC_MESSAGES/$(TARGET).mo
