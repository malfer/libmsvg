/* tcbuild.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg library.
 * libmsvg test files are in the Public Domain, this apply only to test
 * files, the library itself is under the terms of the Expat license
 *
 */

#include <stdio.h>
#include <string.h>
#include "msvg.h"

#define TESTFILE "msvgt3.svg"

int main(int argc, char **argv)
{
    MsvgElement *root, *son, *son2;
    double penta[] = {108,0, 216,62, 216,188, 108,250, 0,188, 0,62};
    TMatrix tm1, tm2;
    int i;
    
    root = MsvgNewElement(EID_SVG, NULL);
    root->psvgattr->vb_min_x = 0;
    root->psvgattr->vb_min_y = 0;
    root->psvgattr->vb_width = 400;
    root->psvgattr->vb_height = 400;
    root->psvgattr->tree_type = COOKED_SVGTREE;

    son = MsvgNewElement(EID_DEFS, root);

    son2 = MsvgNewElement(EID_RECT, son);
    son2->id = strdup("myrect");
    son2->prectattr->x = 0;
    son2->prectattr->y = 0;
    son2->prectattr->width = 100;
    son2->prectattr->height = 50;

    son = MsvgNewElement(EID_RECT, root);
    son->prectattr->x = 1;
    son->prectattr->y = 1;
    son->prectattr->width = 398;
    son->prectattr->height = 398;
    son->pctx.fill = 0XBBBBBB;
    son->pctx.stroke = 0XFF0000;

    son = MsvgNewElement(EID_POLYGON, root);
    MsvgAllocPointsToPolygonElement(son, 6);
    son->ppolygonattr->npoints = 6;
    for (i=0; i<6; i++) {
        son->ppolygonattr->points[i*2] = penta[i*2];
        son->ppolygonattr->points[i*2+1] = penta[i*2+1];
    }
    son->pctx.fill = 0X0000FF;
    son->pctx.stroke = 0X00FF00;
    son->pctx.stroke_width = 10;
    TMSetScaling(&tm1, 0.5, 0.5);
    TMSetTranslation(&tm2, 40, 30);
    TMMpy(&(son->pctx.tmatrix), &tm2, &tm1);

    son = MsvgNewElement(EID_POLYLINE, root);
    MsvgAllocPointsToPolylineElement(son, 6);
    son->ppolylineattr->npoints = 6;
    for (i=0; i<6; i++) {
        son->ppolylineattr->points[i*2] = penta[i*2];
        son->ppolylineattr->points[i*2+1] = penta[i*2+1];
    }
    son->pctx.fill = NO_COLOR;
    son->pctx.stroke = 0XFFFF00;
    son->pctx.stroke_width = 10;
    TMSetScaling(&tm1, 0.5, 0.5);
    TMSetTranslation(&tm2, 240, 30);
    TMMpy(&(son->pctx.tmatrix), &tm2, &tm1);
    
    son = MsvgNewElement(EID_G, root);
    son->pctx.fill = NO_COLOR;
    son->pctx.stroke = 0X00FF00;
    TMSetTranslation(&(son->pctx.tmatrix), 50, 50);

    son2 = MsvgNewElement(EID_CIRCLE, son);
    son2->pcircleattr->cx = 100;
    son2->pcircleattr->cy = 200;
    son2->pcircleattr->r = 80;

    son2 = MsvgNewElement(EID_CIRCLE, son);
    son2->pcircleattr->cx = 200;
    son2->pcircleattr->cy = 200;
    son2->pcircleattr->r = 80;
    
    son2 = MsvgNewElement(EID_ELLIPSE, son);
    son2->pellipseattr->cx = 150;
    son2->pellipseattr->cy = 200;
    son2->pellipseattr->rx_x = 270;
    son2->pellipseattr->rx_y = 200;
    son2->pellipseattr->ry_x = 150;
    son2->pellipseattr->ry_y = 260;
    son2->pctx.stroke = 0X0000FF;

    son2 = MsvgNewElement(EID_USE, son);
    son2->puseattr->refel = strdup("myrect");
    son2->puseattr->x = 100;
    son2->puseattr->y = 280;
    son2->pctx.fill = 0X880000;

    son = MsvgNewElement(EID_LINE, root);
    son->plineattr->x1 = 50;
    son->plineattr->y1 = 350;
    son->plineattr->x2 = 350;
    son->plineattr->y2 = 350;
    son->pctx.stroke = 0X000088;

    printf("===== Cooked to Raw =====\n");
    MsvgCooked2RawTree(root);
    MsvgPrintRawElementTree(stdout, root, 0);
    
    printf("===== Writing %s =====\n", TESTFILE);
    if (!MsvgWriteSvgFile(root, TESTFILE)) {
        printf("Error writing %s\n", TESTFILE);
        return 0;
    }
    
    return 1;
}
