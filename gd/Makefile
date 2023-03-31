#
# libmsvg+GD tests Makefile. Uses GNU Make.
#
# it expects both libmsvg and GD libs installed in advance
#
.PHONY : clean

LIBS=-L../src -lmsvg -lgd -lpng -lz -lm
CFLAGS+=-I../src -Wall -g

all: rsvg gsvg svg2png

rsvg: rsvg.c rendgd.o rendgd.h
	gcc $(CFLAGS) -o rsvg rsvg.c rendgd.o $(LIBS)
gsvg: gsvg.c rendgd.o rendgd.h
	gcc $(CFLAGS) -o gsvg gsvg.c rendgd.o $(LIBS)
svg2png: svg2png.c rendgd.o rendgd.h
	gcc $(CFLAGS) -o svg2png svg2png.c rendgd.o $(LIBS)
rendgd.o: rendgd.c rendgd.h
	gcc $(CFLAGS) -c rendgd.c

clean:
	rm -f *.o
	rm -f rsvg
	rm -f gsvg
	rm -f svg2png
