#
# libmsvg tests Makefile. Uses GNU Make.
#
.PHONY : clean

include ../makedefs

CFLAGS+= -I../src

LIBS=../src/libmsvg.a -lm
LIBSdos= $(subst /,\,$(LIBS))

SAMPLES=tread$(EXE) \
        tbuild$(EXE) \
        tdel$(EXE) \
        tcook$(EXE)

### LINUX VERSION

ifeq ($(LINUX_VERSION),y)

all: $(SAMPLES)

$(SAMPLES): %$(EXE) : %.o $(LIBS)
	$(CC) $(CFLAGS) -o $*$(EXE) $*.o $(LIBS)

clean:
	rm -f *.o $(SAMPLES)
	rm -f msvgt*.svg

### DJGPP or MINGW VERSION

else

all: $(SAMPLES)

$(SAMPLES): %$(EXE) : %.o $(LIBSdos)
	$(CC) $(CFLAGS) -o $*$(EXE) $*.o $(LIBSdos)

clean:
	del *.o
	del *.exe
	del msvgt*.svg

endif

### COMMON

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
