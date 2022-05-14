/* cokdims.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2022 Mariano Alvarez Fernandez
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
#include "msvg.h"

typedef struct {
    double gminx, gmaxx, gminy, gmaxy;
} UserData;

static void setmaxmin(UserData *ud, double dx, double dy)
{
    if (dx < ud->gminx) ud->gminx = dx;
    if (dx > ud->gmaxx) ud->gmaxx = dx;
    if (dy < ud->gminy) ud->gminy = dy;
    if (dy > ud->gmaxy) ud->gmaxy = dy;
}

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;
    MsvgSubPath *sp;
    int i;
    UserData *ud;

    ud = (UserData *)udata;
    
    newel = MsvgTransformCookedElement(el, pctx);
    if (newel == NULL) return;

    switch (newel->eid) {
        case EID_RECT :
            setmaxmin(ud, newel->prectattr->x, newel->prectattr->y);
            setmaxmin(ud, newel->prectattr->x+newel->prectattr->width,
                      newel->prectattr->y+newel->prectattr->height);
            break;
        case EID_CIRCLE :
            setmaxmin(ud, newel->pcircleattr->cx-newel->pcircleattr->r,
                      newel->pcircleattr->cy-newel->pcircleattr->r);
            setmaxmin(ud, newel->pcircleattr->cx+newel->pcircleattr->r,
                      newel->pcircleattr->cy+newel->pcircleattr->r);
            break;
        case EID_ELLIPSE :
            setmaxmin(ud, newel->pellipseattr->rx_x, newel->pellipseattr->rx_y);
            setmaxmin(ud, newel->pellipseattr->ry_x, newel->pellipseattr->ry_y);
            break;
        case EID_LINE :
            setmaxmin(ud, newel->plineattr->x1, newel->plineattr->y1);
            setmaxmin(ud, newel->plineattr->x2, newel->plineattr->y2);
            break;
        case EID_POLYLINE :
            for (i=0; i< newel->ppolylineattr->npoints; i++) {
                setmaxmin(ud, newel->ppolylineattr->points[i*2],
                          newel->ppolylineattr->points[i*2+1]);
            }
            break;
        case EID_POLYGON :
            for (i=0; i< newel->ppolygonattr->npoints; i++) {
                setmaxmin(ud, newel->ppolygonattr->points[i*2],
                          newel->ppolygonattr->points[i*2+1]);
            }
            break;
        case EID_PATH :
            sp = newel->ppathattr->sp;
            while (sp) {
                for (i=0; i<sp->npoints; i++) {
                    setmaxmin(ud, sp->spp[i].x, sp->spp[i].y);
                }
                sp = sp->next;
            }
            break;
        case EID_TEXT :
            setmaxmin(ud, newel->ptextattr->x, newel->ptextattr->y);
            break;
        default :
            break;
    }

    MsvgDeleteElement(newel);
}

int MsvgGetCookedDims(MsvgElement *root, double *minx, double *maxx,
                      double *miny, double *maxy)
{
    // make a rough estimation of the svg dimensions in a cooked tree
    int ret;
    UserData ud = {1e9, -1e9, 1e9, -1e9};
    
    ret = MsvgSerCookedTree(root, sufn, &ud);
    if (ret) {
        *minx = ud.gminx;
        *maxx = ud.gmaxx;
        *miny = ud.gminy;
        *maxy = ud.gmaxy;
        return 1;
    } else {
        return 0;
    }
}
