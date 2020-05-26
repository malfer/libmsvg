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
#include <math.h>
#include "msvg.h"
#include "util.h"

static MsvgElement * transCookEllipse(MsvgElement *el, MsvgPaintCtx *cpctx);
static MsvgElement * transCookPolygon(MsvgElement *el, MsvgPaintCtx *cpctx);
static MsvgElement * transCookPath(MsvgElement *el, MsvgPaintCtx *cpctx);

static void setElPctx(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    TMatrix *t;

    el->pctx = *cpctx;
    if (el->pctx.stroke_width > 0) {
        t = &(cpctx->tmatrix);
        el->pctx.stroke_width *= sqrt(t->a*t->a + t->b*t->b);
    }
    TMSetIdentity(&(el->pctx.tmatrix));
}

static MsvgElement * transCookRect(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel, *auxel;
    MsvgSubPath *sp;
    double zerox = 0, zeroy = 0;
    int x, y, w, h, rx, ry;
    int rounded;

    rounded = (el->prectattr->rx != 0) || (el->prectattr->ry != 0);

    if (!TMHaveRotation(&(cpctx->tmatrix)) && !rounded) {

        newel = MsvgNewElement(EID_RECT, NULL);
        if (newel == NULL) return NULL;

        setElPctx(newel, cpctx);
        *(newel->prectattr) = *(el->prectattr);

        if (TMIsIdentity(&(cpctx->tmatrix))) return newel;
    
        TMTransformCoord(&(newel->prectattr->x), &(newel->prectattr->y),
                     &(cpctx->tmatrix));
        TMTransformCoord(&zerox, &zeroy, &(cpctx->tmatrix));
        TMTransformCoord(&(newel->prectattr->width), &(newel->prectattr->height),
                         &(cpctx->tmatrix));
        newel->prectattr->width -= zerox;
        newel->prectattr->height -= zeroy;
        /*TMTransformCoord(&(newel->prectattr->rx), &(newel->prectattr->ry),
                         &(cpctx->tmatrix));
        newel->prectattr->rx -= zerox;
        newel->prectattr->ry -= zeroy;*/

        return newel;

    } else if (!rounded) {
        // because the rect is rotated, treat it as a polygon

        auxel = MsvgNewElement(EID_POLYGON, NULL);
        if (auxel == NULL) return NULL;

        auxel->ppolygonattr->npoints = 4;
        auxel->ppolygonattr->points = (double *)calloc(8, sizeof(double));
        if (auxel->ppolygonattr->points == NULL) {
            MsvgDeleteElement(auxel);
            return NULL;
        }
        x = el->prectattr->x;
        y = el->prectattr->y;
        w = el->prectattr->width;
        h = el->prectattr->height;
        auxel->ppolygonattr->points[0] = x;
        auxel->ppolygonattr->points[1] = y;
        auxel->ppolygonattr->points[2] = x + w;
        auxel->ppolygonattr->points[3] = y;
        auxel->ppolygonattr->points[4] = x + w;
        auxel->ppolygonattr->points[5] = y + h;
        auxel->ppolygonattr->points[6] = x;
        auxel->ppolygonattr->points[7] = y + h;

        newel = transCookPolygon(auxel, cpctx);

        MsvgDeleteElement(auxel);
    
        return newel;

    } else {
        // build a path element, easier to be streched and rotated
        #define KAPPA90 0.5522847493

        auxel = MsvgNewElement(EID_PATH, NULL);
        if (auxel == NULL) return NULL;

        sp = MsvgNewSubPath(20);
        if (sp == NULL) {
            MsvgDeleteElement(auxel);
            return NULL;
        }
        auxel->ppathattr->sp = sp;
        x = el->prectattr->x;
        y = el->prectattr->y;
        w = el->prectattr->width;
        h = el->prectattr->height;
        rx = el->prectattr->rx;
        ry = el->prectattr->ry;
        MsvgAddPointToSubPath(sp, 'M', x+rx, y);
        MsvgAddPointToSubPath(sp, 'L', x+w-rx, y);
        MsvgAddPointToSubPath(sp, 'C', x+w-rx*(1-KAPPA90), y);
        MsvgAddPointToSubPath(sp, ' ', x+w, y+ry*(1-KAPPA90));
        MsvgAddPointToSubPath(sp, ' ', x+w, y+ry);
        MsvgAddPointToSubPath(sp, 'L', x+w, y+h-ry);
        MsvgAddPointToSubPath(sp, 'C', x+w, y+h-ry*(1-KAPPA90));
        MsvgAddPointToSubPath(sp, ' ', x+w-rx*(1-KAPPA90), y+h);
        MsvgAddPointToSubPath(sp, ' ', x+w-rx, y+h);
        MsvgAddPointToSubPath(sp, 'L', x+rx, y+h);
        MsvgAddPointToSubPath(sp, 'C', x+rx*(1-KAPPA90), y+h);
        MsvgAddPointToSubPath(sp, ' ', x, y+h-ry*(1-KAPPA90));
        MsvgAddPointToSubPath(sp, ' ', x, y+h-ry);
        MsvgAddPointToSubPath(sp, 'L', x, y+ry);
        MsvgAddPointToSubPath(sp, 'C', x, y+ry*(1-KAPPA90));
        MsvgAddPointToSubPath(sp, ' ', x+rx*(1-KAPPA90), y);
        MsvgAddPointToSubPath(sp, ' ', x+rx, y);
        sp->closed = 1;
            
        newel = transCookPath(auxel, cpctx);

        MsvgDeleteElement(auxel);
    
        return newel;
    }
}

static MsvgElement * transCookCircle(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel, *auxel;

    if (TMIsIdentity(&(cpctx->tmatrix))) {;
        newel = MsvgNewElement(EID_CIRCLE, NULL);
        if (newel == NULL) return NULL;
        setElPctx(newel, cpctx);
        *(newel->pcircleattr) = *(el->pcircleattr);
        return newel;
    }

    // because the circle can be streched, we treat it as an ellipse
    
    auxel = MsvgNewElement(EID_ELLIPSE, NULL);
    if (auxel == NULL) return NULL;

    auxel->pellipseattr->cx = el->pcircleattr->cx;
    auxel->pellipseattr->cy = el->pcircleattr->cy;
    auxel->pellipseattr->rx_x = el->pcircleattr->r + el->pcircleattr->cx;
    auxel->pellipseattr->rx_y = el->pcircleattr->cy;
    auxel->pellipseattr->ry_x = el->pcircleattr->cx;
    auxel->pellipseattr->ry_y = el->pcircleattr->r + el->pcircleattr->cy;

    newel = transCookEllipse(auxel, cpctx);

    MsvgDeleteElement(auxel);
    
    return newel;
}

static MsvgElement * transCookEllipse(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel;
    
    newel = MsvgNewElement(EID_ELLIPSE, NULL);
    if (newel == NULL) return NULL;

    setElPctx(newel, cpctx);
    *(newel->pellipseattr) = *(el->pellipseattr);

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;

    TMTransformCoord(&(newel->pellipseattr->cx), &(newel->pellipseattr->cy),
                     &(cpctx->tmatrix));
    TMTransformCoord(&(newel->pellipseattr->rx_x), &(newel->pellipseattr->rx_y),
                     &(cpctx->tmatrix));
    TMTransformCoord(&(newel->pellipseattr->ry_x), &(newel->pellipseattr->ry_y),
                     &(cpctx->tmatrix));

    return newel;
}

static MsvgElement * transCookLine(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel;

    newel = MsvgNewElement(EID_LINE, NULL);
    if (newel == NULL) return NULL;

    setElPctx(newel, cpctx);
    *(newel->plineattr) = *(el->plineattr);

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;
    
    TMTransformCoord(&(newel->plineattr->x1), &(newel->plineattr->y1),
                     &(cpctx->tmatrix));
    TMTransformCoord(&(newel->plineattr->x2), &(newel->plineattr->y2),
                     &(cpctx->tmatrix));

    return newel;
}

static MsvgElement * transCookPolyline(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel;
    int i;

    newel = MsvgNewElement(EID_POLYLINE, NULL);
    if (newel == NULL) return NULL;
    if (!MsvgAllocPointsToPolylineElement(newel, el->ppolylineattr->npoints)) {
        MsvgDeleteElement(newel);
        return NULL;
    }

    setElPctx(newel, cpctx);
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

    return newel;
}

static MsvgElement * transCookPolygon(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel;
    int i;

    newel = MsvgNewElement(EID_POLYGON, NULL);
    if (newel == NULL) return NULL;
    if (!MsvgAllocPointsToPolygonElement(newel, el->ppolygonattr->npoints)) {
        MsvgDeleteElement(newel);
        return NULL;
    }

    setElPctx(newel, cpctx);
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

    return newel;
}

static MsvgElement * transCookPath(MsvgElement *el, MsvgPaintCtx *cpctx)
{
    MsvgElement *newel;
    MsvgSubPath *sp;
    int i;

    newel = MsvgNewElement(EID_PATH, NULL);
    if (newel == NULL) return NULL;
    newel->ppathattr->sp = MsvgDupSubPath(el->ppathattr->sp);
    if (newel->ppathattr->sp == NULL) {
        MsvgDeleteElement(newel);
        return NULL;
    }

    setElPctx(newel, cpctx);

    if (TMIsIdentity(&(cpctx->tmatrix))) return newel;

    sp = newel->ppathattr->sp;
    while (sp) {
        for (i=0; i<sp->npoints; i++) {
            TMTransformCoord(&(sp->spp[i].x), &(sp->spp[i].y), &(cpctx->tmatrix));
        }
        sp = sp->next;
    }

    return newel;
}

MsvgElement * MsvgTransformCookedElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    switch (el->eid) {
        case EID_RECT :
            return transCookRect(el, pctx);
        case EID_CIRCLE :
            return transCookCircle(el, pctx);
        case EID_ELLIPSE :
            return transCookEllipse(el, pctx);
        case EID_LINE :
            return transCookLine(el, pctx);
        case EID_POLYLINE :
            return transCookPolyline(el, pctx);
        case EID_POLYGON :
            return transCookPolygon(el, pctx);
        case EID_PATH :
            return transCookPath(el, pctx);
        default :
            return NULL;
    }
}

MsvgElement * MsvgPathEltoPolyEl(MsvgElement *el, int nsp)
{
    MsvgElement *newel;
    MsvgSubPath *sp;
    int i, ind, nrpoints;

    if (el->eid != EID_PATH) return NULL;

    sp = el->ppathattr->sp;
    for (i=0; i < nsp; i++) {
        if (sp == NULL) return NULL;
        sp = sp->next;
    }
    if (sp == NULL) return NULL;

    nrpoints = 0;
    for(i=0; i<sp->npoints; i++) {
        if (sp->spp[i].cmd != ' ') nrpoints++;
    }

    if (sp->closed) {
        newel = MsvgNewElement(EID_POLYGON, NULL);
        if (newel == NULL) return NULL;
        newel->ppolygonattr->npoints = nrpoints;
        newel->ppolygonattr->points = (double *)calloc(nrpoints*2, sizeof(double));
        if (newel->ppolygonattr->points == NULL) {
            MsvgDeleteElement(newel);
            return NULL;
        }
        newel->pctx = el->pctx;
        ind = 0;
        for(i=0; i<sp->npoints; i++) {
            switch (sp->spp[i].cmd) {
                case 'M' :
                case 'L' :
                    newel->ppolygonattr->points[ind++] = sp->spp[i].x;
                    newel->ppolygonattr->points[ind++] = sp->spp[i].y;
                    break;
                case 'C' :
                    newel->ppolygonattr->points[ind++] = sp->spp[i+2].x;
                    newel->ppolygonattr->points[ind++] = sp->spp[i+2].y;
                    break;
                case 'Q' :
                    newel->ppolygonattr->points[ind++] = sp->spp[i+1].x;
                    newel->ppolygonattr->points[ind++] = sp->spp[i+1].y;
                    break;
            }
        }
    } else {
        newel = MsvgNewElement(EID_POLYLINE, NULL);
        if (newel == NULL) return NULL;
        newel->ppolylineattr->npoints = nrpoints;
        newel->ppolylineattr->points = (double *)calloc(nrpoints*2, sizeof(double));
        if (newel->ppolylineattr->points == NULL) {
            MsvgDeleteElement(newel);
            return NULL;
        }
        newel->pctx = el->pctx;
        ind = 0;
        for(i=0; i<sp->npoints; i++) {
            switch (sp->spp[i].cmd) {
                case 'M' :
                case 'L' :
                    newel->ppolylineattr->points[ind++] = sp->spp[i].x;
                    newel->ppolylineattr->points[ind++] = sp->spp[i].y;
                    break;
                case 'C' :
                    newel->ppolylineattr->points[ind++] = sp->spp[i+2].x;
                    newel->ppolylineattr->points[ind++] = sp->spp[i+2].y;
                    break;
                case 'Q' :
                    newel->ppolylineattr->points[ind++] = sp->spp[i+1].x;
                    newel->ppolylineattr->points[ind++] = sp->spp[i+1].y;
                    break;
            }
        }
    }

    return newel;
}
