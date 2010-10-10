#
# libmsvg global Makefile
#
.PHONY : expat lib test clean install uninstall

#include makedefs

all: expat lib test

expat:
	$(MAKE) -C expat -f Makefile

lib: expat
	$(MAKE) -C src -f Makefile

test: expat lib
	$(MAKE) -C test -f Makefile

clean:
	$(MAKE) -C test -f Makefile clean
	$(MAKE) -C src -f Makefile clean
	$(MAKE) -C expat -f Makefile clean

install:
	$(MAKE) -C src -f Makefile install
 
uninstall:
	$(MAKE) -C src -f Makefile uninstall 
