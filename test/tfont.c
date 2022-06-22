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

static MsvgBFont *bfont = NULL;

static void findfont(MsvgElement *el, void *udata)
{
    if (bfont) return;

    if (el->eid == EID_FONT) {
        bfont = MsvgNewBFont(el);
    }
}

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

static void replacetextbypath(MsvgElement *root)
{
    MsvgTreeCounts tc;
    Repdata *rd;
    int nte, i;

    MsvgCalcCountsCookedTree(root, &tc);
    nte = tc.nelem[EID_TEXT];
    if (nte < 1) return;

    rd = malloc(sizeof(Repdata)+sizeof(MsvgElement *)*nte);
    if (rd == NULL) return;
    rd->nte = 0;
    rd->max = nte;

    MsvgWalkTree(root, reptext, rd);

    for(i=0; i<rd->nte; i++) {
        MsvgElement *group;

        if (rd->el[i]->eid == EID_TEXT) {
            group = MsvgTextToPathGroup(rd->el[i], bfont);
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
        printf("Usage: tfont font.svg file.svg\n");
        return 0;
    }

    printf("==== Reading svg font file %s\n", argv[0]);
    root = MsvgReadSvgFile(argv[0], &error);
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    MsvgRaw2CookedTree(root);

    printf("==== Finding svg font... ");
    MsvgWalkTree(root, findfont, NULL);
    MsvgDeleteElement(root);
    if (bfont) {
        printf("Found!!\n");
    } else {
        printf("Not found :-(\n");
        return 0;
    }

    printf("==== Reading svg file %s\n", argv[1]);
    root = MsvgReadSvgFile(argv[1], &error);
    
    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[1]);
        return 0;
    }

    MsvgRaw2CookedTree(root);
 
    printf("==== Replacing text elements by paths\n");
    replacetextbypath(root);

    printf("==== Cooked to Raw\n");
    MsvgDelAllTreeRawAttributes(root);
    MsvgCooked2RawTree(root);
    printf("==== Writing %s\n", TESTFILE);
    if (!MsvgWriteSvgFile(root, TESTFILE)) {
        printf("Error writing %s\n", TESTFILE);
    }

    MsvgDeleteElement(root);
    MsvgDestroyBFont(bfont);

    return 1;
}
