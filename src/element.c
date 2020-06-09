/* element.c
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
#include "msvg.h"

static MsvgElement *MsvgNewGenericElement(enum EID eid, MsvgElement *father)
{
    MsvgElement *element;
    MsvgElement *ptr;
    
    element = calloc(1, sizeof(MsvgElement));
    if (element == NULL) return NULL;
    
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
    
    element->pctx.fill = NODEFINED_COLOR;
    element->pctx.fill_opacity = NODEFINED_VALUE;
    element->pctx.stroke = NODEFINED_COLOR;
    element->pctx.stroke_width = NODEFINED_VALUE;
    element->pctx.stroke_opacity = NODEFINED_VALUE;
    TMSetIdentity(&(element->pctx.tmatrix));
    element->pctx.font_family = NODEFINED_IVALUE;
    element->pctx.font_style = NODEFINED_IVALUE;
    element->pctx.font_weight = NODEFINED_IVALUE;
    element->pctx.font_size = NODEFINED_VALUE;

    return element;
}

static MsvgElement *MsvgNewSvgElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_SVG, father);
    if (element == NULL) return NULL;
    
    element->psvgattr = calloc(1, sizeof(MsvgSvgAttributes));
    if (element->psvgattr == NULL) {
        free(element);
        return NULL;
    }

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
    
    element = MsvgNewGenericElement(EID_DEFS, father);
    if (element == NULL) return NULL;
    
    element->pdefsattr = calloc(1, sizeof(MsvgDefsAttributes));
    if (element->pdefsattr == NULL) {
        free(element);
        return NULL;
    }
    
    return element;
}

static MsvgElement *MsvgNewGElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_G, father);
    if (element == NULL) return NULL;
    
    element->pgattr = calloc(1, sizeof(MsvgGAttributes));
    if (element->pgattr == NULL) {
        free(element);
        return NULL;
    }
    
    return element;
}

static MsvgElement *MsvgNewUseElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_USE, father);
    if (element == NULL) return NULL;
    
    element->puseattr = calloc(1, sizeof(MsvgUseAttributes));
    if (element->puseattr == NULL) {
        free(element);
        return NULL;
    }
    
    element->puseattr->refel = NULL;
    element->puseattr->x = 0;
    element->puseattr->y = 0;

    return element;
}

static MsvgElement *MsvgNewRectElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_RECT, father);
    if (element == NULL) return NULL;
    
    element->prectattr = calloc(1, sizeof(MsvgRectAttributes));
    if (element->prectattr == NULL) {
        free(element);
        return NULL;
    }
    
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
    
    element = MsvgNewGenericElement(EID_CIRCLE, father);
    if (element == NULL) return NULL;
    
    element->pcircleattr = calloc(1, sizeof(MsvgCircleAttributes));
    if (element->pcircleattr == NULL) {
        free(element);
        return NULL;
    }
    
    element->pcircleattr->cx = 0;
    element->pcircleattr->cy = 0;
    element->pcircleattr->r = 0;
    
    return element;
}

static MsvgElement *MsvgNewEllipseElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_ELLIPSE, father);
    if (element == NULL) return NULL;
    
    element->pellipseattr = calloc(1, sizeof(MsvgEllipseAttributes));
    if (element->pellipseattr == NULL) {
        free(element);
        return NULL;
    }
    
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
    
    element = MsvgNewGenericElement(EID_LINE, father);
    if (element == NULL) return NULL;
    
    element->plineattr = calloc(1, sizeof(MsvgLineAttributes));
    if (element->plineattr == NULL) {
        free(element);
        return NULL;
    }
    
    element->plineattr->x1 = 0;
    element->plineattr->y1 = 0;
    element->plineattr->x2 = 0;
    element->plineattr->y2 = 0;
    
    return element;
}

static MsvgElement *MsvgNewPolylineElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_POLYLINE, father);
    if (element == NULL) return NULL;
    
    element->ppolylineattr = calloc(1, sizeof(MsvgPolylineAttributes));
    if (element->ppolylineattr == NULL) {
        free(element);
        return NULL;
    }
    
    element->ppolylineattr->points = NULL;
    element->ppolylineattr->npoints = 0;
    
    return element;
}

static MsvgElement *MsvgNewPolygonElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_POLYGON, father);
    if (element == NULL) return NULL;
    
    element->ppolygonattr = calloc(1, sizeof(MsvgPolygonAttributes));
    if (element->ppolygonattr == NULL) {
        free(element);
        return NULL;
    }
    
    element->ppolygonattr->points = NULL;
    element->ppolygonattr->npoints = 0;
    
    return element;
}

static MsvgElement *MsvgNewPathElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_PATH, father);
    if (element == NULL) return NULL;
    
    element->ppathattr = calloc(1, sizeof(MsvgPathAttributes));
    if (element->ppathattr == NULL) {
        free(element);
        return NULL;
    }
    
    return element;
}

static MsvgElement *MsvgNewTextElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_TEXT, father);
    if (element == NULL) return NULL;
    
    element->ptextattr = calloc(1, sizeof(MsvgTextAttributes));
    if (element->ptextattr == NULL) {
        free(element);
        return NULL;
    }

    element->ptextattr->x = 0;
    element->ptextattr->y = 0;

    return element;
}

static MsvgElement *MsvgNewVContentElement(MsvgElement *father)
{
    MsvgElement *element;
    
    element = MsvgNewGenericElement(EID_V_CONTENT, father);
    if (element == NULL) return NULL;
    
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

