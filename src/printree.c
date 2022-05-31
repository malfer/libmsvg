/* printree.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include "msvg.h"

static void printRawAttribute(FILE *f, MsvgRawAttribute *el)
{
    if (el == NULL) return;
    
    fprintf(f, " (%s = %s)", el->key, el->value);
    
    printRawAttribute(f, el->nrattr);
}

static void printContents(FILE *f, MsvgElement *el, int depth)
{
    int i;
   
    if (el->fcontent == NULL) return;

    if (depth > 0) {
        for (i=0; i<depth; i++)
            fputs("  |", f);
        fputs("   ", f);
    }
    fprintf(f, "(%d) %s\n", el->fcontent->len, el->fcontent->s);
}

/*static void printContents(FILE *f, MsvgElement *el, int depth)
{
    // linked list of contents version
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
}*/

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
    
    if (depth) MsvgPrintRawElementTree(f, el->nsibling, depth);
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
    else if (color == IRI_COLOR)
        sprintf(s, "IRI_COLOR");
    else
        sprintf(s, "#%06x", color);
    
    return s;
}

static char * printdvalue(double value)
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

static char * printivalue(int value)
{
    static char s[81];

    if (value == INHERIT_IVALUE)
        sprintf(s, "INHERIT_IVALUE");
    else if (value == NODEFINED_IVALUE)
        sprintf(s, "NODEFINED_IVALUE");
    else
        sprintf(s, "%d", value);

    return s;
}

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx)
{
    fprintf(f, "  fill           %s\n", printcolor(pctx->fill));
    if (pctx->fill_iri)
        fprintf(f, "  fill_iri       %s\n", pctx->fill_iri);
    fprintf(f, "  fill_opacity   %s\n", printdvalue(pctx->fill_opacity));
    fprintf(f, "  stroke         %s\n", printcolor(pctx->stroke));
    if (pctx->stroke_iri)
        fprintf(f, "  stroke_iri     %s\n", pctx->stroke_iri);
    fprintf(f, "  stroke_width   %s\n", printdvalue(pctx->stroke_width));
    fprintf(f, "  stroke_opacity %s\n", printdvalue(pctx->stroke_opacity));
    fprintf(f, "  tmatrix        (%g %g %g %g %g %g)\n",
            pctx->tmatrix.a, pctx->tmatrix.b, pctx->tmatrix.c,
            pctx->tmatrix.d, pctx->tmatrix.e, pctx->tmatrix.f);
    fprintf(f, "  font-family    %s\n", printivalue(pctx->font_family));
    fprintf(f, "  font-style     %s\n", printivalue(pctx->font_style));
    fprintf(f, "  font-weight    %s\n", printivalue(pctx->font_weight));
    fprintf(f, "  font-size      %s\n", printdvalue(pctx->font_size));
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
    fprintf(f, "  vp_fill_opacity %s\n", printdvalue(el->psvgattr->vp_fill_opacity));
}

static void printDefsCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printGCookedAttr(FILE *f, MsvgElement *el)
{
}

static void printUseCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %g\n", el->puseattr->x);
    fprintf(f, "  y              %g\n", el->puseattr->y);
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

static void printPathCookedAttr(FILE *f, MsvgElement *el)
{
    MsvgSubPath *sp;
    int i;

    sp = el->ppathattr->sp;
    while (sp) {
        fprintf(f, "  sp             (npoints:%d closed:%d) ",
                sp->npoints, sp->closed);
        for (i=0; i<sp->npoints; i++) {
            if (sp->spp[i].cmd != ' ') fprintf(f, "%c ", sp->spp[i].cmd);
            fprintf(f, "%g,%g ", sp->spp[i].x, sp->spp[i].y);
        }
        fprintf(f, "\n");
        sp = sp->next;
    }
}

static void printTextCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  x              %g\n", el->ptextattr->x);
    fprintf(f, "  y              %g\n", el->ptextattr->y);
}

static void printLinearGradientCookedAttr(FILE *f, MsvgElement *el)
{
    if (el->plgradattr->gradunits == GRADUNIT_USER)
        fprintf(f, "  gradientUnits  %s\n", "GRADUNIT_USER");
    else
        fprintf(f, "  gradientUnits  %s\n", "GRADUNIT_BBOX");

    fprintf(f, "  x1             %g\n", el->plgradattr->x1);
    fprintf(f, "  y1             %g\n", el->plgradattr->y1);
    fprintf(f, "  x2             %g\n", el->plgradattr->x2);
    fprintf(f, "  y2             %g\n", el->plgradattr->y2);
}

static void printRadialGradientCookedAttr(FILE *f, MsvgElement *el)
{
    if (el->prgradattr->gradunits == GRADUNIT_USER)
        fprintf(f, "  gradientUnits  %s\n", "GRADUNIT_USER");
    else
        fprintf(f, "  gradientUnits  %s\n", "GRADUNIT_BBOX");

    fprintf(f, "  cx             %g\n", el->prgradattr->cx);
    fprintf(f, "  cy             %g\n", el->prgradattr->cy);
    fprintf(f, "  r              %g\n", el->prgradattr->r);
}

static void printStopCookedAttr(FILE *f, MsvgElement *el)
{
    fprintf(f, "  offset         %g\n", el->pstopattr->offset);
    fprintf(f, "  stop-opacity   %s\n", printdvalue(el->pstopattr->sopacity));
    fprintf(f, "  stop-color     %s\n", printcolor(el->pstopattr->scolor));
}

static void printVContentCookedAttr(FILE *f, MsvgElement *el)
{
    return;
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
        case EID_DEFS :
            printDefsCookedAttr(f, el);
            break;
        case EID_G :
            printGCookedAttr(f, el);
            break;
        case EID_USE :
            printUseCookedAttr(f, el);
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
        case EID_PATH :
            printPathCookedAttr(f, el);
            break;
        case EID_TEXT :
            printTextCookedAttr(f, el);
            break;
        case EID_LINEARGRADIENT :
            printLinearGradientCookedAttr(f, el);
            break;
        case EID_RADIALGRADIENT :
            printRadialGradientCookedAttr(f, el);
            break;
        case EID_STOP :
            printStopCookedAttr(f, el);
            break;
        case EID_V_CONTENT :
            printVContentCookedAttr(f, el);
            break;
        default :
            break;
    }

    if (el->pctx) {
        fprintf(f, "  --------- element MsvgPaintCtx\n");
        MsvgPrintPctx(f, el->pctx);
    }
}
