/* rsvg.c ---- test read svg for MGRX
 * 
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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

#define TEST_FILE "../svgpics/test.svg"

static void TestFunc(void)
{
    GrEvent ev;
    MsvgElement *root;
    int error;
    GrSVGDrawMode sdm = {SVGDRAWMODE_FIT, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0, 0};
    
    //GrClearContext(GrBlack());
    root = MsvgReadSvgFile(TEST_FILE, &error);
    if (root == NULL) {
        printf("Error %d reading %s\n", error, TEST_FILE);
        return;
    }
    if (!MsvgRaw2CookedTree(root)) return;
    GrDrawSVGtree(root, &sdm);
    GrEventWaitKeyOrClick(&ev);
    MsvgDeleteElement(root);
}

int main(int argc,char **argv)
{
    int yhelptext;
    
    GrContext *ctx;
    //  GrSetMode(GR_default_graphics);
    GrSetMode(GR_width_height_bpp_graphics, 500, 1000, 24);
    GrClearScreen(GrWhite());
    
    yhelptext = GrScreenY() - 60;
    GrTextXY(10, yhelptext+25,
             "test.svg file renderized",
             GrBlack(), GrNOCOLOR);
    GrTextXY(10, yhelptext+40,
             "Press any key to finish",
             GrBlack(), GrNOCOLOR);
    
    ctx = GrCreateSubContext(10, 10, GrScreenX()-10, yhelptext, NULL, NULL);
    GrSetContext(ctx);
    GrEventInit();
    GrMouseDisplayCursor();
    setlocale(LC_NUMERIC, "C");
    TestFunc();
    GrEventUnInit();
    GrSetMode(GR_default_text);
    return(0);
}
