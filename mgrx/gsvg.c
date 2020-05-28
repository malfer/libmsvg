/* gsvg.c ---- test generated svg for MGRX
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

#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <mgrx.h>
#include <mgrxkeys.h>
#include <msvg.h>
#include "rendmgrx.h"

static MsvgElement *CreateTree(void)
{
    MsvgElement *root, *son, *gson;

    root = MsvgNewElement(EID_SVG, NULL);
    root->psvgattr->vb_min_x = 0;
    root->psvgattr->vb_min_y = 0;
    root->psvgattr->vb_width = 640;
    root->psvgattr->vb_height = 480;
    root->psvgattr->tree_type = COOKED_SVGTREE;

    son = MsvgNewElement(EID_DEFS, root);

    gson = MsvgNewElement(EID_RECT, son);
    gson->prectattr->width = 100;
    gson->prectattr->height = 50;
    gson->id = strdup("MyRect");

    son = MsvgNewElement(EID_RECT, root);
    son->prectattr->x = 50;
    son->prectattr->y = 50;
    son->prectattr->width = 300;
    son->prectattr->height = 300;
    son->pctx.fill = 0x0000FF;
    son->pctx.stroke = 0xFF0000;

    son = MsvgNewElement(EID_CIRCLE, root);
    son->pcircleattr->cx = 100;
    son->pcircleattr->cy = 100;
    son->pcircleattr->r = 80;
    son->pctx.fill = 0x888888;
    son->pctx.stroke = 0x00FF00;
    son->pctx.stroke_width = 5;

    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 400;
    son->puseattr->y = 200;
    son->puseattr->refel = strdup("MyRect");
    son->pctx.fill = 0xFFFF00;
    
    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 450;
    son->puseattr->y = 300;
    son->puseattr->refel = strdup("MyRect");
    son->pctx.fill = 0x008888;
    son->pctx.stroke = 0x00FFFF;
    TMSetRotation(&(son->pctx.tmatrix), 30, 500, 325);
    
    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 500;
    son->puseattr->y = 400;
    son->puseattr->refel = strdup("MyRect");
    son->pctx.fill = 0x888800;
    son->pctx.stroke = 0xFFFFFF;
    
    return root;
}

static void TestFunc(void)
{
    GrEvent ev;
    int smode;
    
    //GrClearContext(GrBlack());
    smode = SVGDRAWMODE_PAR | SVGDRAWADJ_CENTER;
    DrawSVGtree(CreateTree(), smode, 1, GrBlack());
    GrEventWaitKeyOrClick(&ev);
}

int main(int argc,char **argv)
{
    int yhelptext;
    
    GrContext *ctx;
    GrSetMode(GR_default_graphics);
    GrClearScreen(GrWhite());
    
    yhelptext = GrScreenY() - 60;
    GrTextXY(10, yhelptext+25,
             "Test a generated MsvgElement tree",
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
