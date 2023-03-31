/* bpserver.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2023 Mariano Alvarez Fernandez
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

MsvgBPServer *MsvgNewBPServer(MsvgElement *el)
{
    MsvgBPServer *bpser;
    MsvgBGradientStops *bstops;
    MsvgElement *nson;
    int nst = 0;

    if (el->eid != EID_LINEARGRADIENT &&
        el->eid != EID_RADIALGRADIENT) return NULL;

    nson = el->fson;
    while (nson) {
        if (nson->eid == EID_STOP) {
            nst++;
        }
        nson = nson->nsibling;
    }

    if (nst < 2) return NULL;
    if (nst > BGRADIENT_MAXSTOPS) nst = BGRADIENT_MAXSTOPS;

    bpser = calloc(1, sizeof(MsvgBPServer));
    if (bpser == NULL) return NULL;

    if (el->eid == EID_LINEARGRADIENT) {
        bpser->type = BPSERVER_LINEARGRADIENT;
        bpser->blg.gradunits = el->plgradattr->gradunits;
        bpser->blg.x1 = el->plgradattr->x1;
        bpser->blg.y1 = el->plgradattr->y1;
        bpser->blg.x2 = el->plgradattr->x2;
        bpser->blg.y2 = el->plgradattr->y2;
        bstops = &(bpser->blg.stops);
    } else if (el->eid == EID_RADIALGRADIENT) {
        bpser->type = BPSERVER_RADIALGRADIENT;
        bpser->brg.gradunits = el->prgradattr->gradunits;
        bpser->brg.cx = el->prgradattr->cx;
        bpser->brg.cy = el->prgradattr->cy;
        bpser->brg.r = el->prgradattr->r;
        bstops = &(bpser->brg.stops);
    } else {
        free(bpser);
        return NULL;
    }

    bstops->nstops = 0;
    nson = el->fson;
    while (nson) {
        if (nson->eid == EID_STOP) {
            if (bstops->nstops < nst) {
                bstops->offset[bstops->nstops] = nson->pstopattr->offset;
                bstops->sopacity[bstops->nstops] = nson->pstopattr->sopacity;
                bstops->scolor[bstops->nstops] = nson->pstopattr->scolor;
                bstops->nstops++;
            }
        }
        nson = nson->nsibling;
    }

    return bpser;
}

void MsvgDestroyBPServer(MsvgBPServer *bps)
{
    free(bps);
}

int MsvgCalcUnitsBPServer(MsvgBPServer *bps, MsvgBox *bbox, TMatrix *t)
{
    double w, h, r;

    if (bps->type == BPSERVER_LINEARGRADIENT) {
        if (bps->blg.gradunits == GRADUNIT_BBOX) {
            if (bbox) {
                w = bbox->gmaxx - bbox->gminx;
                bps->blg.x1 *= w;
                bps->blg.x1 += bbox->gminx;
                bps->blg.x2 *= w;
                bps->blg.x2 += bbox->gminx;
                h = bbox->gmaxy - bbox->gminy;
                bps->blg.y1 *= h;
                bps->blg.y1 += bbox->gminy;
                bps->blg.y2 *= h;
                bps->blg.y2 += bbox->gminy;
                bps->blg.gradunits = GRADUNIT_USER;
            }
        }
        if (t) {
            TMTransformCoord(&(bps->blg.x1), &(bps->blg.y1), t);
            TMTransformCoord(&(bps->blg.x2), &(bps->blg.y2), t);
        }
        return 1;
    } else if (bps->type == BPSERVER_RADIALGRADIENT) {
        if (bps->brg.gradunits == GRADUNIT_BBOX) {
            if (bbox) {
                w = bbox->gmaxx - bbox->gminx;
                bps->brg.cx *= w;
                bps->brg.cx += bbox->gminx;
                h = bbox->gmaxy - bbox->gminy;
                bps->brg.cy *= h;
                bps->brg.cy += bbox->gminy;
                r = (w > h) ? w : h;
                bps->brg.r *= r;
                bps->brg.gradunits = GRADUNIT_USER;
            }
        }
        if (t) {
            TMTransformCoord(&(bps->brg.cx), &(bps->brg.cy), t);
            bps->brg.r *= sqrt(t->a*t->a + t->b*t->b);
        }
        return 1;
    }

    return 0;
}
