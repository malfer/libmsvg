/* tdel.c
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

int main(int argc, char **argv)
{
  MsvgElement *root, *son1, *son2, *son3;

  root = MsvgNewElement(EID_SVG, NULL);
  MsvgAddAttribute(root, "version", "1.2");
  MsvgAddAttribute(root, "baseProfile", "tiny");
  MsvgAddAttribute(root, "viewBox", "0 0 400 400");

  son1 = MsvgNewElement(EID_RECT, root);
  MsvgAddAttribute(son1, "id", "son1");
  MsvgAddAttribute(son1, "x", "1");
  MsvgAddAttribute(son1, "y", "1");
  MsvgAddAttribute(son1, "width", "398");
  MsvgAddAttribute(son1, "height", "398");
  MsvgAddAttribute(son1, "stroke", "#F00");
  MsvgAddAttribute(son1, "fill", "#FFF");

  son2 = MsvgNewElement(EID_RECT, root);
  MsvgAddAttribute(son2, "x", "11");
  MsvgAddAttribute(son2, "y", "11");
  MsvgAddAttribute(son2, "width", "380");
  MsvgAddAttribute(son2, "height", "380");
  MsvgAddAttribute(son2, "stroke", "#0F0");
  MsvgAddAttribute(son2, "fill", "none");

  son3 = MsvgNewElement(EID_POLYLINE, root);
  MsvgAddAttribute(son3, "stroke", "#0F0");
  MsvgAddAttribute(son3, "stroke-width", "10");
  MsvgAddAttribute(son3, "points", "100,360 200,320 300,360 300,320");

  MsvgRaw2CookedTree(root);
  
  printf("---Original tree\n");
  MsvgPrintElementTree(stdout, root, 0);
  
  MsvgDelAttribute(son1, "width");
  printf("---Deleted \"width\" attribute on son1\n");
  MsvgPrintElementTree(stdout, root, 0);

  MsvgDelAllAttributes(son1);
  printf("---Deleted all attributes on son1\n");
  MsvgPrintElementTree(stdout, root, 0);

  MsvgDelAttribute(son2, "fill");
  printf("---Deleted \"fill\" attribute on son2\n");
  MsvgPrintElementTree(stdout, root, 0);

  MsvgDelAttribute(son2, "x");
  printf("---Deleted \"x\" attribute on son2\n");
  MsvgPrintElementTree(stdout, root, 0);

  MsvgDelAttribute(root, "baseProfile");
  printf("---Deleted \"baseProfile\" attribute on root\n");
  MsvgPrintElementTree(stdout, root, 0);

  MsvgPruneElement(son2);
  printf("---Prune second soon\n");
  MsvgPrintElementTree(stdout, root, 0);
  
  MsvgInsertSonElement(son2, root);
  printf("---Insert second soon at the end\n");
  MsvgPrintElementTree(stdout, root, 0);
  
  MsvgDeleteElement(son1);
  printf("---Deleted first soon\n");
  MsvgPrintElementTree(stdout, root, 0);

  //MsvgDeleteElement(son2);
  //printf("---Deleted second soon\n");
  //MsvgPrintElementTree(stdout, root, 0);

  MsvgDeleteElement(root);
  printf("---Deleted root\n");
  //MsvgPrintElementTree(stdout, root, 0);

  return 1;
}
