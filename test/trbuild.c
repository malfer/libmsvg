/* trbuild.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg library.
 * libmsvg test files are in the Public Domain, this apply only to test
 * files, the library itself is under the terms of the Expat license
 *
 */

#include <stdio.h>
#include "msvg.h"

#define TESTFILEA "msvgt2a.svg"
#define TESTFILEB "msvgt2b.svg"

int main(int argc, char **argv)
{
    MsvgElement *root;
    root = MsvgNewElement(EID_SVG, NULL);
    MsvgAddRawAttribute(root, "version", "1.2");
    MsvgAddRawAttribute(root, "baseProfile", "tiny");
    MsvgAddRawAttribute(root, "xmlns", "http://www.w3.org/2000/svg");
    MsvgAddRawAttribute(root, "xmlns:xlink", "http://www.w3.org/1999/xlink");
    MsvgAddRawAttribute(root, "viewBox", "0 0 400 400");
    
    MsvgElement *son;
    son = MsvgNewElement(EID_RECT, root);
    MsvgAddRawAttribute(son, "x", "1");
    MsvgAddRawAttribute(son, "y", "1");
    MsvgAddRawAttribute(son, "width", "398");
    MsvgAddRawAttribute(son, "height", "398");
    MsvgAddRawAttribute(son, "stroke", "#F00");
    MsvgAddRawAttribute(son, "fill", "#FFF");
    son = MsvgNewElement(EID_G, root);
    MsvgAddRawAttribute(son, "stroke", "#0F0");
    MsvgAddRawAttribute(son, "fill", "none");
    MsvgAddRawAttribute(son, "transform", "translate(50)");

    MsvgElement *son2;
    son2 = MsvgNewElement(EID_CIRCLE, son);
    MsvgAddRawAttribute(son2, "id", "myrect");
    MsvgAddRawAttribute(son2, "cx", "100");
    MsvgAddRawAttribute(son2, "cy", "200");
    MsvgAddRawAttribute(son2, "r", "80");
    son2 = MsvgNewElement(EID_USE, son);
    MsvgAddRawAttribute(son2, "xlink:href", "#myrect");
    MsvgAddRawAttribute(son2, "x", "100");
    
    printf("===== Constructed svgtree =====\n");
    MsvgPrintRawElementTree(stdout, root, 0);
    
    printf("===== Writing svgtree on %s =====\n", TESTFILEA);
    if (!MsvgWriteSvgFile(root, TESTFILEA)) {
        printf("Error writing %s\n", TESTFILEA);
        return 0;
    }
    
    printf("===== Duplicate svgtree =====\n");
    MsvgElement *rootdup;
    rootdup = MsvgDupElement(root);
    MsvgPrintRawElementTree(stdout, rootdup, 0);
    
    printf("===== Writing duplicate svgtree on %s =====\n", TESTFILEB);
    if (!MsvgWriteSvgFile(rootdup, TESTFILEB)) {
        printf("Error writing %s\n", TESTFILEB);
        return 0;
    }
    
    return 1;
}
