/* path2ply.c
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

typedef struct {
    int maxpoints;           // max capacity (realloc if necesary)
    int npoints;             // actual number of points
    int failed_realloc;      // 1 = yes, 0 = no
    double *points;          // Points array
} ExpPointArray;

#define POINTSEP 8
#define MAX_BEZPOINTS 1000

static ExpPointArray * NewExpPointArray(int maxpoints)
{
    ExpPointArray *pa;

    if (maxpoints <= 0) maxpoints = 32;

    pa = malloc(sizeof(ExpPointArray));
    if (pa == NULL) return NULL;
    pa->points = malloc(sizeof(double)*2*maxpoints);
    if (pa->points == NULL) {
        free(pa);
        return NULL;
    }

    pa->maxpoints = maxpoints;
    pa->npoints = 0;
    pa->failed_realloc = 0;

    return pa;
}

static void ExpandExpPointArray(ExpPointArray *pa)
{
    double *newpoints;
    int newmaxpoints;

    newmaxpoints = pa->maxpoints * 2;
    newpoints = realloc(pa->points, sizeof(double)*2*newmaxpoints);
    if (newpoints == NULL) {
        pa->failed_realloc = 1;
        return;
    }

    pa->maxpoints = newmaxpoints;
    pa->points = newpoints;
}

static void AddPointToExpPointArray(ExpPointArray *pa, double x, double y)
{
    if (pa->npoints >= pa->maxpoints) {
        if (pa->failed_realloc) return;
        ExpandExpPointArray(pa);
        if (pa->failed_realloc) return;
    }

    pa->points[pa->npoints*2] = x;
    pa->points[pa->npoints*2+1] = y;
    pa->npoints++;
}

static void DestroyExpPointArray(ExpPointArray *pa)
{
    if (pa == NULL) return;
    free(pa->points);
    free(pa);
}

static void GenQBezier(MsvgSubPath *sp, int pos, ExpPointArray *pa, double px_x_unit)
{
    int numpts;
    double xorg, yorg, xpc, ypc, xend, yend;
    double ax, bx, ay, by, x, y;
    double t, dt, tSquared;
    int i;

    xorg = sp->spp[pos-1].x;
    yorg = sp->spp[pos-1].y;
    xpc = sp->spp[pos].x;
    ypc = sp->spp[pos].y;
    xend = sp->spp[pos+1].x;
    yend = sp->spp[pos+1].y;

    numpts = (fabs(xorg - xpc) + fabs(yorg - ypc) +
              fabs(xpc - xend) + fabs(ypc - yend)) * px_x_unit / POINTSEP;
    if (numpts < 3) numpts = 3;
    if (numpts > MAX_BEZPOINTS) numpts = MAX_BEZPOINTS;

    // calcula los coeficientes polinomiales
    bx = -2 * xorg + 2 * xpc;
    ax = xorg - 2 * xpc + xend;
    by = -2 * yorg + 2 * ypc;
    ay = yorg - 2 * ypc + yend;

    dt = 1.0 / (numpts - 1);
    for( i = 1; i < numpts-1; i++) {
        t = i * dt;
        tSquared = t * t;
        x = (ax * tSquared) + (bx * t) + xorg;
        y = (ay * tSquared) + (by * t) + yorg;
        AddPointToExpPointArray(pa, x, y);
    }
    AddPointToExpPointArray(pa, xend, yend);
}

static void GenCBezier(MsvgSubPath *sp, int pos, ExpPointArray *pa, double px_x_unit)
{
    int numpts;
    double xorg, yorg, xpc1, ypc1, xpc2, ypc2, xend, yend;
    double ax, bx, cx, ay, by, cy, x, y;
    double t, dt, tSquared, tCubed;
    int i;

    xorg = sp->spp[pos-1].x;
    yorg = sp->spp[pos-1].y;
    xpc1 = sp->spp[pos].x;
    ypc1 = sp->spp[pos].y;
    xpc2 = sp->spp[pos+1].x;
    ypc2 = sp->spp[pos+1].y;
    xend = sp->spp[pos+2].x;
    yend = sp->spp[pos+2].y;

    numpts = (fabs(xorg - xpc1) + fabs(yorg - ypc1) +
              fabs(xpc1 - xpc2) + fabs(ypc1 - ypc2) +
              fabs(xpc2 - xend) + fabs(ypc2 - yend)) * px_x_unit / POINTSEP;
    if (numpts < 3) numpts = 3;
    if (numpts > MAX_BEZPOINTS) numpts = MAX_BEZPOINTS;

    // calcula los coeficientes polinomiales
    cx = 3 * (xpc1 - xorg);
    bx = 3 * (xpc2 - xpc1) - cx;
    ax = xend - xorg - cx - bx;
    cy = 3 * (ypc1 - yorg);
    by = 3 * (ypc2 - ypc1) - cy;
    ay = yend - yorg - cy - by;
    
    dt = 1.0 / (numpts - 1);
    for( i = 1; i < numpts-1; i++) {
        t = i * dt;
        tSquared = t * t;
        tCubed = tSquared * t;
        x = (ax * tCubed) + (bx * tSquared) + (cx * t) + xorg;
        y = (ay * tCubed) + (by * tSquared) + (cy * t) + yorg;
        AddPointToExpPointArray(pa, x, y);
    }
    AddPointToExpPointArray(pa, xend, yend);
}

static ExpPointArray * PathToExpPointArray(MsvgSubPath *sp, double px_x_unit)
{
    ExpPointArray *pa;
    int i;

    if (sp->npoints < 2) return NULL;

    pa = NewExpPointArray(sp->npoints*2);
    if (pa == NULL) return NULL;

    AddPointToExpPointArray(pa, sp->spp[0].x, sp->spp[0].y);
    for (i=1; i<sp->npoints; i++) {
        if (sp->spp[i].cmd == 'L') {
            AddPointToExpPointArray(pa, sp->spp[i].x, sp->spp[i].y);
        } else if (sp->spp[i].cmd == 'Q') {
            GenQBezier(sp, i, pa, px_x_unit);
        } else if (sp->spp[i].cmd == 'C') {
            GenCBezier(sp, i, pa, px_x_unit);
        }
    }

    return pa;
}

MsvgElement * MsvgPathEltoPolyEl(MsvgElement *el, int nsp, double px_x_unit)
{
    MsvgElement *newel;
    MsvgSubPath *sp;
    ExpPointArray *pa;
    int i;

    if (el->eid != EID_PATH) return NULL;

    sp = el->ppathattr->sp;
    for (i=0; i < nsp; i++) {
        if (sp == NULL) return NULL;
        sp = sp->next;
    }
    if (sp == NULL) return NULL;

    pa = PathToExpPointArray(sp, px_x_unit);
    if (pa == NULL) return NULL;

    if (sp->closed) {
        newel = MsvgNewElement(EID_POLYGON, NULL);
        if (newel == NULL) {
            DestroyExpPointArray(pa);
            return NULL;
        }
        newel->ppolygonattr->npoints = pa->npoints;
        newel->ppolygonattr->points = pa->points;
        free(pa); // we don't free pa->points !!!!
        newel->pctx = el->pctx;
    } else {
        newel = MsvgNewElement(EID_POLYLINE, NULL);
        if (newel == NULL) {
            DestroyExpPointArray(pa);
            return NULL;
        }
        newel->ppolylineattr->npoints = pa->npoints;
        newel->ppolylineattr->points = pa->points;
        free(pa); // we don't free pa->points !!!!
        newel->pctx = el->pctx;
    }

    return newel;
}
