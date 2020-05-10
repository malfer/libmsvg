/* printree.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include "msvg.h"

static void printRawAttribute(FILE *f, MsvgRawAttribute *el)
{
    if (el == NULL) return;
    
    fprintf(f, " (%s = %s)", el->key, el->value);
    
    printRawAttribute(f, el->nrattr);
}

static void printContents(FILE *f, MsvgElement *el, int depth)
{
    MsvgContent *cnt;
     int i;
   
    cnt = el->fcontent;
    while (cnt) {
        if (depth > 0) {
            for (i=0; i<depth; i++)
                fputs("  |", f);
            fputs("   ", f);
        }
        fprintf(f, "(%d) %s\n", cnt->len, cnt->s);
        cnt = cnt->ncontent;
    }
}

void MsvgPrintRawElementTree(FILE *f, MsvgElement *el, int depth)
{
    int i;
    
    if (el == NULL) return;
    
    if (depth > 0) {
        for (i=0; i<depth; i++)
            fputs("  |", f);
        fputs("-->", f);
    }
    
    fprintf(f, "%s", MsvgFindElementName(el->eid));
    printRawAttribute(f, el->frattr);
    fputs("\n", f);
    printContents(f, el, depth);
    
    MsvgPrintRawElementTree(f, el->fson, depth+1);
    
    MsvgPrintRawElementTree(f, el->nsibling, depth);
}

static char * printcolor(rgbcolor color)
{
    static char s[81];

    if (color == NO_COLOR)
        sprintf(s, "NO_COLOR");
    else if (color == INHERIT_COLOR)
        sprintf(s, "INHERIT_COLOR");
    else if (color == NODEFINED_COLOR)
        sprintf(s, "NODEFINED_COLOR");
    else
        sprintf(s, "#%06x", color);
    
    return s;
}

static char * printvalue(double value)
{
    static char s[81];

    if (value == INHERIT_VALUE)
        sprintf(s, "INHERIT_VALUE");
    else if (value == NODEFINED_VALUE)
        sprintf(s, "NODEFINED_VALUE");
    else
        sprintf(s, "%g", value);

    return s;
}

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx)
{
    fprintf(f, "  fill           %s\n", printcolor(pctx->fill));
    fprintf(f, "  fill_opacity   %s\n", printvalue(pctx->fill_opacity));
    fprintf(f, "  stroke         %s\n", printcolor(pctx->stroke));
    fprintf(f, "  stroke_width   %s\n", printvalue(pctx->stroke_width));
    fprintf(f, "  stroke_opacity %s\n", printvalue(pctx->stroke_opacity));
    fprintf(f, "  tmatrix        (%g %g %g %g %g %g)\n",
            pctx->tmatrix.a, pctx->tmatrix.b, pctx->tmatrix.c,
            pctx->tmatrix.d, pctx->tmatrix.e, pctx->tmatrix.f);
}

static void printSvgCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  width          %g\n", el->psvgattr->width);
    fprintf(f, "  height         %g\n", el->psvgattr->height);
    fprintf(f, "  vb_min_x       %g\n", el->psvgattr->vb_min_x);
    fprintf(f, "  vb_min_y       %g\n", el->psvgattr->vb_min_y);
    fprintf(f, "  vb_width       %g\n", el->psvgattr->vb_width);
    fprintf(f, "  vb_height      %g\n", el->psvgattr->vb_height);
    fprintf(f, "  vp_fill         %s\n", printcolor(el->psvgattr->vp_fill));
    fprintf(f, "  vp_fill_opacity %s\n", printvalue(el->psvgattr->vp_fill_opacity));
}

static void printGCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printRectCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %g\n", el->prectattr->x);
    fprintf(f, "  y              %g\n", el->prectattr->y);
    fprintf(f, "  width          %g\n", el->prectattr->width);
    fprintf(f, "  height         %g\n", el->prectattr->height);
    fprintf(f, "  rx             %g\n", el->prectattr->rx);
    fprintf(f, "  ry             %g\n", el->prectattr->ry);
}

static void printCircleCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  cx             %g\n", el->pcircleattr->cx);
    fprintf(f, "  cy             %g\n", el->pcircleattr->cy);
    fprintf(f, "  r              %g\n", el->pcircleattr->r);
}

static void printEllipseCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  cx             %g\n", el->pellipseattr->cx);
    fprintf(f, "  cy             %g\n", el->pellipseattr->cy);
    fprintf(f, "  rx_x           %g\n", el->pellipseattr->rx_x);
    fprintf(f, "  rx_y           %g\n", el->pellipseattr->rx_y);
    fprintf(f, "  ry_x           %g\n", el->pellipseattr->ry_x);
    fprintf(f, "  ry_y           %g\n", el->pellipseattr->ry_y);
}

static void printLineCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x1             %g\n", el->plineattr->x1);
    fprintf(f, "  y1             %g\n", el->plineattr->y1);
    fprintf(f, "  x2             %g\n", el->plineattr->x2);
    fprintf(f, "  y3             %g\n", el->plineattr->y2);
}

static void printPolylineCookedAttr(FILE *f, MsvgElement *el)
{
    int i;

    fprintf(f, "  npoints        %d\n", el->ppolylineattr->npoints);
    fprintf(f, "  points        ");
    for (i=0; i<el->ppolylineattr->npoints; i++) {
        fprintf(f, " (%g %g)", el->ppolylineattr->points[i*2],
                el->ppolylineattr->points[i*2+1]);
    }
    fprintf(f, "\n");
}

static void printPolygonCookedAttr(FILE *f, MsvgElement *el)
{
    int i;

    fprintf(f, "  npoints        %d\n", el->ppolygonattr->npoints);
    fprintf(f, "  points        ");
    for (i=0; i<el->ppolygonattr->npoints; i++) {
        fprintf(f, " (%g %g)", el->ppolygonattr->points[i*2],
                el->ppolygonattr->points[i*2+1]);
    }
    fprintf(f, "\n");
}

static void printTextCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %g\n", el->ptextattr->x);
    fprintf(f, "  y              %g\n", el->ptextattr->y);
    fprintf(f, "  font_size      %g\n", el->ptextattr->font_size);
    fprintf(f, "  font_family    %s\n", el->ptextattr->font_family);
}

static void printDefsCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printUseCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %g\n", el->puseattr->x);
    fprintf(f, "  y              %g\n", el->puseattr->y);
}

void MsvgPrintCookedElement(FILE *f, MsvgElement *el)
{
    fprintf(f, "%s", MsvgFindElementName(el->eid));
    if (el->id)
        fprintf(f, "  (id=%s)\n", el->id);
    else
        fprintf(f, "\n");

    switch (el->eid) {
        case EID_SVG :
            printSvgCookedAttr(f, el);
            break;
        case EID_G :
            printGCookedAttr(f, el);
            break;
        case EID_RECT :
            printRectCookedAttr(f, el);
            break;
        case EID_CIRCLE :
            printCircleCookedAttr(f, el);
            break;
        case EID_ELLIPSE :
            printEllipseCookedAttr(f, el);
            break;
        case EID_LINE :
            printLineCookedAttr(f, el);
            break;
        case EID_POLYLINE :
            printPolylineCookedAttr(f, el);
            break;
        case EID_POLYGON :
            printPolygonCookedAttr(f, el);
            break;
        case EID_TEXT :
            printTextCookedAttr(f, el);
            break;
        case EID_DEFS :
            printDefsCookedAttr(f, el);
            break;
        case EID_USE :
            printUseCookedAttr(f, el);
            break;
        default :
            break;
    }

    fprintf(f, "  --------- element MsvgPaintCtx\n");
    MsvgPrintPctx(f, &el->pctx);
}
