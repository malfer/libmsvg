/* tread.c
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

#define TESTFILE "msvgt1.svg"

int main(int argc, char **argv)
{
    MsvgElement *root, *el;
    MsvgTreeCounts tc;
    int eid, error;
    MsvgTableId *tid;
    
    if (argc <2) {
        printf("Usage: tread file [id]\n");
        return 0;
    }
    
    root = MsvgReadSvgFile(argv[1], &error);
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[1]);
        return 0;
    }
    
    MsvgPrintRawElementTree(stdout, root, 0);

    printf("==== Writing %s\n", TESTFILE);
    if (!MsvgWriteSvgFile(root, TESTFILE)) {
        printf("Error writing %s\n", TESTFILE);
        return 0;
    }

    printf("==== Tree counts raw tree\n");
    MsvgCalcCountsRawTree(root, &tc);
    for (eid=EID_SVG+1; eid <= EID_LAST; eid++) {
        printf("%-10s : %d\n", MsvgFindElementName(eid), tc.nelem[eid]);
    }
    printf("Total      : %d\n", tc.totelem);
    printf("With Id    : %d\n", tc.totelwid);

    if (argc < 3) return 1;

    printf("==== Find %s in raw tree\n", argv[2]);
    el = MsvgFindIdRawTree(root, argv[2]);
    if (el) MsvgPrintRawElementTree(stdout, el, 0);

    printf("==== Build MsvgTableId for raw tree\n");
    tid = MsvgBuildTableIdRawTree(root);
    if (tid == NULL) return 0;

    printf("==== Find %s in TableId\n", argv[2]);
    el = MsvgFindIdTableId(tid, argv[2]);
    if (el) MsvgPrintRawElementTree(stdout, el, 0);

    MsvgDestroyTableId(tid);

    printf("==== Tree counts cooked tree\n");
    MsvgRaw2CookedTree(root);
    MsvgCalcCountsCookedTree(root, &tc);
    for (eid=EID_SVG+1; eid <= EID_LAST; eid++) {
        printf("%-10s : %d\n", MsvgFindElementName(eid), tc.nelem[eid]);
    }
    printf("Total      : %d\n", tc.totelem);
    printf("With Id    : %d\n", tc.totelwid);

    printf("==== Find %s in cooked tree\n", argv[2]);
    el = MsvgFindIdCookedTree(root, argv[2]);
    if (el) MsvgPrintCookedElement(stdout, el);

    printf("==== Build MsvgTableId for cooked tree\n");
    tid = MsvgBuildTableIdCookedTree(root);
    if (tid == NULL) return 0;

    printf("==== Find %s in TableId\n", argv[2]);
    el = MsvgFindIdTableId(tid, argv[2]);
    if (el) MsvgPrintCookedElement(stdout, el);

    MsvgDestroyTableId(tid);

    return 1;
}
