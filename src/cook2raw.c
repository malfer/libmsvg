/* cook2raw.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020-2022 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
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

static void addColorExtRawAttr(MsvgElement *el, char *key, rgbcolor color, char *iri)
{
    char s[41];
    
    if (color != NODEFINED_COLOR) {
        if (color == IRI_COLOR) {
            if (iri != NULL) {
                sprintf(s, "url(#%s", iri);
                MsvgAddRawAttribute(el, key, s);
            }
        }
        else {
            addColorRawAttr(el, key, color);
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

static void addTextRawAttr(MsvgElement *el, char *key, int value)
{
    if (value == NODEFINED_IVALUE) return;

    if (value == INHERIT_VALUE) MsvgAddRawAttribute(el, key, "inherit");

    if (strcmp(key,"text-anchor") == 0) {
        if (value == TEXTANCHOR_START) MsvgAddRawAttribute(el, key, "start");
        else if (value == TEXTANCHOR_MIDDLE) MsvgAddRawAttribute(el, key, "middle");
        else if (value == TEXTANCHOR_END) MsvgAddRawAttribute(el, key, "end");
    } else if (strcmp(key,"font-family") == 0) {
        if (value == FONTFAMILY_SANS) MsvgAddRawAttribute(el, key, "sans");
        else if (value == FONTFAMILY_SERIF) MsvgAddRawAttribute(el, key, "serif");
        else if (value == FONTFAMILY_CURSIVE) MsvgAddRawAttribute(el, key, "cursive");
        else if (value == FONTFAMILY_FANTASY) MsvgAddRawAttribute(el, key, "fantasy");
        else if (value == FONTFAMILY_MONOSPACE) MsvgAddRawAttribute(el, key, "monospace");
    } else if (strcmp(key,"font-style") == 0) {
        if (value == FONTSTYLE_NORMAL) MsvgAddRawAttribute(el, key, "normal");
        else if (value == FONTSTYLE_ITALIC) MsvgAddRawAttribute(el, key, "italic");
        else if (value == FONTSTYLE_OBLIQUE) MsvgAddRawAttribute(el, key, "oblique");
    } else if (strcmp(key,"font-weight") == 0) {
        if (value == FONTWEIGHT_100) MsvgAddRawAttribute(el, key, "100");
        else if (value == FONTWEIGHT_100) MsvgAddRawAttribute(el, key, "100");
        else if (value == FONTWEIGHT_200) MsvgAddRawAttribute(el, key, "200");
        else if (value == FONTWEIGHT_300) MsvgAddRawAttribute(el, key, "300");
        else if (value == FONTWEIGHT_400) MsvgAddRawAttribute(el, key, "normal");
        else if (value == FONTWEIGHT_500) MsvgAddRawAttribute(el, key, "500");
        else if (value == FONTWEIGHT_600) MsvgAddRawAttribute(el, key, "600");
        else if (value == FONTWEIGHT_700) MsvgAddRawAttribute(el, key, "bold");
        else if (value == FONTWEIGHT_800) MsvgAddRawAttribute(el, key, "800");
        else if (value == FONTWEIGHT_900) MsvgAddRawAttribute(el, key, "900");
    }
}

static void torawPCtxAttr(MsvgElement *el)
{
    char s[121];
    TMatrix *tm;

    if (el->id) MsvgAddRawAttribute(el, "id", el->id);

    if (el->pctx == NULL) return;

    addColorExtRawAttr(el, "fill", el->pctx->fill, el->pctx->fill_iri);
    addSpcDblRawAttr(el, "fill-opacity", el->pctx->fill_opacity);
    addColorExtRawAttr(el, "stroke", el->pctx->stroke, el->pctx->stroke_iri);
    addSpcDblRawAttr(el, "stroke-width", el->pctx->stroke_width);
    addSpcDblRawAttr(el, "stroke-opacity", el->pctx->stroke_opacity);
    tm = &(el->pctx->tmatrix);
    if (!TMIsIdentity(tm)) {
        sprintf(s, "matrix(%g %g %g %g %g %g)",
                tm->a, tm->b, tm->c, tm->d, tm->e, tm->f);
        MsvgAddRawAttribute(el, "transform", s);
    }
    addTextRawAttr(el, "text-anchor", el->pctx->text_anchor);
    addTextRawAttr(el, "font-family", el->pctx->font_family);
    addTextRawAttr(el, "font-style", el->pctx->font_style);
    addTextRawAttr(el, "font-weight", el->pctx->font_weight);
    addSpcDblRawAttr(el, "font-size", el->pctx->font_size);
}

static void toRawSvgCookedAttr(MsvgElement *el)
{
    char s[81];
    
    MsvgAddRawAttribute(el, "version", "1.2");
    MsvgAddRawAttribute(el, "baseProfile", "tiny");
    MsvgAddRawAttribute(el, "xmlns", "http://www.w3.org/2000/svg");
    MsvgAddRawAttribute(el, "xmlns:xlink", "http://www.w3.org/1999/xlink");
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

    s = malloc(sizeof(char)*el->ppolylineattr->npoints*40+1);
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

    s = malloc(sizeof(char)*el->ppolygonattr->npoints*40+1);
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
    MsvgSubPath *sp;
    int i, n;
    char *s, *p;

    sp = el->ppathattr->sp;

    s = malloc(sizeof(char)*sp->npoints*40+1);
    if (s == NULL) return;
    p = s;

    for (i=0; i<sp->npoints; i++) {
        if (sp->spp[i].cmd != ' ') {
            n = sprintf(p, "%c", sp->spp[i].cmd);
            p += n;
        }
        n = sprintf(p, "%g,%g ", sp->spp[i].x, sp->spp[i].y);
        p += n;
    }

    if (sp->closed) sprintf(p, "Z");

    MsvgAddRawAttribute(el, "d", s);

    free(s);
}

static void toRawTextCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "x", el->ptextattr->x);
    addDoubleRawAttr(el, "y", el->ptextattr->y);
}

static void toRawLinearGradientCookedAttr(MsvgElement *el)
{
    if (el->plgradattr->gradunits == GRADUNIT_USER)
        MsvgAddRawAttribute(el, "gradientUnits", "userSpaceOnUse");
    else
        MsvgAddRawAttribute(el, "gradientUnits", "objectBoundingBox");

    addDoubleRawAttr(el, "x1", el->plgradattr->x1);
    addDoubleRawAttr(el, "y1", el->plgradattr->y1);
    addDoubleRawAttr(el, "x2", el->plgradattr->x2);
    addDoubleRawAttr(el, "y2", el->plgradattr->y2);
}

static void toRawRadialGradientCookedAttr(MsvgElement *el)
{
    if (el->prgradattr->gradunits == GRADUNIT_USER)
        MsvgAddRawAttribute(el, "gradientUnits", "userSpaceOnUse");
    else
        MsvgAddRawAttribute(el, "gradientUnits", "objectBoundingBox");

    addDoubleRawAttr(el, "cx", el->prgradattr->cx);
    addDoubleRawAttr(el, "cy", el->prgradattr->cy);
    addDoubleRawAttr(el, "r", el->prgradattr->r);
}

static void toRawStopCookedAttr(MsvgElement *el)
{
    addDoubleRawAttr(el, "offset", el->pstopattr->offset);
    addSpcDblRawAttr(el, "stop-opacity", el->pstopattr->sopacity);
    addColorRawAttr(el, "stop-color", el->pstopattr->scolor);
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
        case EID_LINEARGRADIENT :
            toRawLinearGradientCookedAttr(el);
            break;
        case EID_RADIALGRADIENT :
            toRawRadialGradientCookedAttr(el);
            break;
        case EID_STOP :
            toRawStopCookedAttr(el);
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
