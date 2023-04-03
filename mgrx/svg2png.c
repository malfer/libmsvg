/* svg2png.c ---- svg to png using MGRX and libmsvg
 * 
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2022-2023 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg+MGRX libraries.
 * libmsvg+MGRX test files are in the Public Domain, this apply only to test
 * files, the libmsvg library itself is under the terms of the Expat license
 * and the MGRX library under the LGPL license
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <mgrx.h>
#include <msvg.h>
#include "rendmgrx.h"

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

static void ReplaceTextByPaths(MsvgElement *root)
{
    MsvgTreeCounts tc;
    MsvgBFont *bfont;
    Repdata *rd;
    int nte, i;

    MsvgCalcCountsCookedTree(root, &tc);
    if (tc.nelem[EID_TEXT] < 0) return; // nothing to do

    if (tc.nelem[EID_FONT] > 0) { // we have one or more internal fonts, load them
        MsvgBFontLibLoad(root);
    }

    // now load three standard fonts sans, serif & mono
    MsvgBFontLibLoadFromFile("../gfonts/rsans.svg");
    MsvgBFontLibLoadFromFile("../gfonts/rserif.svg");
    MsvgBFontLibLoadFromFile("../gfonts/rmono.svg");

    // do the work
    nte = tc.nelem[EID_TEXT];
    rd = malloc(sizeof(Repdata)+sizeof(MsvgElement *)*nte);
    if (rd == NULL) { // opss no memory
        MsvgBFontLibFree();
        return;
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
                    if (MsvgReplaceElement(rd->el[i], group))
                    MsvgDeleteElement(rd->el[i]);
                }
            }
        }
    }

    MsvgBFontLibFree();
    free(rd);
}

int main(int argc,char **argv)
{
    GrSVGDrawMode sdm = {SVGDRAWMODE_PAR, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0, 0xFFFFFF};
    int width = 0;
    int height = 0;
    MsvgElement *root;
    int error;
    char drspec[81];

    // Get parameters
    if (argc > 0) {
        argv++;
        argc--;
    }

    while (argc > 0 && argv[0][0] == '-') {
        if (strcmp(argv[0], "-gd") == 0) {
            if (argc > 2) {
                width = atoi(argv[1]);
                height = atoi(argv[2]);
                argv += 2;
                argc -= 2;
            }
        } else if (strcmp(argv[0], "-f") == 0) {
            sdm.mode = SVGDRAWMODE_FIT;
        } else if (strcmp(argv[0], "-p") == 0) {
            sdm.mode = SVGDRAWMODE_PAR;
        } else if (strcmp(argv[0], "-s") == 0) {
            sdm.mode = SVGDRAWMODE_SCOORD;
        } else if (strcmp(argv[0], "-l") == 0) {
            sdm.adj = SVGDRAWADJ_LEFT;
        } else if (strcmp(argv[0], "-c") == 0) {
            sdm.adj = SVGDRAWADJ_CENTER;
        } else if (strcmp(argv[0], "-r") == 0) {
            sdm.adj = SVGDRAWADJ_RIGHT;
        }
        argv++;
        argc--;
    }

    if (argc < 2) {
        printf("Usage: svg2png [-f|p|s] [-l|c|r] [-gd width height] file.svg file.png\n");
        return EXIT_FAILURE;
    }

    // Load svg file and prepare it
    error = 0;
    root = MsvgReadSvgFile(argv[0], &error);
    if (root == NULL) {
        printf("Error %d opening %s\n", error, argv[0]);
        return EXIT_FAILURE;
    }
    if (MsvgRaw2CookedTree(root) != 1) {
        printf("Error cooking root element\n");
        return EXIT_FAILURE;
    }
    if (width == 0 || height == 0) {
        width = root->psvgattr->vb_width;
        height = root->psvgattr->vb_height;
    }

    // Create the memory framebuffer and do the magic
    sprintf(drspec, "memory gw %d gh %d nc 16M", width, height);
    printf("Set driver specs to \"%s\"\n", drspec);
    GrSetDriver( drspec );
    GrSetMode( GR_default_graphics );
    printf("FrameDriver: %s\n", GrFrameDriverName(GrScreenFrameMode()));

    ReplaceTextByPaths(root);
    MsvgNormalizeRawGradients(root);

    error = GrDrawSVGtree(root, &sdm);
    if (error) {
        printf("Error %d rendering %s\n", error, argv[0]);
        return EXIT_FAILURE;
    }

    // Save the framebuffer in a png file
    if (GrPngSupport() == 0 ) {
        printf("No PNG support in the local MGRX lib\n");
        return EXIT_FAILURE;
    }
    if (GrSaveContextToPng(NULL, argv[1]) != 0) {
        printf("Error creating %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Finishing
    MsvgDeleteElement(root);

    return EXIT_SUCCESS;
}
