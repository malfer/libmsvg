/* tpa2poly.c
 * 
 * libmsvg, a minimal library to read and write svg files
 * 
 * Copyright (C) 2022 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg library.
 * libmsvg test files are in the Public Domain, this apply only to test
 * files, the library itself is under the terms of the Expat license
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "msvg.h"

#define TESTFILE "msvgt6.svg"

typedef struct _Repdata {
    int npe;            // num of EID_PATH elements
    int max;            // max EID_PATH elements
    MsvgElement *el[1]; // elements (not actual size)
} Repdata;

static void reppath(MsvgElement *el, void *udata)
{
    Repdata *rd;

    rd = (Repdata *)udata;

    if (el->eid == EID_PATH) {
        if (rd->npe < rd->max) {
            rd->el[rd->npe] = el;
            rd->npe++;
        }
    }
}

static void replacepathbypoly(MsvgElement *root)
{
    MsvgTreeCounts tc;
    Repdata *rd;
    int npe, i;

    MsvgCalcCountsCookedTree(root, &tc);
    npe = tc.nelem[EID_PATH];
    if (npe < 1) return;

    rd = malloc(sizeof(Repdata)+sizeof(MsvgElement *)*npe);
    if (rd == NULL) return;
    rd->npe = 0;
    rd->max = npe;

    MsvgWalkTree(root, reppath, rd);

    for(i=0; i<rd->npe; i++) {
        MsvgElement *group;

        if (rd->el[i]->eid == EID_PATH) {
            group = MsvgPathToPolyGroup(rd->el[i], 10);
            if (group) {
                if (MsvgReplaceElement(rd->el[i], group))
                    MsvgDeleteElement(rd->el[i]);
            }
        }
    }

    free(rd);
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    int error;

    if (argc > 0) {
        argv++;
        argc--;
    }

/*    while (argc > 0 && argv[0][0] == '-') {
        if (strcmp(argv[0], "-r") == 0)
            report = 1;
        else if (strncmp(argv[0], "-id=", 4) == 0)
            sid = &(argv[0][4]);
        argv++;
        argc--;
    }*/

    if (argc < 1) {
        printf("Usage: tpa2poly file.svg\n");
        return 0;
    }

    printf("==== Reading svg file %s\n", argv[0]);
    root = MsvgReadSvgFile(argv[0], &error);
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    MsvgRaw2CookedTree(root);

    printf("==== Replacing path elements by poly\n");
    replacepathbypoly(root);

    printf("==== Cooked to Raw\n");
    MsvgDelAllTreeRawAttributes(root);
    MsvgCooked2RawTree(root);
    printf("==== Writing %s\n", TESTFILE);
    if (!MsvgWriteSvgFile(root, TESTFILE)) {
        printf("Error writing %s\n", TESTFILE);
    }

    MsvgDeleteElement(root);

    return 1;
}
