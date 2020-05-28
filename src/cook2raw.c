/* cook2raw.c
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "msvg.h"
#include "util.h"

static void addColorRawAttr(MsvgElement *el, char *key, rgbcolor color)
{
    char s[41];
    
    if (color != NODEFINED_COLOR) {
        if (color == NO_COLOR)
            MsvgAddRawAttribute(el, key, "none");
        else if (color == INHERIT_COLOR)
            MsvgAddRawAttribute(el, key, "inherit");
        else {
            sprintf(s, "#%06x", color);
            MsvgAddRawAttribute(el, key, s);
        }
    }
}

static void addSpcDblRawAttr(MsvgElement *el, char *key, double value)
{
    char s[41];

    if (value != NODEFINED_VALUE) {
        if (value == INHERIT_VALUE)
            MsvgAddRawAttribute(el, key, "inherit");
        else {
            sprintf(s, "%g", value);
            MsvgAddRawAttribute(el, key, s);
        }
    }
}

static void addDoubleRawAttr(MsvgElement *el, char *key, double value)
{
    char s[41];

    sprintf(s, "%g", value);
    MsvgAddRawAttribute(el, key, s);
}

static void torawPCtxAttr(MsvgElement *el)
{
    char s[121];
    TMatrix *tm;

    if (el->id) MsvgAddRawAttribute(el, "id", el->id);
    addColorRawAttr(el, "fill", el->pctx.fill);
    addSpcDblRawAttr(el, "fill-opacity", el->pctx.fill_opacity);
    addColorRawAttr(el, "stroke", el->pctx.stroke);
    addSpcDblRawAttr(el, "stroke-width", el->pctx.stroke_width);
    addSpcDblRawAttr(el, "stroke-opacity", el->pctx.stroke_opacity);
    tm = &(el->pctx.tmatrix);
    if (!TMIsIdentity(tm)) {
        sprintf(s, "matrix(%g %g %g %g %g %g)",
                tm->a, tm->b, tm->c, tm->d, tm->e, tm->f);
        MsvgAddRawAttribute(el, "transform", s);
    }
}

static void toRawSvgCookedAttr(MsvgElement *el)
{
    char s[81];
    
    MsvgAddRawAttribute(el, "xmlns", "http://www.w3.org/2000/svg");
    MsvgAddRawAttribute(el, "version", "1.2");
    MsvgAddRawAttribute(el, "baseProfile", "tiny");
    sprintf(s, "%g %g %g %g", el->psvgattr->vb_min_x, el->psvgattr->vb_min_y,
            el->psvgattr->vb_width, el->psvgattr->vb_height);
    MsvgAddRawAttribute(el, "viewBox", s);
    addColorRawAttr(el, "vieport-fill", el->psvgattr->vp_fill);
    addSpcDblRawAttr(el, "vieport-fill-opacity", el->psvgattr->vp_fill_opacity);
}

static void toRawDefsCookedAttr(MsvgElement *el)
{
    return;
}

static void toRawGCookedAttr(MsvgElement *el)
{
    return;
}

static void toRawUseCookedAttr(MsvgElement *el)
{
    char s[81];

    addDoubleRawAttr(el, "x", el->puseattr->x);
    addDoubleRawAttr(el, "y", el->puseattr->y);
    if (el->puseattr->refel) {
        sprintf(s, "#%s", el->puseattr->refel);
        MsvgAddRawAttribute(el, "xlink:href", s);
    }
}

static void toRawRectCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "x", el->prectattr->x);
    addDoubleRawAttr(el, "y", el->prectattr->y);
    addDoubleRawAttr(el, "width", el->prectattr->width);
    addDoubleRawAttr(el, "height", el->prectattr->height);
    if (el->prectattr->rx > 0)
        addDoubleRawAttr(el, "rx", el->prectattr->rx);
    if (el->prectattr->ry > 0)
        addDoubleRawAttr(el, "ry", el->prectattr->ry);
}

static void toRawCircleCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "cx", el->pcircleattr->cx);
    addDoubleRawAttr(el, "cy", el->pcircleattr->cy);
    addDoubleRawAttr(el, "r", el->pcircleattr->r);
}

static void toRawEllipseCookedAttr(MsvgElement *el)
{
    double rx, ry;

    addDoubleRawAttr(el, "cx", el->pellipseattr->cx);
    addDoubleRawAttr(el, "cy", el->pellipseattr->cy);
    rx = sqrt(pow(el->pellipseattr->rx_x-el->pellipseattr->cx, 2) +
              pow(el->pellipseattr->rx_y-el->pellipseattr->cy, 2));
    addDoubleRawAttr(el, "rx", rx);
    ry = sqrt(pow(el->pellipseattr->ry_x-el->pellipseattr->cx, 2) +
              pow(el->pellipseattr->ry_y-el->pellipseattr->cy, 2));
    addDoubleRawAttr(el, "ry", ry);
}

static void toRawLineCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "x1", el->plineattr->x1);
    addDoubleRawAttr(el, "y1", el->plineattr->y1);
    addDoubleRawAttr(el, "x2", el->plineattr->x2);
    addDoubleRawAttr(el, "y2", el->plineattr->y2);
}

static void toRawPolylineCookedAttr(MsvgElement *el)
{
    int i, n;
    char *s, *p;

    s = malloc(sizeof(char)*el->ppolylineattr->npoints*30+1);
    if (s == NULL) return;
    p = s;

    for (i=0; i<el->ppolylineattr->npoints; i++) {
        n = sprintf(p, "%g,%g ", el->ppolylineattr->points[i*2],
                    el->ppolylineattr->points[i*2+1]);
        p += n;
    }

    MsvgAddRawAttribute(el, "points", s);

    free(s);
}

static void toRawPolygonCookedAttr(MsvgElement *el)
{
    int i, n;
    char *s, *p;

    s = malloc(sizeof(char)*el->ppolygonattr->npoints*30+1);
    if (s == NULL) return;
    p = s;

    for (i=0; i<el->ppolygonattr->npoints; i++) {
        n = sprintf(p, "%g,%g ", el->ppolygonattr->points[i*2],
                    el->ppolygonattr->points[i*2+1]);
        p += n;
    }

    MsvgAddRawAttribute(el, "points", s);

    free(s);
}

static void toRawPathCookedAttr(MsvgElement *el)
{
    // TODO
    return;
}

static void toRawTextCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "x", el->ptextattr->x);
    addDoubleRawAttr(el, "y", el->ptextattr->y);
    addDoubleRawAttr(el, "font-size", el->ptextattr->font_size);
    if (el->ptextattr->font_family) {
        MsvgAddRawAttribute(el, "font-family", el->ptextattr->font_family);
    }
}

static void toRawElement(MsvgElement *el)
{
    torawPCtxAttr(el);

    switch (el->eid) {
        case EID_SVG :
            toRawSvgCookedAttr(el);
            break;
        case EID_DEFS :
            toRawDefsCookedAttr(el);
            break;
        case EID_G :
            toRawGCookedAttr(el);
            break;
        case EID_USE :
            toRawUseCookedAttr(el);
            break;
        case EID_RECT :
            toRawRectCookedAttr(el);
            break;
        case EID_CIRCLE :
            toRawCircleCookedAttr(el);
            break;
        case EID_ELLIPSE :
            toRawEllipseCookedAttr(el);
            break;
        case EID_LINE :
            toRawLineCookedAttr(el);
            break;
        case EID_POLYLINE :
            toRawPolylineCookedAttr(el);
            break;
        case EID_POLYGON :
            toRawPolygonCookedAttr(el);
            break;
        case EID_PATH :
            toRawPathCookedAttr(el);
            break;
        case EID_TEXT :
            toRawTextCookedAttr(el);
            break;
        default :
            break;
    }

    if (el->fson != NULL)
        toRawElement(el->fson);
    
    if (el->nsibling != NULL)
        toRawElement(el->nsibling);
}

int MsvgCooked2RawTree(MsvgElement *root)
{
    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;

    toRawElement(root);
    //root->psvgattr->tree_type = RAW_SVGTREE;
    
    return 1;
}
