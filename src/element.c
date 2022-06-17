/* element.c
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
#include "msvg.h"

static MsvgElement *MsvgNewGenericElement(enum EID eid, MsvgElement *father, int addpctx)
{
    MsvgElement *element;
    MsvgElement *ptr;
    MsvgPaintCtx *pctx = NULL;

    if (addpctx) {
        pctx = MsvgNewPaintCtx(NULL);
        if (pctx == NULL) return NULL;
    }

    element = calloc(1, sizeof(MsvgElement));
    if (element == NULL) {
        if (pctx) MsvgDestroyPaintCtx(pctx);
        return NULL;
    }

    element->eid = eid;

    if (father) {
        element->father = father;
        if (father->fson == NULL) {
            father->fson = element;
        } else {
            ptr = father->fson;
            while (ptr->nsibling != NULL)
                ptr = ptr->nsibling;
            ptr->nsibling = element;
            element->psibling = ptr;
        }
    }

    element->fcontent = NULL;
    element->id = NULL;
    element->pctx = pctx;

    return element;
}

static MsvgElement *MsvgNewSvgElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgSvgAttributes *psvgattr;

    psvgattr = calloc(1, sizeof(MsvgSvgAttributes));
    if (psvgattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_SVG, father, 1);
    if (element == NULL) {
        free(psvgattr);
        return NULL;
    }

    element->psvgattr = psvgattr;
    element->psvgattr->width = 0;
    element->psvgattr->height = 0;
    element->psvgattr->vb_min_x = 0;
    element->psvgattr->vb_min_y = 0;
    element->psvgattr->vb_width = 0;
    element->psvgattr->vb_height = 0;
    element->psvgattr->vp_fill = NO_COLOR; /* black */
    element->psvgattr->vp_fill_opacity = 1; /* solid */
    element->psvgattr->tree_type = RAW_SVGTREE;
    return element;
}

static MsvgElement *MsvgNewDefsElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgDefsAttributes *pdefsattr;

    pdefsattr = calloc(1, sizeof(MsvgDefsAttributes));
    if (pdefsattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_DEFS, father, 0);
    if (element == NULL) {
        free(pdefsattr);
        return NULL;
    }

    element->pdefsattr = pdefsattr;
    return element;
}

static MsvgElement *MsvgNewGElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgGAttributes *pgattr;

    pgattr = calloc(1, sizeof(MsvgGAttributes));
    if (pgattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_G, father, 1);
    if (element == NULL) {
        free(pgattr);
        return NULL;
    }

    element->pgattr = pgattr;
    return element;
}

static MsvgElement *MsvgNewUseElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgUseAttributes *puseattr;

    puseattr = calloc(1, sizeof(MsvgUseAttributes));
    if (puseattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_USE, father, 1);
    if (element == NULL) {
        free(puseattr);
        return NULL;
    }

    element->puseattr = puseattr;
    element->puseattr->refel = NULL;
    element->puseattr->x = 0;
    element->puseattr->y = 0;
    return element;
}

static MsvgElement *MsvgNewRectElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgRectAttributes *prectattr;

    prectattr = calloc(1, sizeof(MsvgRectAttributes));
    if (prectattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_RECT, father, 1);
    if (element == NULL) {
        free(prectattr);
        return NULL;
    }
    
    element->prectattr = prectattr;
    element->prectattr->x = 0;
    element->prectattr->y = 0;
    element->prectattr->width = 0;
    element->prectattr->height = 0;
    element->prectattr->rx = NODEFINED_VALUE;
    element->prectattr->ry = NODEFINED_VALUE;
    return element;
}

static MsvgElement *MsvgNewCircleElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgCircleAttributes *pcircleattr;

    pcircleattr = calloc(1, sizeof(MsvgCircleAttributes));
    if (pcircleattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_CIRCLE, father, 1);
    if (element == NULL) {
        free(pcircleattr);
        return NULL;
    }

    element->pcircleattr = pcircleattr;
    element->pcircleattr->cx = 0;
    element->pcircleattr->cy = 0;
    element->pcircleattr->r = 0;
    return element;
}

static MsvgElement *MsvgNewEllipseElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgEllipseAttributes *pellipseattr;

    pellipseattr = calloc(1, sizeof(MsvgEllipseAttributes));
    if (pellipseattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_ELLIPSE, father, 1);
    if (element == NULL) {
        free(pellipseattr);
        return NULL;
    }

    element->pellipseattr = pellipseattr;
    element->pellipseattr->cx = 0;
    element->pellipseattr->cy = 0;
    element->pellipseattr->rx_x = 0;
    element->pellipseattr->rx_y = 0;
    element->pellipseattr->ry_x = 0;
    element->pellipseattr->ry_y = 0;
    return element;
}

static MsvgElement *MsvgNewLineElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgLineAttributes *plineattr;

    plineattr = calloc(1, sizeof(MsvgLineAttributes));
    if (plineattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_LINE, father, 1);
    if (element == NULL) {
        free(plineattr);
        return NULL;
    }

    element->plineattr = plineattr;
    element->plineattr->x1 = 0;
    element->plineattr->y1 = 0;
    element->plineattr->x2 = 0;
    element->plineattr->y2 = 0;
    return element;
}

static MsvgElement *MsvgNewPolylineElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgPolylineAttributes *ppolylineattr;

    ppolylineattr = calloc(1, sizeof(MsvgPolylineAttributes));
    if (ppolylineattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_POLYLINE, father, 1);
    if (element == NULL) {
        free(ppolylineattr);
        return NULL;
    }

    element->ppolylineattr = ppolylineattr;
    element->ppolylineattr->points = NULL;
    element->ppolylineattr->npoints = 0;
    return element;
}

static MsvgElement *MsvgNewPolygonElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgPolygonAttributes *ppolygonattr;

    ppolygonattr = calloc(1, sizeof(MsvgPolygonAttributes));
    if (ppolygonattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_POLYGON, father, 1);
    if (element == NULL) {
        free(ppolygonattr);
        return NULL;
    }

    element->ppolygonattr = ppolygonattr;
    element->ppolygonattr->points = NULL;
    element->ppolygonattr->npoints = 0;
    return element;
}

static MsvgElement *MsvgNewPathElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgPathAttributes *ppathattr;

    ppathattr = calloc(1, sizeof(MsvgPathAttributes));
    if (ppathattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_PATH, father, 1);
    if (element == NULL) {
        free(ppathattr);
        return NULL;
    }

    element->ppathattr = ppathattr;
    element->ppathattr->sp = NULL;
    return element;
}

static MsvgElement *MsvgNewTextElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgTextAttributes *ptextattr;

    ptextattr = calloc(1, sizeof(MsvgTextAttributes));
    if (ptextattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_TEXT, father, 1);
    if (element == NULL) {
        free(ptextattr);
        return NULL;
    }

    element->ptextattr = ptextattr;
    element->ptextattr->x = 0;
    element->ptextattr->y = 0;
    return element;
}

static MsvgElement *MsvgNewVContentElement(MsvgElement *father)
{
    MsvgElement *element;

    element = MsvgNewGenericElement(EID_V_CONTENT, father, 0);
    if (element == NULL) return NULL;

    return element;
}

static MsvgElement *MsvgNewLinearGradientElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgLinearGradientAttributes *plgradattr;

    plgradattr = calloc(1, sizeof(MsvgLinearGradientAttributes));
    if (plgradattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_LINEARGRADIENT, father, 0);
    if (element == NULL) {
        free(plgradattr);
        return NULL;
    }

    element->plgradattr = plgradattr;
    element->plgradattr->gradunits = GRADUNIT_BBOX;
    element->plgradattr->x1 = 0;
    element->plgradattr->y1 = 0;
    element->plgradattr->x2 = 1;
    element->plgradattr->y2 = 0;
    return element;
}

static MsvgElement *MsvgNewRadialGradientElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgRadialGradientAttributes *prgradattr;

    prgradattr = calloc(1, sizeof(MsvgRadialGradientAttributes));
    if (prgradattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_RADIALGRADIENT, father, 0);
    if (element == NULL) {
        free(prgradattr);
        return NULL;
    }

    element->prgradattr = prgradattr;
    element->prgradattr->gradunits = GRADUNIT_BBOX;
    element->prgradattr->cx = 0.5;
    element->prgradattr->cy = 0.5;
    element->prgradattr->r = 0.5;
    return element;
}

static MsvgElement *MsvgNewStopElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgStopAttributes *pstopattr;

    pstopattr = calloc(1, sizeof(MsvgLinearGradientAttributes));
    if (pstopattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_STOP, father, 0);
    if (element == NULL) {
        free(pstopattr);
        return NULL;
    }

    element->pstopattr = pstopattr;
    element->pstopattr->offset = 0;
    element->pstopattr->sopacity = 1;
    element->pstopattr->scolor = 0; /* black */
    return element;
}

static MsvgElement *MsvgNewFontElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgFontAttributes *pfontattr;

    pfontattr = calloc(1, sizeof(MsvgFontAttributes));
    if (pfontattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_FONT, father, 0);
    if (element == NULL) {
        free(pfontattr);
        return NULL;
    }

    element->pfontattr = pfontattr;
    element->pfontattr->horiz_adv_x = 0;
    return element;
}

static MsvgElement *MsvgNewFontFaceElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgFontFaceAttributes *pfontfaceattr;

    pfontfaceattr = calloc(1, sizeof(MsvgFontFaceAttributes));
    if (pfontfaceattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_FONTFACE, father, 0);
    if (element == NULL) {
        free(pfontfaceattr);
        return NULL;
    }

    element->pfontfaceattr = pfontfaceattr;
    element->pfontfaceattr->sfont_family = NULL;
    element->pfontfaceattr->font_family = NODEFINED_IVALUE;
    element->pfontfaceattr->font_style = NODEFINED_IVALUE;
    element->pfontfaceattr->font_weight = NODEFINED_IVALUE;
    element->pfontfaceattr->units_per_em = 1000.0;
    element->pfontfaceattr->ascent = NODEFINED_VALUE;
    element->pfontfaceattr->descent = NODEFINED_VALUE;
    return element;
}

static MsvgElement *MsvgNewMissingGlyphElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgGlyphAttributes *pglyphattr;

    pglyphattr = calloc(1, sizeof(MsvgGlyphAttributes));
    if (pglyphattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_MISSINGGLYPH, father, 0);
    if (element == NULL) {
        free(pglyphattr);
        return NULL;
    }

    element->pglyphattr = pglyphattr;
    element->pglyphattr->unicode = 0;
    element->pglyphattr->horiz_adv_x = NODEFINED_VALUE;
    element->pglyphattr->sp = NULL;
    return element;
}

static MsvgElement *MsvgNewGlyphElement(MsvgElement *father)
{
    MsvgElement *element;
    MsvgGlyphAttributes *pglyphattr;

    pglyphattr = calloc(1, sizeof(MsvgGlyphAttributes));
    if (pglyphattr == NULL) return NULL;

    element = MsvgNewGenericElement(EID_GLYPH, father, 0);
    if (element == NULL) {
        free(pglyphattr);
        return NULL;
    }

    element->pglyphattr = pglyphattr;
    element->pglyphattr->unicode = 0;
    element->pglyphattr->horiz_adv_x = NODEFINED_VALUE;
    element->pglyphattr->sp = NULL;
    return element;
}

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father)
{
    MsvgElement *element;

    switch (eid) {
        case EID_SVG :
            element = MsvgNewSvgElement(father);
            break;
        case EID_DEFS :
            element = MsvgNewDefsElement(father);
            break;
        case EID_G :
            element = MsvgNewGElement(father);
            break;
        case EID_USE :
            element = MsvgNewUseElement(father);
            break;
        case EID_RECT :
            element = MsvgNewRectElement(father);
            break;
        case EID_CIRCLE :
            element = MsvgNewCircleElement(father);
            break;
        case EID_ELLIPSE :
            element = MsvgNewEllipseElement(father);
            break;
        case EID_LINE :
            element = MsvgNewLineElement(father);
            break;
        case EID_POLYLINE :
            element = MsvgNewPolylineElement(father);
            break;
        case EID_POLYGON :
            element = MsvgNewPolygonElement(father);
            break;
        case EID_PATH :
            element = MsvgNewPathElement(father);
            break;
        case EID_TEXT :
            element = MsvgNewTextElement(father);
            break;
        case EID_LINEARGRADIENT :
            element = MsvgNewLinearGradientElement(father);
            break;
        case EID_RADIALGRADIENT :
            element = MsvgNewRadialGradientElement(father);
            break;
        case EID_STOP :
            element = MsvgNewStopElement(father);
            break;
        case EID_FONT :
            element = MsvgNewFontElement(father);
            break;
        case EID_FONTFACE :
            element = MsvgNewFontFaceElement(father);
            break;
        case EID_MISSINGGLYPH :
            element = MsvgNewMissingGlyphElement(father);
            break;
        case EID_GLYPH :
            element = MsvgNewGlyphElement(father);
            break;
        case EID_V_CONTENT :
            element = MsvgNewVContentElement(father);
            break;
        default :
            return NULL;
    }

    return element;
}

int MsvgAllocPointsToPolylineElement(MsvgElement *el, int npoints)
{
    double *points;

    if (el->eid != EID_POLYLINE) return 0;
    points = (double *)calloc(npoints*2, sizeof(double));
    if (points == NULL) return 0;

    if (el->ppolylineattr->points) free(el->ppolylineattr->points);
    el->ppolylineattr->points = points;
    el->ppolylineattr->npoints = npoints;

    return 1;
}

int MsvgAllocPointsToPolygonElement(MsvgElement *el, int npoints)
{
    double *points;

    if (el->eid != EID_POLYGON) return 0;
    points = (double *)calloc(npoints*2, sizeof(double));
    if (points == NULL) return 0;

    if (el->ppolygonattr->points) free(el->ppolygonattr->points);
    el->ppolygonattr->points = points;
    el->ppolygonattr->npoints = npoints;

    return 1;
}

