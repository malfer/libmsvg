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

#include <stdio.h>
#include "msvg.h"

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx)
{
    printf("=========\n");
    MsvgPrintCookedElement(stdout, el);
    printf("---------\n");
    MsvgPrintPctx(stdout, pctx);
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    
    if (argc <2) {
        printf("Usage: tcook file\n");
        return 0;
    }
    
    root = MsvgReadSvgFile(argv[1]);
    
    if (root == NULL) {
        printf("Error opening %s\n", argv[1]);
        return 0;
    }
    
    MsvgPrintElementTree(stdout, root, 0);

    printf("===== Converting to cooked tree\n");
    MsvgRaw2CookedTree(root);

    printf("===== Serialize cooked tree\n");
    MsvgSerCookedTree(root, sufn);
    
    return 1;
}
