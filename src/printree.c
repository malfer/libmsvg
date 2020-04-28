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
        sprintf(s, "%f", value);

    return s;
}

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx)
{
    fprintf(f, "  fill           %s\n", printcolor(pctx->fill));
    fprintf(f, "  fill_opacity   %s\n", printvalue(pctx->fill_opacity));
    fprintf(f, "  stroke         %s\n", printcolor(pctx->stroke));
    fprintf(f, "  stroke_width   %s\n", printvalue(pctx->stroke_width));
    fprintf(f, "  stroke_opacity %s\n", printvalue(pctx->stroke_opacity));
}

static void printSvgCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  width          %f\n", el->psvgattr->width);
    fprintf(f, "  height         %f\n", el->psvgattr->height);
    fprintf(f, "  vb_min_x       %f\n", el->psvgattr->vb_min_x);
    fprintf(f, "  vb_min_y       %f\n", el->psvgattr->vb_min_y);
    fprintf(f, "  vb_width       %f\n", el->psvgattr->vb_width);
    fprintf(f, "  vb_height      %f\n", el->psvgattr->vb_height);
    fprintf(f, "  vp_fill         %s\n", printcolor(el->psvgattr->vp_fill));
    fprintf(f, "  vp_fill_opacity %s\n", printvalue(el->psvgattr->vp_fill_opacity));
}

static void printGCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printRectCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %f\n", el->prectattr->x);
    fprintf(f, "  y              %f\n", el->prectattr->y);
    fprintf(f, "  width          %f\n", el->prectattr->width);
    fprintf(f, "  height         %f\n", el->prectattr->height);
    fprintf(f, "  rx             %f\n", el->prectattr->rx);
    fprintf(f, "  ry             %f\n", el->prectattr->ry);
}

static void printCircleCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  cx             %f\n", el->pcircleattr->cx);
    fprintf(f, "  cy             %f\n", el->pcircleattr->cy);
    fprintf(f, "  r              %f\n", el->pcircleattr->r);
}

static void printEllipseCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  cx             %f\n", el->pellipseattr->cx);
    fprintf(f, "  cy             %f\n", el->pellipseattr->cy);
    fprintf(f, "  rx             %f\n", el->pellipseattr->rx);
    fprintf(f, "  ry             %f\n", el->pellipseattr->ry);
}

static void printLineCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x1             %f\n", el->plineattr->x1);
    fprintf(f, "  y1             %f\n", el->plineattr->y1);
    fprintf(f, "  x2             %f\n", el->plineattr->x2);
    fprintf(f, "  y3             %f\n", el->plineattr->y2);
}

static void printPolylineCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printPolygonCookedAttr(FILE *f, MsvgElement *el)
{
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
        default :
            break;
    }

    fprintf(f, "  --------- element MsvgPaintCtx\n");
    MsvgPrintPctx(f, &el->pctx);
}
