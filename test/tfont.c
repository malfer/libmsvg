/* tfont.c
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

#define TESTFILE "msvgt5.svg"

typedef struct _Repdata {
    int nte;            // num of EID_TEXT elements
    int max;            // max EID_TEXT elements
    MsvgElement *el[1]; // elements (not actual size)
} Repdata;

static void reptext(MsvgElement *el, void *udata)
{
    Repdata *rd;

    rd = (Repdata *)udata;

    if (el->eid == EID_TEXT) {
        if (rd->nte < rd->max) {
            rd->el[rd->nte] = el;
            rd->nte++;
        }
    }
}

static int ReplaceTextByPaths(MsvgElement *root)
{
    MsvgTreeCounts tc;
    MsvgBFont *bfont;
    Repdata *rd;
    int nte, i;
    int nr = 0;

    MsvgCalcCountsCookedTree(root, &tc);
    if (tc.nelem[EID_TEXT] < 0) return 0; // nothing to do

    nte = tc.nelem[EID_TEXT];
    rd = malloc(sizeof(Repdata)+sizeof(MsvgElement *)*nte);
    if (rd == NULL) { // opss no memory
        MsvgBFontLibFree();
        return 0;
    }
    rd->nte = 0;
    rd->max = nte;

    MsvgWalkTree(root, reptext, rd);

    for(i=0; i<rd->nte; i++) {
        MsvgElement *group;

        if (rd->el[i]->eid == EID_TEXT) {
            bfont = MsvgBFontLibFind(rd->el[i]->pctx->sfont_family,
                                     rd->el[i]->pctx->ifont_family);
            if (bfont != NULL) {
                group = MsvgTextToPathGroup(rd->el[i], bfont);
                if (group) {
                    if (MsvgReplaceElement(rd->el[i], group)) {
                        MsvgDeleteElement(rd->el[i]);
                        nr++;
                    }
                }
            }
        }
    }

    free(rd);
    return nr;
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    int error, ind, ind2, nr;
    int loadbfontlib = 1;

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
        printf("Usage: tfont file.svg [font.svg]\n");
        return 0;
    }

    if (argc > 1) {
        printf("==== Reading svg font file %s\n", argv[1]);
        root = MsvgReadSvgFile(argv[1], &error);
    
        if (root == NULL) {
            printf("Error %d reading %s\n", error, argv[1]);
            return 0;
        }

        printf("==== Finding svg fonts in %s... ", argv[1]);
        MsvgRaw2CookedTree(root);
        ind = MsvgBFontLibLoad(root);
        MsvgDeleteElement(root);
        if (ind > 0) {
            printf("Found!!\n");
        } else {
            printf("Not found :-(\n");
            return 0;
        }

        MsvgBFontLibSetDefaultBfont(0);
        loadbfontlib = 0;
    }

    printf("==== Reading svg file %s\n", argv[0]);
    root = MsvgReadSvgFile(argv[0], &error);
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    MsvgRaw2CookedTree(root);

    if (loadbfontlib) {
        printf("==== Loading fonts\n");
        ind = MsvgBFontLibLoad(root);
        if (ind) printf("%d internal fonts loaded\n", ind);
        ind2 = MsvgBFontLibLoadFromFile("../gfonts/rsans.svg");
        ind2 += MsvgBFontLibLoadFromFile("../gfonts/rserif.svg");
        ind2 += MsvgBFontLibLoadFromFile("../gfonts/rmono.svg");
        if (ind2) printf("%d external fonts loaded\n", ind2);
    }

    printf("==== Replacing text elements by paths\n");
    nr = ReplaceTextByPaths(root);
    printf("%d EID_TEXT elements replaced\n", nr);

    printf("==== Cooked to Raw\n");
    MsvgDelAllTreeRawAttributes(root);
    MsvgCooked2RawTree(root);
    printf("==== Writing %s\n", TESTFILE);
    if (!MsvgWriteSvgFile(root, TESTFILE)) {
        printf("Error writing %s\n", TESTFILE);
    }

    MsvgDeleteElement(root);
    MsvgBFontLibFree();

    return 1;
}
