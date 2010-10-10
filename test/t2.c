/* t2.c
 *
 * libmsvg, a minimal library to read and write svg files
 * Copyright (C) 2010 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This is a test/demo file of the libmsvg library.
 * libmsvg test/demo are in the Public Domain, this apply only to test/demo
 * files, the library itself is under the terms of the GNU Library General
 * Public License.
 *
 */

#include <stdio.h>
#include "msvg.h"

#define TESTFILE "msvgt2.svg"

int main(int argc, char **argv)
{
  MsvgElement *root, *son;

  root = MsvgNewElement(EID_SVG, NULL);
  MsvgAddAttribute(root, "version", "1.2");
  MsvgAddAttribute(root, "baseProfile", "tiny");
  MsvgAddAttribute(root, "viewBox", "0 0 400 400");

  son = MsvgNewElement(EID_RECT, root);
  MsvgAddAttribute(son, "x", "1");
  MsvgAddAttribute(son, "y", "1");
  MsvgAddAttribute(son, "width", "398");
  MsvgAddAttribute(son, "height", "398");
  MsvgAddAttribute(son, "stroke", "#F00");
  MsvgAddAttribute(son, "fill", "#FFF");

  son = MsvgNewElement(EID_RECT, root);
  MsvgAddAttribute(son, "x", "11");
  MsvgAddAttribute(son, "y", "11");
  MsvgAddAttribute(son, "width", "380");
  MsvgAddAttribute(son, "height", "380");
  MsvgAddAttribute(son, "stroke", "#0F0");
  MsvgAddAttribute(son, "fill", "none");
  MsvgPrintElementTree(stdout, root, 0);

  if (!MsvgWriteSvgFile(root, TESTFILE)) {
    printf("Error writing %s\n", TESTFILE);
    return 0;
  }

  return 1;
}
