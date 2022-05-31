/* tcook.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "msvg.h"

typedef struct {
    int usetranscooked;
} UserData;

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;
    UserData *ud;

    ud = (UserData *)udata;
    
    if (ud->usetranscooked) {
        newel = MsvgTransformCookedElement(el, pctx);
        if (newel == NULL) return;
    } else {
        newel = el;
    }

    printf("=========\n");
    MsvgPrintCookedElement(stdout, newel);
    if (!ud->usetranscooked) {
        printf("  --------- effective MsvgPaintCtx\n");
        MsvgPrintPctx(stdout, pctx);
    }

    if (ud->usetranscooked)
        MsvgDeleteElement(newel);
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    int error;
    UserData ud = {0};
    int normalizegradients = 0;

    if (argc > 0) {
        argv++;
        argc--;
    }

    while (argc > 0 && argv[0][0] == '-') {
        if (strcmp(argv[0], "-utc") == 0)
            ud.usetranscooked = 1;
        else if (strcmp(argv[0], "-ng") == 0)
            normalizegradients = 1;
        argv++;
        argc--;
    }

    if (argc < 1) {
        printf("Usage: tcook [-utc] [-ng] file\n");
        return 0;
    }

    root = MsvgReadSvgFile(argv[0], &error);

    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    if (normalizegradients) {
        printf("===== Normalize gradients to SVG Tiny 1.2\n");
        MsvgNormalizeRawGradients(root);
    }

    MsvgPrintRawElementTree(stdout, root, 0);

    printf("===== Converting to cooked tree\n");
    MsvgRaw2CookedTree(root);
    MsvgPrintCookedElement(stdout, root);

    printf("===== Deleting all raw parameters\n");
    MsvgDelAllTreeRawAttributes(root);
    MsvgPrintRawElementTree(stdout, root, 0);

    printf("===== Serialize cooked tree\n");
    if (ud.usetranscooked)
        printf("===== transforming elements\n");
    MsvgSerCookedTree(root, sufn, &ud);

    MsvgDeleteElement(root);

    return 1;
}
