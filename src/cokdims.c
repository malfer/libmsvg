/* cokdims.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2022-2023 Mariano Alvarez Fernandez
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

static void iniboxmaxmin(MsvgBox *box)
{
    box->gminx = 1e9;
    box->gmaxx = -1e9;
    box->gminy = 1e9;
    box->gmaxy = -1e9;
}

static void setboxmaxmin(MsvgBox *box, double dx, double dy)
{
    if (dx < box->gminx) box->gminx = dx;
    if (dx > box->gmaxx) box->gmaxx = dx;
    if (dy < box->gminy) box->gminy = dy;
    if (dy > box->gmaxy) box->gmaxy = dy;
}

int MsvgGetCookedBoundingBox(MsvgElement *el, MsvgBox *box, int inibox)
{
    MsvgSubPath *sp;
    int i, dx, dy;

    if (inibox) iniboxmaxmin(box);

    switch (el->eid) {
        case EID_RECT :
            setboxmaxmin(box, el->prectattr->x, el->prectattr->y);
            setboxmaxmin(box, el->prectattr->x+el->prectattr->width,
                      el->prectattr->y+el->prectattr->height);
            break;
        case EID_CIRCLE :
            setboxmaxmin(box, el->pcircleattr->cx-el->pcircleattr->r,
                      el->pcircleattr->cy-el->pcircleattr->r);
            setboxmaxmin(box, el->pcircleattr->cx+el->pcircleattr->r,
                      el->pcircleattr->cy+el->pcircleattr->r);
            break;
        case EID_ELLIPSE :
            dx = (el->pellipseattr->rx_x - el->pellipseattr->cx) * 2;
            dy = (el->pellipseattr->ry_y - el->pellipseattr->cy) * 2;
            setboxmaxmin(box, el->pellipseattr->rx_x, el->pellipseattr->ry_y);
            setboxmaxmin(box, el->pellipseattr->rx_x-dx, el->pellipseattr->ry_y-dy);
            break;
        case EID_LINE :
            setboxmaxmin(box, el->plineattr->x1, el->plineattr->y1);
            setboxmaxmin(box, el->plineattr->x2, el->plineattr->y2);
            break;
        case EID_POLYLINE :
            for (i=0; i< el->ppolylineattr->npoints; i++) {
                setboxmaxmin(box, el->ppolylineattr->points[i*2],
                          el->ppolylineattr->points[i*2+1]);
            }
            break;
        case EID_POLYGON :
            for (i=0; i< el->ppolygonattr->npoints; i++) {
                setboxmaxmin(box, el->ppolygonattr->points[i*2],
                          el->ppolygonattr->points[i*2+1]);
            }
            break;
        case EID_PATH :
            // TODO do it right
            sp = el->ppathattr->sp;
            while (sp) {
                for (i=0; i<sp->npoints; i++) {
                    setboxmaxmin(box, sp->spp[i].x, sp->spp[i].y);
                }
                sp = sp->next;
            }
            break;
        case EID_TEXT :
            // TODO do it right
            setboxmaxmin(box, el->ptextattr->x, el->ptextattr->y);
            break;
        default :
            return 0;
    }
    return 1;
}

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;
    MsvgBox *box;

    box = (MsvgBox *)udata;
    
    newel = MsvgTransformCookedElement(el, pctx, 0);
    if (newel == NULL) return;

    MsvgGetCookedBoundingBox(newel, box, 0);

    MsvgDeleteElement(newel);
}

int MsvgGetCookedDims(MsvgElement *root, double *minx, double *maxx,
                      double *miny, double *maxy)
{
    // make a rough estimation of the svg dimensions in a cooked tree
    int ret;
    MsvgBox box = {1e9, -1e9, 1e9, -1e9};
    
    ret = MsvgSerCookedTree(root, sufn, &box, 0);
    if (ret) {
        *minx = box.gminx;
        *maxx = box.gmaxx;
        *miny = box.gminy;
        *maxy = box.gmaxy;
        return 1;
    } else {
        return 0;
    }
}
