#
# libmsvg source Makefile. Uses GNU Make.
#
.PHONY : clean install uninstall

include ../makedefs

CFLAGS+= -I../expat/xmlparse

LIBOBJS=tables.o \
        element.o \
        manielem.o \
        attribut.o \
        raw2cook.o \
        printree.o \
        rdsvgf.o \
        wtsvgf.o \
        util.o

LIB=libmsvg.a

EXPATOBJS=../expat/xmltok/xmltok.o \
          ../expat/xmltok/xmlrole.o \
          ../expat/xmlparse/xmlparse.o
EXPATOBJSdos= $(subst /,\,$(EXPATOBJS))

### LINUX VERSION

ifeq ($(LINUX_VERSION),y)

all: $(LIB)

$(LIB): $(LIBOBJS) $(EXPATOBJS)
	$(AR) rc $(LIB) $(LIBOBJS) $(EXPATOBJS)

clean:
	rm -f $(LIBOBJS) $(LIB)

install: $(LIB)
	install -m 0755 -d $(INSTALLDIR)/lib
	install -m 0755 -d $(INSTALLDIR)/include
	install -m 0644 $(LIB) $(INSTALLDIR)/lib
	install -m 0644 msvg.h $(INSTALLDIR)/include

uninstall:
	rm -f $(INSTALLDIR)/lib/$(notdir $(LIB))
	rm -f $(INSTALLDIR)/include/msvg.h

### DJGPP or MINGW VERSION

else

all: $(LIB)

$(LIB): $(LIBOBJS) $(EXPATOBJSdos)
	$(AR) rc $(LIB) $(LIBOBJS) $(EXPATOBJSdos)

clean:
	del *.o
	del $(LIB)

install: $(LIB)
	copy $(LIB) $(INSTALLDIR)\lib
	copy msvg.h $(INSTALLDIR)\include

uninstall:
	del $(INSTALLDIR)\lib\$(LIB)
	del $(INSTALLDIR)\include\msvg.h

endif

### COMMON

$(LIBOBJS): msvg.h

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
