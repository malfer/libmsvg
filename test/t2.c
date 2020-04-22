/* t2.c
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

#define TESTFILEA "msvgt2a.svg"
#define TESTFILEB "msvgt2b.svg"

int main(int argc, char **argv)
{
    MsvgElement *root, *son, *rootdup;
    
    root = MsvgNewElement(EID_SVG, NULL);
    MsvgAddRawAttribute(root, "xmlns", "http://www.w3.org/2000/svg");
    MsvgAddRawAttribute(root, "version", "1.2");
    MsvgAddRawAttribute(root, "baseProfile", "tiny");
    MsvgAddRawAttribute(root, "viewBox", "0 0 400 400");
    
    son = MsvgNewElement(EID_RECT, root);
    MsvgAddRawAttribute(son, "x", "1");
    MsvgAddRawAttribute(son, "y", "1");
    MsvgAddRawAttribute(son, "width", "398");
    MsvgAddRawAttribute(son, "height", "398");
    MsvgAddRawAttribute(son, "stroke", "#F00");
    MsvgAddRawAttribute(son, "fill", "#FFF");
    
    son = MsvgNewElement(EID_RECT, root);
    MsvgAddRawAttribute(son, "x", "11");
    MsvgAddRawAttribute(son, "y", "11");
    MsvgAddRawAttribute(son, "width", "380");
    MsvgAddRawAttribute(son, "height", "380");
    MsvgAddRawAttribute(son, "stroke", "#0F0");
    MsvgAddRawAttribute(son, "fill", "none");
    
    printf("===== Constructed svgtree =====\n");
    MsvgPrintElementTree(stdout, root, 0);
    
    if (!MsvgWriteSvgFile(root, TESTFILEA)) {
        printf("Error writing %s\n", TESTFILEA);
        return 0;
    }
    
    rootdup = MsvgDupElement(root);
    
    printf("===== Duplicate svgtree =====\n");
    MsvgPrintElementTree(stdout, rootdup, 0);
    
    if (!MsvgWriteSvgFile(rootdup, TESTFILEB)) {
        printf("Error writing %s\n", TESTFILEB);
        return 0;
    }
    
    return 1;
}
