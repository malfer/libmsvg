#
# libmsvg tests Makefile. Uses GNU Make.
#
.PHONY : clean

include ../makedefs

CFLAGS+= -I../src

LIBS=../src/libmsvg.a -lm
LIBSdos= $(subst /,\,$(LIBS))

SAMPLES=tread$(EXE) \
        trbuild$(EXE) \
        tcbuild$(EXE) \
        tdel$(EXE) \
        tcook$(EXE) \
        tfont$(EXE) \
        tpa2poly$(EXE) \
        tbpsrv$(EXE)

### LINUX VERSION

ifeq ($(LINUX_VERSION),y)

all: $(SAMPLES)

$(SAMPLES): %$(EXE) : %.c $(LIBS) ../src/msvg.h
	$(CC) $(CFLAGS) -o $*$(EXE) $*.c $(LIBS)

clean:
	rm -f *.o $(SAMPLES)
	rm -f msvgt*.svg

### DJGPP or MINGW VERSION

else

all: $(SAMPLES)

$(SAMPLES): %$(EXE) : %.c $(LIBSdos) ../src/msvg.h
	$(CC) $(CFLAGS) -o $*$(EXE) $*.c $(LIBSdos)

clean:
	del *.o
	del *.exe
	del msvgt*.svg

endif

### COMMON

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
