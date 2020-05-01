/* rsvg.c ---- test read svg for MGRX
 *
 * These test programs are a dirty hack to test the libmsvg librarie with the
 * mgrx graphics library. It is NOT part of the librarie really.
 *
 * In the future this will be added to MGRX
 *
 * libmsvg, a minimal library to read and write svg files
 * Copyright (C) 2010 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

static void TestFunc(char *fname, int par, double zoom)
{
    MsvgElement *root;
    
    root = MsvgReadSvgFile(fname);
    if (root == NULL) return;
    if (!MsvgRaw2CookedTree(root)) return;
    DrawSVGtree(root, par, zoom);
    MsvgDeleteElement(root);
}

int main(int argc,char **argv)
{
    GrEvent ev;
    char *fname;
    int yhelptext;
    int par;
    char s[121];
    double zoom = 1;
    GrColor bg;
    
    if (argc <2) {
        printf("Usage: rsvg file.svg [width height bpp]\n");
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
             "[p] [b] [w] [+] [-], [q] to quit",
             GrBlack(), GrNOCOLOR);
    
    ctx = GrCreateSubContext(10, 10, GrScreenX()-10, yhelptext, NULL, NULL);
    GrSetContext(ctx);
    GrEventInit();
    GrMouseDisplayCursor();

    setlocale(LC_NUMERIC, "C");
    bg = GrBlack();

    par = 1;
    while (1) {
        GrClearContext(bg);
        TestFunc(fname, par, zoom);
        GrEventWait(&ev);
        if (ev.p1 == 'q') break;
        if (ev.p1 == 'b') bg = GrBlack();
        if (ev.p1 == 'w') bg = GrWhite();
        if (ev.p1 == 'p') par = !par;
        if (ev.p1 == '+') zoom = zoom / 2;
        if (ev.p1 == '-') zoom = zoom * 2;
    }
    
    GrEventUnInit();
    GrSetMode(GR_default_text);

    return(0);
}
