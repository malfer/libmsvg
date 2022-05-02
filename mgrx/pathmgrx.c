/* pathmgrx.c ---- functions to render svg trees using MGRX library
 * 
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020-2022 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * This source is free software; you can redistribute it and/or
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mgrx.h>
#include "pathmgrx.h"

#define POINTSEP 8
#define MAX_BEZPOINTS 1000

GrPath * GrNewPath(int maxpoints)
{
    GrPath *gp;

    if (maxpoints <= 0) maxpoints = 32;

    gp = malloc(sizeof(GrPath));
    if (gp == NULL) return NULL;
    gp->pp = malloc(sizeof(GrPathPoint)*maxpoints);
    if (gp->pp == NULL) {
        free(gp);
        return NULL;
    }

    gp->maxpoints = maxpoints;
    gp->npoints = 0;
    gp->closed = 0;
    gp->failed_realloc = 0;

    return gp;
}

void GrExpandPath(GrPath *gp)
{
    GrPathPoint *newpp;
    int newmaxpoints;

    newmaxpoints = gp->maxpoints * 2;
    newpp = realloc(gp->pp, sizeof(GrPathPoint)*newmaxpoints);
    if (newpp == NULL) {
        gp->failed_realloc = 1;
        return;
    }

    gp->maxpoints = newmaxpoints;
    gp->pp = newpp;
}

void GrAddPointToPath(GrPath *gp, char cmd, int x, int y)
{
    if (gp->npoints >= gp->maxpoints) {
        if (gp->failed_realloc) return;
        GrExpandPath(gp);
        if (gp->failed_realloc) return;
    }

    gp->pp[gp->npoints].cmd = cmd;
    gp->pp[gp->npoints].x = x;
    gp->pp[gp->npoints].y = y;
    gp->npoints++;
}

void GrDestroyPath(GrPath *gp)
{
    if (gp == NULL) return;
    free(gp->pp);
    free(gp);
}

GrExpPointArray * GrNewExpPointArray(int maxpoints)
{
    GrExpPointArray *pa;

    if (maxpoints <= 0) maxpoints = 32;

    pa = malloc(sizeof(GrExpPointArray));
    if (pa == NULL) return NULL;
    pa->points = malloc(sizeof(int)*2*maxpoints);
    if (pa->points == NULL) {
        free(pa);
        return NULL;
    }

    pa->maxpoints = maxpoints;
    pa->npoints = 0;
    pa->closed = 0;
    pa->failed_realloc = 0;

    return pa;
}

void GrExpandExpPointArray(GrExpPointArray *pa)
{
    int (*newpoints)[2];
    int newmaxpoints;

    newmaxpoints = pa->maxpoints * 2;
    newpoints = realloc(pa->points, sizeof(int)*2*newmaxpoints);
    if (newpoints == NULL) {
        pa->failed_realloc = 1;
        return;
    }

    pa->maxpoints = newmaxpoints;
    pa->points = newpoints;
}

void GrAddPointToExpPointArray(GrExpPointArray *pa, int x, int y, int notequal)
{
    if (notequal && pa->npoints > 0) {
        if (x == pa->points[pa->npoints-1][0] &&
            y == pa->points[pa->npoints-1][1]) return;
    }
        
    if (pa->npoints >= pa->maxpoints) {
        if (pa->failed_realloc) return;
        GrExpandExpPointArray(pa);
        if (pa->failed_realloc) return;
    }

    pa->points[pa->npoints][0] = x;
    pa->points[pa->npoints][1] = y;
    pa->npoints++;
}

void GrDestroyExpPointArray(GrExpPointArray *pa)
{
    if (pa == NULL) return;
    free(pa->points);
    free(pa);
}


/**************************/

int GrReducePoints(int numpts, int points[][2])
{
    int i, ind1, ind2;
    int rnumpts, desp;

    if (numpts < 2) return numpts;

    rnumpts = numpts;
    ind1 = 1;
    while (ind1 < rnumpts) {
        ind2 = ind1;
        while (ind2 < rnumpts) {
            if ((points[ind2][0] != points[ind2-1][0]) ||
                (points[ind2][1] != points[ind2-1][1])) break;
            ind2++;
        }
        if (ind2 > ind1) {
            desp = ind2 - ind1;
            for (i=ind1; i<rnumpts-desp; i++) {
                points[i][0] = points[i+desp][0];
                points[i][1] = points[i+desp][1];
            }
            rnumpts -= desp;
        }
        ind1++;
    }
    return rnumpts;
}

static void GrGenQBezier(GrPath *gp, int pos, GrExpPointArray *pa)
{
    int numpts, xorg, yorg, xpc, ypc, xend, yend;
    int ax, bx, ay, by, x, y;
    float t, dt, tSquared;
    int i;

    xorg = gp->pp[pos-1].x;
    yorg = gp->pp[pos-1].y;
    xpc = gp->pp[pos].x;
    ypc = gp->pp[pos].y;
    xend = gp->pp[pos+1].x;
    yend = gp->pp[pos+1].y;

    numpts = (abs(xorg - xpc) + abs(yorg - ypc) +
              abs(xpc - xend) + abs(ypc - yend)) / POINTSEP;
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
        x = ((ax * tSquared) + (bx * t) + xorg) + 0.5;
        y = ((ay * tSquared) + (by * t) + yorg) + 0.5;
        GrAddPointToExpPointArray(pa, x, y, 1);
    }
    GrAddPointToExpPointArray(pa, xend, yend, 1);
}

static void GrGenCBezier(GrPath *gp, int pos, GrExpPointArray *pa)
{
    int numpts, xorg, yorg, xpc1, ypc1, xpc2, ypc2, xend, yend;
    int ax, bx, cx, ay, by, cy, x, y;
    float t, dt, tSquared, tCubed;
    int i;

    xorg = gp->pp[pos-1].x;
    yorg = gp->pp[pos-1].y;
    xpc1 = gp->pp[pos].x;
    ypc1 = gp->pp[pos].y;
    xpc2 = gp->pp[pos+1].x;
    ypc2 = gp->pp[pos+1].y;
    xend = gp->pp[pos+2].x;
    yend = gp->pp[pos+2].y;

    numpts = (abs(xorg - xpc1) + abs(yorg - ypc1) +
              abs(xpc1 - xpc2) + abs(ypc1 - ypc2) +
              abs(xpc2 - xend) + abs(ypc2 - yend)) / POINTSEP;
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
        x = ((ax * tCubed) + (bx * tSquared) + (cx * t) + xorg) + 0.5;
        y = ((ay * tCubed) + (by * tSquared) + (cy * t) + yorg) + 0.5;
        GrAddPointToExpPointArray(pa, x, y, 1);
    }
    GrAddPointToExpPointArray(pa, xend, yend, 1);
}

GrExpPointArray * GrPathToExpPointArray(GrPath *gp)
{
    GrExpPointArray *pa;
    int i;

    if (gp->npoints < 2) return NULL;

    pa = GrNewExpPointArray(gp->npoints*2);
    if (pa == NULL) return NULL;

    GrAddPointToExpPointArray(pa, gp->pp[0].x, gp->pp[0].y, 0);
    for (i=1; i<gp->npoints; i++) {
        if (gp->pp[i].cmd == 'L') {
            GrAddPointToExpPointArray(pa, gp->pp[i].x, gp->pp[i].y, 1);
        } else if (gp->pp[i].cmd == 'Q') {
            GrGenQBezier(gp, i, pa);
        } else if (gp->pp[i].cmd == 'C') {
            GrGenCBezier(gp, i, pa);
        }
    }

    pa->closed = gp->closed;

    return pa;
}

#define min(x,y)    (((x) < (y)) ?  (x) : (y))
#define max(x,y)    (((x) > (y)) ?  (x) : (y))

static int indrawarea(GrPathPoint *pp, int np)
{
    int i, minx, maxx, miny, maxy;

    if (np < 2) return 0;

    minx = min(pp[0].x, pp[1].x);
    miny = min(pp[0].y, pp[1].y);
    maxx = max(pp[0].x, pp[1].x);
    maxy = max(pp[0].y, pp[1].y);

    for (i=2; i < np; i++) {
        minx = min(pp[i].x, minx);
        miny = min(pp[i].y, miny);
        maxx = max(pp[i].x, maxx);
        maxy = max(pp[i].y, maxy);
    }

    if (minx > GrHighX() || maxx < GrLowX() || miny > GrHighY() || maxy < GrLowY()) {
        return 0;
    }

    return 1;
}

GrExpPointArray * GrPathToExpPointArray2(GrPath *gp)
{
    // This version doesn't calculate Bezier points it they are out of the
    // mgrx current drawing cliparea, so it is faster
    GrExpPointArray *pa;
    int i;
    //static int maxpoints = 0;

    if (gp->npoints < 2) return NULL;

    pa = GrNewExpPointArray(gp->npoints*2);
    if (pa == NULL) return NULL;

    GrAddPointToExpPointArray(pa, gp->pp[0].x, gp->pp[0].y, 0);
    for (i=1; i<gp->npoints; i++) {
        if (gp->pp[i].cmd == 'L') {
            GrAddPointToExpPointArray(pa, gp->pp[i].x, gp->pp[i].y, 1);
        } else if (gp->pp[i].cmd == 'Q') {
            if (indrawarea(&(gp->pp[i-1]), 3)) {
                GrGenQBezier(gp, i, pa);
            } else {
                GrAddPointToExpPointArray(pa, gp->pp[i+1].x, gp->pp[i+1].y, 1);
            }
        } else if (gp->pp[i].cmd == 'C') {
            if (indrawarea(&(gp->pp[i-1]), 4)) {
                GrGenCBezier(gp, i, pa);
                //if (gp->npoints == 40) printf("in %d %d  %d %d\n",
                //    gp->pp[i-1].x, gp->pp[i-1].y, gp->pp[i+2].x, gp->pp[i+2].y);
            } else {
                GrAddPointToExpPointArray(pa, gp->pp[i+2].x, gp->pp[i+2].y, 1);
                //if (gp->npoints == 40) printf("out %d %d  %d %d\n",
                //    gp->pp[i-1].x, gp->pp[i-1].y, gp->pp[i+2].x, gp->pp[i+2].y);
            }
        }
    }

    pa->closed = gp->closed;

    //if (pa->npoints > maxpoints) {
    //    maxpoints = pa->npoints;
    //    printf("GrPathToExpPointArray2: %d %d\n", gp->npoints, pa->npoints);
    //}
    return pa;
}
