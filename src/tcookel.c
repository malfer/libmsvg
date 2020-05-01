/* tcookel.c
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

#include <stdlib.h>
#include <string.h>
#include "msvg.h"
#include "util.h"

static MsvgElement * transCookEllipse(MsvgElement *el, MsvgPaintCtx *cpctx, int bef);
static MsvgElement * transCookPolygon(MsvgElement *el, MsvgPaintCtx *cpctx, int bef);

static MsvgElement * transCookRect(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel, *auxel;
    double zerox = 0, zeroy = 0, w, h;

    if (!TMHaveRotation(&(cpctx->tmatrix))) {

        newel = MsvgNewElement(EID_RECT, NULL);
        if (newel == NULL) return NULL;

        newel->pctx =*cpctx;
        *(newel->prectattr) = *(el->prectattr);

        if (TMIsIdentity(&(cpctx->tmatrix))) return newel;
    
        TMTransformCoord(&(newel->prectattr->x), &(newel->prectattr->y),
                     &(cpctx->tmatrix));
        TMTransformCoord(&zerox, &zeroy, &(cpctx->tmatrix));
        TMTransformCoord(&(newel->prectattr->width), &(newel->prectattr->height),
                         &(cpctx->tmatrix));
        newel->prectattr->width -= zerox;
        newel->prectattr->height -= zeroy;
        TMTransformCoord(&(newel->prectattr->rx), &(newel->prectattr->ry),
                         &(cpctx->tmatrix));
        newel->prectattr->rx -= zerox;
        newel->prectattr->ry -= zeroy;
        TMSetIdentity(&(newel->pctx.tmatrix));

        return newel;

    } else {
        // because the rect is rotated, treat it as a polygon

        // TODO: manage rx, ry
        if (!bef && (el->prectattr->rx != 0 || el->prectattr->ry != 0))
            return NULL;

        auxel = MsvgNewElement(EID_POLYGON, NULL);
        if (auxel == NULL) return NULL;

        auxel->ppolygonattr->npoints = 4;
        auxel->ppolygonattr->points = (double *)calloc(8, sizeof(double));
        if (auxel->ppolygonattr->points == NULL) {
            MsvgDeleteElement(auxel);
            return NULL;
        }
        w = el->prectattr->width - 1;
        h = el->prectattr->height - 1;
        auxel->ppolygonattr->points[0] = el->prectattr->x;
        auxel->ppolygonattr->points[1] = el->prectattr->y;
        auxel->ppolygonattr->points[2] = el->prectattr->x + w;
        auxel->ppolygonattr->points[3] = el->prectattr->y;
        auxel->ppolygonattr->points[4] = el->prectattr->x + w;
        auxel->ppolygonattr->points[5] = el->prectattr->y + h;
        auxel->ppolygonattr->points[6] = el->prectattr->x;
        auxel->ppolygonattr->points[7] = el->prectattr->y + h;

        newel = transCookPolygon(auxel, cpctx, bef);

        MsvgDeleteElement(auxel);
    
        return newel;
    }
}

static MsvgElement * transCookCircle(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel, *auxel;

    if (TMIsIdentity(&(cpctx->tmatrix))) {;
        newel = MsvgNewElement(EID_CIRCLE, NULL);
        if (newel == NULL) return NULL;
        newel->pctx =*cpctx;
        *(newel->pcircleattr) = *(el->pcircleattr);
        return newel;
    }

    // because the circle can be streched, we treat it as an ellipse
    
    auxel = MsvgNewElement(EID_ELLIPSE, NULL);
    if (auxel == NULL) return NULL;

    auxel->pellipseattr->cx = el->pcircleattr->cx;
    auxel->pellipseattr->cy = el->pcircleattr->cy;
    auxel->pellipseattr->rx = el->pcircleattr->r;
    auxel->pellipseattr->rx = el->pcircleattr->r;

    newel = transCookEllipse(auxel, cpctx, bef);

    MsvgDeleteElement(auxel);
    
    return newel;
}

static MsvgElement * transCookEllipse(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel;
    double zerox = 0, zeroy = 0;

    // TODO: manage rotations, converting to polygon?
    if (!bef && TMHaveRotation(&(cpctx->tmatrix)))
        return NULL;
    
    newel = MsvgNewElement(EID_ELLIPSE, NULL);
    if (newel == NULL) return NULL;

    newel->pctx =*cpctx;
    *(newel->pellipseattr) = *(el->pellipseattr);

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;
    
    TMTransformCoord(&(newel->pellipseattr->cx), &(newel->pellipseattr->cy),
                     &(cpctx->tmatrix));
    TMTransformCoord(&zerox, &zeroy, &(cpctx->tmatrix));
    TMTransformCoord(&(newel->pellipseattr->rx), &(newel->pellipseattr->ry),
                     &(cpctx->tmatrix));
    newel->pellipseattr->rx -= zerox;
    newel->pellipseattr->ry -= zeroy;
    TMSetIdentity(&(newel->pctx.tmatrix));

    return newel;
}

static MsvgElement * transCookLine(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel;

    newel = MsvgNewElement(EID_LINE, NULL);
    if (newel == NULL) return NULL;

    newel->pctx =*cpctx;
    *(newel->plineattr) = *(el->plineattr);

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;
    
    TMTransformCoord(&(newel->plineattr->x1), &(newel->plineattr->y1),
                     &(cpctx->tmatrix));
    TMTransformCoord(&(newel->plineattr->x2), &(newel->plineattr->y2),
                     &(cpctx->tmatrix));
    TMSetIdentity(&(newel->pctx.tmatrix));

    return newel;
}

static MsvgElement * transCookPolyline(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel;
    int i;

    newel = MsvgNewElement(EID_POLYLINE, NULL);
    if (newel == NULL) return NULL;
    if (!MsvgAllocPointsToPolylineElement(newel, el->ppolylineattr->npoints)) {
        MsvgDeleteElement(newel);
        return NULL;
    }

    newel->pctx =*cpctx;
    newel->ppolylineattr->npoints = el->ppolylineattr->npoints;
    for (i=0; i< newel->ppolylineattr->npoints; i++) {
        newel->ppolylineattr->points[i*2] = el->ppolylineattr->points[i*2];
        newel->ppolylineattr->points[i*2+1] = el->ppolylineattr->points[i*2+1];
    }
    
    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;

    for (i=0; i< newel->ppolylineattr->npoints; i++) {
        TMTransformCoord(&(newel->ppolylineattr->points[i*2]),
                         &(newel->ppolylineattr->points[i*2+1]),
                        &(cpctx->tmatrix));
    }
    TMSetIdentity(&(newel->pctx.tmatrix));

    return newel;
}

static MsvgElement * transCookPolygon(MsvgElement *el, MsvgPaintCtx *cpctx, int bef)
{
    MsvgElement *newel;
    int i;

    newel = MsvgNewElement(EID_POLYGON, NULL);
    if (newel == NULL) return NULL;
    if (!MsvgAllocPointsToPolygonElement(newel, el->ppolygonattr->npoints)) {
        MsvgDeleteElement(newel);
        return NULL;
    }

    newel->pctx =*cpctx;
    newel->ppolygonattr->npoints = el->ppolygonattr->npoints;
    for (i=0; i< newel->ppolygonattr->npoints; i++) {
        newel->ppolygonattr->points[i*2] = el->ppolygonattr->points[i*2];
        newel->ppolygonattr->points[i*2+1] = el->ppolygonattr->points[i*2+1];
    }

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;

    for (i=0; i< newel->ppolygonattr->npoints; i++) {
        TMTransformCoord(&(newel->ppolygonattr->points[i*2]),
                         &(newel->ppolygonattr->points[i*2+1]),
                        &(cpctx->tmatrix));
    }
    TMSetIdentity(&(newel->pctx.tmatrix));

    return newel;
}

MsvgElement * MsvgTransformCookedElement(MsvgElement *el, MsvgPaintCtx *pctx, int bef)
{
    switch (el->eid) {
        case EID_RECT :
            return transCookRect(el, pctx, bef);
        case EID_CIRCLE :
            return transCookCircle(el, pctx, bef);
        case EID_ELLIPSE :
            return transCookEllipse(el, pctx, bef);
        case EID_LINE :
            return transCookLine(el, pctx, bef);
        case EID_POLYLINE :
            return transCookPolyline(el, pctx, bef);
        case EID_POLYGON :
            return transCookPolygon(el, pctx, bef);
        default :
            return NULL;
    }
}
