CFLAGS+=`pkg-config --cflags gtk+-2.0`
LDLIBS+=`pkg-config --libs gtk+-2.0`
CFLAGS+=`pkg-config --cflags libpcre`
LDLIBS+=`pkg-config --libs libpcre`

ds-findbytags: main.o common.o structopts.o regexpmatch.o help.o gui.o
	gcc $(LDLIBS) -o ds-findbytags main.o common.o structopts.o regexpmatch.o help.o gui.o
main.o: main.c
	gcc $(CFLAGS) -c main.c
common.o: common.c
	gcc -c common.c
structopts.o: structopts.c
	gcc -c structopts.c
regexpmatch.o: regexpmatch.c
	gcc -c regexpmatch.c
help.o: help.c
	gcc $(CFLAGS) -c help.c
gui.o: gui.c
	gcc $(CFLAGS) -c gui.c
clean:
	rm ./*.o ds-findbytags
