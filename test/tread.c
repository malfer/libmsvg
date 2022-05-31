/* tread.c
 * 
 * libmsvg, a minimal library to read and write svg files
 * 
 * Copyright (C) 2010, 2020-2022 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg library.
 * libmsvg test files are in the Public Domain, this apply only to test
 * files, the library itself is under the terms of the Expat license
 *
 */

#include <stdio.h>
#include <string.h>
#include "msvg.h"

#define TESTFILE "msvgt1.svg"

int main(int argc, char **argv)
{
    MsvgElement *root, *el;
    MsvgTreeCounts tc;
    int eid, error;
    MsvgTableId *tid;
    int report = 0;
    char *sid = NULL;

    if (argc > 0) {
        argv++;
        argc--;
    }

    while (argc > 0 && argv[0][0] == '-') {
        if (strcmp(argv[0], "-r") == 0)
            report = 1;
        else if (strncmp(argv[0], "-id=", 4) == 0)
            sid = &(argv[0][4]);
        argv++;
        argc--;
    }

    if (argc < 1) {
        printf("Usage: tread [-r] [-id=id] file.svg\n");
        return 0;
    }

    printf("==== Reading %s\n", argv[0]);
    root = MsvgReadSvgFile2(argv[0], &error, (report ? stdout : NULL));
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    printf("==== Printing raw tree\n");
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

    if (!sid) return 1;

    printf("==== Finding %s in raw tree\n", sid);
    el = MsvgFindIdRawTree(root, sid);
    if (el) MsvgPrintRawElementTree(stdout, el, 0);

    printf("==== Building MsvgTableId for raw tree\n");
    tid = MsvgBuildTableIdRawTree(root);
    if (tid == NULL) return 0;

    printf("==== Finding %s in TableId\n", sid);
    el = MsvgFindIdTableId(tid, sid);
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

    printf("==== Finding %s in cooked tree\n", sid);
    el = MsvgFindIdCookedTree(root, sid);
    if (el) MsvgPrintCookedElement(stdout, el);

    printf("==== Building MsvgTableId for cooked tree\n");
    tid = MsvgBuildTableIdCookedTree(root);
    if (tid == NULL) return 0;

    printf("==== Finding %s in TableId\n", sid);
    el = MsvgFindIdTableId(tid, sid);
    if (el) MsvgPrintCookedElement(stdout, el);

    MsvgDestroyTableId(tid);

    return 1;
}
