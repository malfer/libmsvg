/* tdel.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
    MsvgAddRawAttribute(root, "xmlns", "http://www.w3.org/2000/svg");
    MsvgAddRawAttribute(root, "version", "1.2");
    MsvgAddRawAttribute(root, "baseProfile", "tiny");
    MsvgAddRawAttribute(root, "viewBox", "0 0 400 400");
    
    son1 = MsvgNewElement(EID_RECT, root);
    MsvgAddRawAttribute(son1, "id", "son1");
    MsvgAddRawAttribute(son1, "x", "1");
    MsvgAddRawAttribute(son1, "y", "1");
    MsvgAddRawAttribute(son1, "width", "398");
    MsvgAddRawAttribute(son1, "height", "398");
    MsvgAddRawAttribute(son1, "stroke", "#F00");
    MsvgAddRawAttribute(son1, "fill", "#FFF");
    
    son2 = MsvgNewElement(EID_RECT, root);
    MsvgAddRawAttribute(son2, "x", "11");
    MsvgAddRawAttribute(son2, "y", "11");
    MsvgAddRawAttribute(son2, "width", "380");
    MsvgAddRawAttribute(son2, "height", "380");
    MsvgAddRawAttribute(son2, "stroke", "#0F0");
    MsvgAddRawAttribute(son2, "fill", "none");
    
    son3 = MsvgNewElement(EID_POLYLINE, root);
    MsvgAddRawAttribute(son3, "stroke", "#0F0");
    MsvgAddRawAttribute(son3, "stroke-width", "10");
    MsvgAddRawAttribute(son3, "points", "100,360 200,320 300,360 300,320");
    
    printf("===== Original tree\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDelRawAttribute(son1, "width");
    printf("===== Deleted \"width\" attribute on son1\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDelAllRawAttributes(son1);
    printf("===== Deleted all attributes on son1\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDelRawAttribute(son2, "fill");
    printf("===== Deleted \"fill\" attribute on son2\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDelRawAttribute(son2, "x");
    printf("===== Deleted \"x\" attribute on son2\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDelRawAttribute(root, "baseProfile");
    printf("===== Deleted \"baseProfile\" attribute on root\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgPruneElement(son2);
    printf("===== Prune second soon\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgInsertSonElement(son2, root);
    printf("===== Insert second soon at the end\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDeleteElement(son1);
    printf("===== Deleted first soon\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    //MsvgDeleteElement(son2);
    //printf("===== Deleted second soon\n");
    //MsvgPrintRawElementTree(stdout, root, 0);
    
    MsvgDeleteElement(root);
    printf("===== Deleted root\n");
    //MsvgPrintRawElementTree(stdout, root, 0);
    
    return 1;
}
