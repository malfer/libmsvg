/* tbpsrv.c
 * 
 * libmsvg, a minimal library to read and write svg files
 * 
 * Copyright (C) 2023 Mariano Alvarez Fernandez
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

static void wufn(MsvgElement *el, void *udata)
{
    MsvgBPServer *bps;

    if (el->eid != EID_LINEARGRADIENT &&
        el->eid != EID_RADIALGRADIENT) return;

    printf("Gradient %s, generating binary paint server: ", el->id);

    bps = MsvgNewBPServer(el);
    if (bps) {
        if (bps->type == BPSERVER_LINEARGRADIENT) {
            printf("ok, linear, %d stops\n", bps->blg.stops.nstops);
        } else {
            printf("ok, radial, %d stops\n", bps->brg.stops.nstops);
        }
        MsvgDestroyBPServer(bps);
    } else {
        printf("failed\n");
    }
}

int main(int argc, char **argv)
{
    MsvgElement *root;
    int normalizegradients = 0;
    int error;
    int n;

    if (argc > 0) {
        argv++;
        argc--;
    }

    while (argc > 0 && argv[0][0] == '-') {
        if (strcmp(argv[0], "-ng") == 0)
            normalizegradients = 1;
        argv++;
        argc--;
    }

    if (argc < 1) {
        printf("Usage: tbpsrv [-ng] file\n");
        return 0;
    }

    root = MsvgReadSvgFile(argv[0], &error);

    if (root == NULL) {
        printf("Error %d reading %s\n", error, argv[0]);
        return 0;
    }

    if (normalizegradients) {
        printf("===== Normalize gradients to SVG Tiny 1.2\n");
        n = MsvgNormalizeRawGradients(root);
        printf("%d gradients normalized\n", n);
    }

    printf("===== Converting to cooked tree\n");
    MsvgRaw2CookedTree(root);

    printf("===== Walking the tree searching for gradients\n");
    MsvgWalkTree(root, wufn, NULL);

    MsvgDeleteElement(root);

    return 1;
}
