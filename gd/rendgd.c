/* rendgd.c ---- functions to render svg trees using GD library
 * 
 * This is a dirty hack to test the libmsvg librarie with the GD
 * graphics library. It is NOT part of the libmsvg librarie really.
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
#include <math.h>
#include <limits.h>

#include <gd.h>
#include <msvg.h>
#include "rendgd.h"
//#include "pathgd.h"

static double glob_xorg;
static double glob_yorg;
static double glob_px_x_unit;
static int glob_bg;
static gdImagePtr glob_im;

static void get_icoord(int *x, int *y, double dx, double dy)
{
    *x = dx + 0.5 + glob_xorg;
    *y = dy + 0.5 + glob_yorg;
}

static void DrawRectElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    int cfill;
    int cstroke;
    int istroke_width;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->prectattr->x, el->prectattr->y);
    get_icoord(&x2, &y2,
               el->prectattr->x+el->prectattr->width,
               el->prectattr->y+el->prectattr->height);

    if (pctx->fill != NO_COLOR) {
        cfill = pctx->fill;
        gdImageFilledRectangle(glob_im, x1, y1, x2, y2, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = pctx->stroke;
        istroke_width = pctx->stroke_width + 0.5;
        if (istroke_width < 1) istroke_width = 1;
        gdImageSetThickness(glob_im, istroke_width);
        gdImageRectangle(glob_im, x1, y1, x2, y2, cstroke);
     } 
}

static void DrawCircleElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
}

static void DrawEllipseElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
}

static void DrawLineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    int cstroke;
    int istroke_width;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->plineattr->x1, el->plineattr->y1);
    get_icoord(&x2, &y2, el->plineattr->x2, el->plineattr->y2);

    if (pctx->stroke != NO_COLOR) {
        cstroke = pctx->stroke;
        istroke_width = pctx->stroke_width + 0.5;
        if (istroke_width < 1) istroke_width = 1;
        gdImageLine(glob_im, x1, y1, x2, y2, cstroke);
    } 
}

static void DrawPolylineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    int cfill;
    int cstroke;
    int istroke_width;
    int i, npoints;
    gdPointPtr points;

    npoints = el->ppolylineattr->npoints;
    points = calloc(npoints, sizeof(gdPoint));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i].x), &(points[i].y),
                   el->ppolylineattr->points[i*2],
                   el->ppolylineattr->points[i*2+1]);
    }

    if (pctx->fill != NO_COLOR) {
        cfill = pctx->fill;
        gdImageFilledPolygon(glob_im, points, npoints, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = pctx->stroke;
        istroke_width = pctx->stroke_width + 0.5;
        if (istroke_width < 1) istroke_width = 1;
        gdImageSetThickness(glob_im, istroke_width);
        gdImageOpenPolygon(glob_im, points, npoints, cstroke);
    } 
    free(points);
}

static void DrawPolygonElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    int cfill;
    int cstroke;
    int istroke_width;
    int i, npoints;
    gdPointPtr points;

    npoints = el->ppolygonattr->npoints;
    points = calloc(npoints, sizeof(gdPoint));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i].x), &(points[i].y),
                   el->ppolylineattr->points[i*2],
                   el->ppolylineattr->points[i*2+1]);
    }

    if (pctx->fill != NO_COLOR) {
        cfill = pctx->fill;
        gdImageFilledPolygon(glob_im, points, npoints, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = pctx->stroke;
        istroke_width = pctx->stroke_width + 0.5;
        if (istroke_width < 1) istroke_width = 1;
        gdImageSetThickness(glob_im, istroke_width);
        gdImagePolygon(glob_im, points, npoints, cstroke);
    }
    free(points);
}
/*
static int InsidePolygonTest(int npoints, double *points, double x, double y)
{
    int i, ytest0, ytest1, xtest0, inside;
    double *edge0, *edge1;

    if (npoints < 3) return 0;

    edge0 = &points[npoints*2-2];
    edge1 = &points[0];
    ytest0 = (edge0[1] >= y);
    inside = 0;

    for (i=0; i<npoints; i++) {
        ytest1 = (edge1[1] >= y);

        if (ytest0 != ytest1) {
            xtest0 = (edge0[0] >= x);
            if (xtest0 == (edge1[0] >= x)) {
                if (xtest0) inside = !inside;
            } else {
                if ((edge1[0] - (edge1[1] - y) *
                    (edge0[0] - edge1[0]) / (edge0[1] - edge1[1])) >= x) {
                    inside = !inside;
                }
            }
        }

        ytest0 = ytest1;
        edge0 = edge1;
        edge1 += 2;
    }

    return inside;
}
*/
static void DrawPathElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    MsvgElement *newel2;
    int i, nsp;

    nsp = MsvgCountSubPaths(el->ppathattr->sp);
    for (i=0; i<nsp; i++) {
        newel2 = MsvgSubPathToPoly(el, i, glob_px_x_unit);
        if (newel2) {
            if (newel2->eid == EID_POLYGON)
                DrawPolygonElement(newel2, newel2->pctx);
            else if (newel2->eid == EID_POLYLINE)
                DrawPolylineElement(newel2, newel2->pctx);
            MsvgDeleteElement(newel2);
        }
    }
}

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;

    #define GDMODE (MSVGTCE_CIR2PATH | MSVGTCE_ELL2PATH)
    newel = MsvgTransformCookedElement(el, pctx, GDMODE);
    if (newel == NULL) return;

    // I don't know how to draw gradients with GD, so at least
    // draw something
    if (newel->pctx->fill == IRI_COLOR) newel->pctx->fill = SILVER_COLOR;
    if (newel->pctx->stroke == IRI_COLOR) newel->pctx->stroke = GRAY_COLOR;

    switch (newel->eid) {
        case EID_RECT :
            DrawRectElement(newel, newel->pctx);
            break;
        case EID_CIRCLE :
            DrawCircleElement(newel, newel->pctx);
            break;
        case EID_ELLIPSE :
            DrawEllipseElement(newel, newel->pctx);
            break;
        case EID_LINE :
            DrawLineElement(newel, newel->pctx);
            break;
        case EID_POLYLINE :
            DrawPolylineElement(newel, newel->pctx);
            break;
        case EID_POLYGON :
            DrawPolygonElement(newel, newel->pctx);
            break;
        case EID_PATH :
            DrawPathElement(newel, newel->pctx);
            break;
        default :
            break;
    }

    MsvgDeleteElement(newel);
}

int GDDrawSVGtree(MsvgElement *root, GDSVGDrawMode *sdm, gdImagePtr im)
{
    int cfill;
    double ratiow, ratioh, rvb_width, rvb_height;
    int ret;
    double cx, cy;
    TMatrix taux1, taux2, taux3, tsave;
    double scale_x, scale_y;

    if (root == NULL) return -1;
    if (root->eid != EID_SVG) return -2;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return -3;

    glob_im = im;

    rvb_width = root->psvgattr->vb_width;
    rvb_height = root->psvgattr->vb_height;

    // check for possible integer overflow (it happens if zoom is a big value)
    if ((rvb_width* sdm->zoom) > (INT_MAX/2) ||
        (rvb_height* sdm->zoom) > (INT_MAX/2)) {
        //printf("Possible overflow %g %g\n", rvb_width* sdm->zoom, rvb_height* sdm->zoom);
        return -4;
    }

    switch (sdm->mode) {
        case SVGDRAWMODE_FIT :
            scale_x = rvb_width / gdImageSX(im);
            scale_y = rvb_height / gdImageSY(im);
            break;
        case SVGDRAWMODE_PAR :
            ratiow = gdImageSX(im) / rvb_width;
            ratioh = gdImageSY(im) / rvb_height;
            if (ratiow > ratioh) {
                scale_x = rvb_height / gdImageSY(im);
                scale_y = rvb_height / gdImageSY(im);
            } else {
                scale_x = rvb_width / gdImageSX(im);
                scale_y = rvb_width / gdImageSX(im);
            }
            break;
        case SVGDRAWMODE_SCOORD :
            scale_x = 1;
            scale_y = 1;
            break;
        default:
            return -5;
    }

    //glob_px_x_unit = 4;
    glob_px_x_unit = sdm->zoom / scale_x;
    //printf("scale_x %f  scale_y  %f px_unit %f\n", scale_x, scale_y, glob_px_x_unit);

    if (sdm->adj == SVGDRAWADJ_LEFT) {
        glob_xorg = 0;
        glob_yorg = 0;
    } else if (sdm->adj == SVGDRAWADJ_CENTER) {
        glob_xorg = (gdImageSX(im) - rvb_width * sdm->zoom / scale_x) / 2;
        glob_yorg = (gdImageSY(im) - rvb_height * sdm->zoom / scale_y) / 2;
    } else if (sdm->adj == SVGDRAWADJ_RIGHT) {
        glob_xorg = (gdImageSX(im) - rvb_width * sdm->zoom / scale_x);
        glob_yorg = (gdImageSY(im) - rvb_height * sdm->zoom / scale_y);
    } else
        return -5;

    glob_bg = sdm->bg;
    if (root->psvgattr->vp_fill != NO_COLOR) {
        cfill = root->psvgattr->vp_fill;
        gdImageFilledRectangle(glob_im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, cfill);
        glob_bg = cfill;
    } else if (sdm->bg != NO_COLOR) {
        gdImageFilledRectangle(glob_im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, sdm->bg);
    }

    TMSetTranslation(&taux1, sdm->xdespl, sdm->ydespl);
    TMSetScaling(&taux2, sdm->zoom / scale_x, sdm->zoom / scale_y);
    TMMpy(&taux3, &taux1, &taux2);

    cx = root->psvgattr->vb_width / 2;
    cy = root->psvgattr->vb_height / 2;
    TMSetRotation(&taux2, sdm->rotang, cx, cy);
    TMMpy(&taux1, &taux3, &taux2);

    TMSetTranslation(&taux2, -root->psvgattr->vb_min_x, -root->psvgattr->vb_min_y);
    TMMpy(&taux3, &taux1, &taux2);

    tsave = root->pctx->tmatrix;
    TMMpy(&(root->pctx->tmatrix), &taux3, &tsave);

    ret = MsvgSerCookedTree(root, sufn, NULL, 0);
    root->pctx->tmatrix = tsave;
    if (ret != 1) return -6;

    return 0;
}
