/* tcook.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "msvg.h"

static int usetranscooked = 0;

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx)
{
    MsvgElement *newel;

    if (usetranscooked) {
        newel = MsvgTransformCookedElement(el, pctx);
        if (newel == NULL) return;
    } else {
        newel = el;
    }

    printf("=========\n");
    MsvgPrintCookedElement(stdout, newel);
    printf("  --------- effective MsvgPaintCtx\n");
    MsvgPrintPctx(stdout, pctx);

    if (usetranscooked)
        free(newel);
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    int error;
    
    if (argc <2) {
        printf("Usage: tcook file [utc]\n");
        return 0;
    }
    
    if (argc >= 3 && strcmp(argv[2], "utc") == 0) usetranscooked = 1;
    
    root = MsvgReadSvgFile(argv[1], &error);

    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[1]);
        return 0;
    }
    
    MsvgPrintRawElementTree(stdout, root, 0);

    printf("===== Converting to cooked tree\n");
    MsvgRaw2CookedTree(root);
    MsvgPrintCookedElement(stdout, root);

    printf("===== Deleting all raw parameters\n");
    MsvgDelAllTreeRawAttributes(root);
    MsvgPrintRawElementTree(stdout, root, 0);

    printf("===== Serialize cooked tree\n");
    MsvgSerCookedTree(root, sufn);

    MsvgDeleteElement(root);

    return 1;
}
