.PHONY : clean

include ../makedefs

# If you know what your system's byte order is, define XML_BYTE_ORDER:
# use -DXML_BYTE_ORDER=12 for little-endian byte order;
# use -DXML_BYTE_ORDER=21 for big-endian (network) byte order.
# -DXML_NS adds support for checking of lexical aspects of XML namespaces spec
# -DXML_MIN_SIZE makes a smaller but slower parser
# -DXML_DTD adds full support for parsing DTDs
#CFLAGS=-Wall -O2 -Ixmltok -Ixmlparse -DXML_NS -DXML_DTD
CFLAGS+= -Ixmltok -Ixmlparse

LIBOBJS=xmltok/xmltok.o \
        xmltok/xmlrole.o \
        xmlparse/xmlparse.o
LIBOBJSdos=$(subst /,\,$(LIBOBJS))

OBJS=sample/elements.o
OBJSdos=$(subst /,\,$(OBJS))

LIB=xmlparse/libexpat.a
LIBdos=$(subst /,\,$(LIB))

SAMPLE=sample/elements$(EXE)
SAMPLEdos=$(subst /,\,$(SAMPLE))

### LINUX VERSION

ifeq ($(LINUX_VERSION),y)

all: $(SAMPLE)

$(SAMPLE): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB)

$(LIB): $(LIBOBJS)
	$(AR) rc $(LIB) $(LIBOBJS)

clean:
	rm -f $(OBJS) $(LIBOBJS) $(LIB) $(SAMPLE)
	rm -f gennmtab/gennmtab$(EXE)

xmltok/nametab.h: gennmtab/gennmtab$(EXE)
	rm -f $@
	gennmtab/gennmtab$(EXE) >$@

gennmtab/gennmtab$(EXE): gennmtab/gennmtab.c
	$(CC) $(CFLAGS) -o $@ gennmtab/gennmtab.c

xmltok/xmltok.o: xmltok/nametab.h

### DJGPP or MINGW VERSION

else

all: $(SAMPLEdos)

$(SAMPLEdos): $(OBJSdos) $(LIBdos)
	$(CC) $(CFLAGS) -o $@ $(OBJSdos) $(LIBdos)

$(LIBdos): $(LIBOBJSdos)
	$(AR) rc $(LIBdos) $(LIBOBJSdos)

clean:
	del xmltok\*.o
	del xmlparse\*.o
	del sample\*.o
	del $(LIBdos)
	del $(SAMPLEdos)
	del gennmtab\gennmtab$(EXE)

xmltok\nametab.h: gennmtab\gennmtab$(EXE)
	del $@
	gennmtab\gennmtab$(EXE) >$@

gennmtab\gennmtab$(EXE): gennmtab\gennmtab.c
	$(CC) $(CFLAGS) -o $@ gennmtab\gennmtab.c

xmltok\xmltok.o: xmltok\nametab.h

endif

### COMMON

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
