CFLAGS+=`pkg-config --cflags gtk+-2.0`
LDLIBS+=`pkg-config --libs gtk+-2.0`

ds-findbytags: ds-findbytags.o gui.o
	gcc $(LDLIBS) -o ds-findbytags ds-findbytags.o gui.o
ds-findbytags.o: ds-findbytags.c
	gcc -c ds-findbytags.c
gui.o: gui.c
	gcc $(CFLAGS) -c gui.c
clean:
	rm ds-findbytags ds-findbytags.o gui.o

#stringsarray: stringsarray.o
#	gcc -o stringsarray stringsarray.o
#stringsarray.o: stringsarray.c
#	gcc -c stringsarray.c
#clean:
#	rm stringsarray stringsarray.o
