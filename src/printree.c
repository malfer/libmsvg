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

static void printAttribute(FILE *f, MsvgRawAttribute *ptr)
{
    if (ptr == NULL) return;
    
    fprintf(f, " (%s = %s)", ptr->key, ptr->value);
    
    printAttribute(f, ptr->nrattr);
}

void MsvgPrintElementTree(FILE *f, MsvgElement *ptr, int depth)
{
    int i;
    
    if (ptr == NULL) return;
    
    if (depth > 0) {
        for (i=0; i<depth; i++)
            fputs("  |", f);
        fputs("-->", f);
    }
    
    fprintf(f, "%s", MsvgFindElementName(ptr->eid));
    printAttribute(f, ptr->frattr);
    fputs("\n", f);
    
    MsvgPrintElementTree(f, ptr->fson, depth+1);
    
    MsvgPrintElementTree(f, ptr->nsibling, depth);
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

void MsvgPrintCookedElement(FILE *f, MsvgElement *ptr)
{
    fprintf(f, "%s", MsvgFindElementName(ptr->eid));
    if (ptr->id)
        fprintf(f, "  (id=%s)\n", ptr->id);
    else
        fprintf(f, "\n");

    MsvgPrintPctx(f, &ptr->pctx);

/*    switch (el->eid) {
        case EID_SVG :
            cookSvgGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_G :
            cookGGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_RECT :
            cookRectGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_CIRCLE :
            cookCircleGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_ELLIPSE :
            cookEllipseGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_LINE :
            cookLineGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_POLYLINE :
            cookPolylineGenAttr(el, pattr->key, pattr->value);
            break;
        case EID_POLYGON :
            cookPolygonGenAttr(el, pattr->key, pattr->value);
            break;
        default :
            break;
    }*/
}
