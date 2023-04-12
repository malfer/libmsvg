/* svg2png.c ---- svg to png using GD and libmsvg
 * 
 * This is a dirty hack to test the libmsvg librarie with the GD
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * Copyright (C) 2022 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <gd.h>
#include <msvg.h>
#include "rendgd.h"

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
    GDSVGDrawMode sdm = {SVGDRAWMODE_PAR, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0, 0xFFFFFF};
    MsvgElement *root;
    int error;
    gdImagePtr im;
    FILE *pngout;
    int width = 0;
    int height = 0;

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
        } if (strcmp(argv[0], "-z") == 0) {
            if (argc > 1) {
                sdm.zoom = atof(argv[1]);
                argv += 1;
                argc -= 1;
            }
        } if (strcmp(argv[0], "-rt") == 0) {
            if (argc > 1) {
                sdm.rotang = atof(argv[1]);
                argv += 1;
                argc -= 1;
            }
        } if (strcmp(argv[0], "-dp") == 0) {
            if (argc > 2) {
                sdm.xdespl = atof(argv[1]);
                sdm.ydespl = atof(argv[2]);
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
        printf("Usage: svg2png [-f|p|s] [-l|c|r] [-gd width height] [-z zoom]\n");
        printf("               [-rt angle] [-dp xdespl ydespl] file.svg file.png\n");
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

    // Create the image and do the magic
    im = gdImageCreateTrueColor(width, height);
    if (im == NULL) {
        printf("Error creating GD image\n");
        return EXIT_FAILURE;
    }

    ReplaceTextByPaths(root);
    error = GDDrawSVGtree(root, &sdm, im);
    if (error) {
        printf("Error %d rendering %s\n", error, argv[0]);
        return EXIT_FAILURE;
    }

    // Save the rendering in a png file
    pngout = fopen(argv[1], "wb");
    if (pngout == NULL) {
        printf("Error creating %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    gdImagePng(im, pngout);
    fclose(pngout);

    // Finishing
    gdImageDestroy(im);
    MsvgDeleteElement(root);

    return EXIT_SUCCESS;
}
