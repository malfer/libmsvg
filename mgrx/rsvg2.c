/* rsvg2.c ---- test read svg for MGRX
 *
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include <mgrxkeys.h>
#include <msvg.h>
#include "rendmgrx.h"

/* default mode */

static int gwidth = 1024;
static int gheight = 728;
static int gbpp = 24;

static int DrawSvgFile(char *fname, int smode, double zoom, int rotang, GrColor bg)
{
    MsvgElement *root;
    char s[81];
    int error = 0;
    
    root = MsvgReadSvgFile(fname, &error);
    if (root == NULL) return error;

    if (rotang != 0) {
        sprintf(s, "rotate(%d %d %d)", rotang, 250, 500);
        MsvgAddRawAttribute(root, "transform", s);
    }

    if (!MsvgRaw2CookedTree(root)) return -5;

    //DrawSVGtree(root, smode, zoom, bg);
    DrawSVGtreeUsingDB(root, smode, zoom, bg);
    MsvgDeleteElement(root);

    return 0;
}

int main(int argc,char **argv)
{
    GrEvent ev;
    char *fname;
    int yhelptext;
    int mode, adj;
    char s[121];
    double zoom = 1;
    GrColor bg;
    int rotang = 0;
    int error;
    
    if (argc <2) {
        printf("Usage: rsvg2 file.svg [width height bpp]\n");
        return 0;
    }

    fname = argv[1];

    if (argc >= 5) {
        gwidth = atoi(argv[2]);
        gheight = atoi(argv[3]);
        gbpp = atoi(argv[4]);
    }
    
    GrContext *ctx;
    GrSetMode(GR_width_height_bpp_graphics, gwidth, gheight, gbpp);
    GrClearScreen(GrWhite());
    
    yhelptext = GrScreenY() - 60;
    sprintf(s, "%s file renderized", fname);
    GrTextXY(10, yhelptext+25, s, GrBlack(), GrNOCOLOR);
    GrTextXY(10, yhelptext+40,
             "mode: [f] [p] [s]  adj: [l] [c] [r]  zoom: [+] [-]  "
             "rotate: [<] [>]  color: [b] [w]  quit: [q]",
             GrBlack(), GrNOCOLOR);
    
    ctx = GrCreateSubContext(10, 10, GrScreenX()-10, yhelptext, NULL, NULL);
    GrSetContext(ctx);
    GrEventInit();
    GrMouseDisplayCursor();

    setlocale(LC_NUMERIC, "C");
    bg = GrBlack();

    mode = SVGDRAWMODE_PAR;
    adj = SVGDRAWADJ_LEFT;
    while (1) {
        error = DrawSvgFile(fname, mode|adj, zoom, rotang, bg);
        if (error) {
            printf("Error %d reading %s\n", error, fname);
            break;
        }
        GrEventWait(&ev);
        if (ev.p1 == 'q') break;
        else if (ev.p1 == 'b') bg = GrBlack();
        else if (ev.p1 == 'w') bg = GrWhite();
        else if (ev.p1 == 'f') mode = SVGDRAWMODE_FIT;
        else if (ev.p1 == 'p') mode = SVGDRAWMODE_PAR;
        else if (ev.p1 == 's') mode = SVGDRAWMODE_SCOORD;
        else if (ev.p1 == 'l') adj = SVGDRAWADJ_LEFT;
        else if (ev.p1 == 'c') adj = SVGDRAWADJ_CENTER;
        else if (ev.p1 == 'r') adj = SVGDRAWADJ_RIGHT;
        else if (ev.p1 == '+') zoom = zoom * 2;
        else if (ev.p1 == '-') zoom = zoom / 2;
        else if (ev.p1 == '>') rotang++;
        else if (ev.p1 == '<') rotang--;
    }
    
    GrEventUnInit();
    GrSetMode(GR_default_text);

    return 1;
}
