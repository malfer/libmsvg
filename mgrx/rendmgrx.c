/* rendmgrx.c ---- functions to render svg trees using MGRX library
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <mgrx.h>
#include <msvg.h>
#include "rendmgrx.h"
#include "pathmgrx.h"

static double glob_vb_min_x;
static double glob_vb_min_y;
static double glob_scale_x;
static double glob_scale_y;
static double glob_thick1 = 1;
static double glob_xorg;
static double glob_yorg;
static GrColor glob_bg;

static void get_icoord(int *x, int *y, double dx, double dy)
{
    *x = (dx - glob_vb_min_x) / glob_scale_x + 0.5 + glob_xorg;
    *y = (dy - glob_vb_min_y) / glob_scale_y + 0.5 + glob_yorg;
}

static void DrawRectElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->prectattr->x, el->prectattr->y);
    get_icoord(&x2, &y2,
               el->prectattr->x+el->prectattr->width,
               el->prectattr->y+el->prectattr->height);

    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrFilledBox(x1, y1, x2, y2, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomBox(x1, y1, x2, y2, &lopt);
        } else {
            GrBox(x1, y1, x2, y2, cstroke);
        }
    } 
}

static void DrawCircleElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int cx, cy, rx, ry;

    get_icoord(&cx, &cy, el->pcircleattr->cx, el->pcircleattr->cy);
    rx = el->pcircleattr->r / glob_scale_x + 0.5;
    ry = el->pcircleattr->r / glob_scale_y + 0.5;
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrFilledEllipse(cx, cy, rx, ry, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomEllipse(cx, cy, rx, ry, &lopt);
        } else {
            GrEllipse(cx, cy, rx, ry, cstroke);
        }
    } 
}

static void DrawEllipseElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int points[GR_MAX_ELLIPSE_POINTS][2];
    int npoints;
    int i, x, y;
    double rx, ry, d1, d2;
    double rotang, sinang, cosang;
    int icx, icy, irx, iry;
    double min_scale, precalx, precaly;

    d1 = el->pellipseattr->rx_x - el->pellipseattr->cx;
    d2 = el->pellipseattr->rx_y - el->pellipseattr->cy;
    rotang = atan2(d2, d1);
    //printf("rotang %g\n", rotang);
    rx = sqrt(d1 * d1 + d2 * d2);
    d1 = el->pellipseattr->ry_x - el->pellipseattr->cx;
    d2 = el->pellipseattr->ry_y - el->pellipseattr->cy;
    ry = sqrt(d1 * d1 + d2 * d2);

    get_icoord(&icx, &icy, el->pellipseattr->cx, el->pellipseattr->cy);
    min_scale = (glob_scale_x > glob_scale_y) ? glob_scale_y : glob_scale_x;
    irx = rx / min_scale + 0.5;
    iry = ry / min_scale + 0.5;
    //printf("%g %g   %g %g   %d %d\n", rx, ry, glob_scale_x, glob_scale_y, irx, iry);
    
    npoints = GrGenerateEllipse(icx, icy, irx, iry, points);

    sinang = sin(-rotang);
    cosang = cos(-rotang);
    precalx = min_scale / glob_scale_x;
    precaly = min_scale / glob_scale_y;
    for (i=0; i<npoints; i++) {
        x = points[i][0] - icx;
        y = points[i][1] - icy;
        points[i][0] = (cosang * x + sinang * y) * precalx + icx + 0.5;
        points[i][1] = (-sinang * x + cosang * y) * precaly + icy + 0.5;
    }

    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrFilledPolygon(npoints, points, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        //printf("%g %g %d\n", el->pctx.stroke_width, glob_thick1, istroke_width);
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomPolygon(npoints, points, &lopt);
        } else {
            GrPolygon(npoints, points, cstroke);
        }
    }
}

static void DrawLineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->plineattr->x1, el->plineattr->y1);
    get_icoord(&x2, &y2, el->plineattr->x2, el->plineattr->y2);

    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomLine(x1, y1, x2, y2, &lopt);
        } else {
            GrLine(x1, y1, x2, y2, cstroke);
        }
    } 
}

static void DrawPolylineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int i, npoints, (*points)[2];
    
    npoints = el->ppolylineattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i][0]), &(points[i][1]),
                   el->ppolylineattr->points[i*2],
                   el->ppolylineattr->points[i*2+1]);
    }
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrFilledPolygon(npoints, points, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomPolyLine(npoints, points, &lopt);
        } else {
            GrPolyLine(npoints, points, cstroke);
        }
    } 
    free(points);
}

static void DrawPolygonElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    int i, npoints, (*points)[2];
    
    npoints = el->ppolygonattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i][0]), &(points[i][1]),
                   el->ppolygonattr->points[i*2],
                   el->ppolygonattr->points[i*2+1]);
    }
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrFilledPolygon(npoints, points, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        //printf("%g %g %d\n", el->pctx.stroke_width, glob_thick1, istroke_width);
        if (istroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = istroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrCustomPolygon(npoints, points, &lopt);
        } else {
            GrPolygon(npoints, points, cstroke);
        }
    }
    free(points);
}

static void DrawPathElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill, rcfill;
    GrColor cstroke;
    GrLineOption lopt;
    int istroke_width;
    MsvgSubPath *sp;
    GrPath *gp;
    GrExpPointArray *pa;
    int x, y, i;
    int changercfill = 0;

    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        rcfill = cfill;
        changercfill = 1;
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        istroke_width = pctx->stroke_width * glob_thick1 + 0.5;
        lopt.lno_color = cstroke;
        lopt.lno_width = istroke_width;
        lopt.lno_pattlen = 0;
        lopt.lno_dashpat = NULL;
    }

    sp = el->ppathattr->sp;
    while (sp) {
        gp = GrNewPath(sp->npoints);
        if (gp) {
            for (i=0; i< sp->npoints; i++) {
                get_icoord(&x, &y, sp->spp[i].x, sp->spp[i].y);
                GrAddPointToPath(gp, sp->spp[i].cmd, x, y);
            }
            gp->closed = sp->closed;
            pa = GrPathToExpPointArray2(gp);
            if (pa) {
                if (pctx->fill != NO_COLOR) {
                    GrFilledPolygon(pa->npoints, pa->points, rcfill);
                }
                if (pctx->stroke != NO_COLOR) {
                    if (pa->closed) {
                        if (istroke_width > 1)
                            GrCustomPolygon(pa->npoints, pa->points, &lopt);
                        else
                            GrPolygon(pa->npoints, pa->points, cstroke);
                    } else {
                        if (istroke_width > 1)
                            GrCustomPolyLine(pa->npoints, pa->points, &lopt);
                        else
                            GrPolyLine(pa->npoints, pa->points, cstroke);
                    }
                }
                GrDestroyExpPointArray(pa);
            }
            GrDestroyPath(gp);
        }
        if (changercfill) {
            rcfill = glob_bg;
            changercfill = 0;
        }
        sp = sp->next;
    }
}

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;
    //MsvgElement *newel2;
    //int i, nsp;

    //printf("id %s\n", el->id);
    //MsvgPrintCookedElement(stdout, el);
    newel = MsvgTransformCookedElement(el, pctx);
    if (newel == NULL) return;
    //printf("after ");
    //MsvgPrintCookedElement(stdout, newel);

    switch (newel->eid) {
        case EID_RECT :
            DrawRectElement(newel, newel->ppctx);
            break;
        case EID_CIRCLE :
            DrawCircleElement(newel, newel->ppctx);
            break;
        case EID_ELLIPSE :
            DrawEllipseElement(newel, newel->ppctx);
            break;
        case EID_LINE :
            DrawLineElement(newel, newel->ppctx);
            break;
        case EID_POLYLINE :
            DrawPolylineElement(newel, newel->ppctx);
            break;
        case EID_POLYGON :
            DrawPolygonElement(newel, newel->ppctx);
            break;
        case EID_PATH :
            DrawPathElement(newel, newel->ppctx);
            /*nsp = MsvgCountSubPaths(newel->ppathattr->sp);
            for (i=0; i<nsp; i++) {
                newel2 = MsvgPathEltoPolyEl(newel, i, 4);
                if (newel2) {
                    if (newel2->eid == EID_POLYGON)
                        DrawPolygonElement(newel2, &(newel2->pctx));
                    else if (newel2->eid == EID_POLYLINE)
                        DrawPolylineElement(newel2, &(newel2->pctx));
                    MsvgDeleteElement(newel2);
                }
            }*/
            break;
        default :
            break;
    }

    MsvgDeleteElement(newel);
}

int GrDrawSVGtree(MsvgElement *root, GrSVGDrawMode *sdm)
{
    GrColor cfill;
    double ratiow, ratioh, rvb_width, rvb_height;
    double old_width, old_height, zoom;

    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;

    rvb_width = root->psvgattr->vb_width;
    rvb_height = root->psvgattr->vb_height;

    ratiow = GrSizeX() / rvb_width;
    ratioh = GrSizeY() / rvb_height;

    glob_xorg = 0;
    glob_yorg = 0;
    
    zoom = sdm->zoom;

    switch (sdm->mode) {
        case SVGDRAWMODE_FIT :
            if (ratiow > ratioh)
                glob_thick1 = (GrSizeY() / rvb_height) * zoom;
            else
                glob_thick1 = (GrSizeX() / rvb_width) * zoom;
            glob_scale_x = (rvb_width / zoom) / GrSizeX();
            glob_scale_y = (rvb_height / zoom) / GrSizeY();
            if (sdm->adj == SVGDRAWADJ_CENTER) {
                glob_xorg = (GrSizeX() - GrSizeX() * zoom) / 2;
                glob_yorg = (GrSizeY() - GrSizeY() * zoom) / 2;
            } else if (sdm->adj == SVGDRAWADJ_RIGHT) {
                glob_xorg = (GrSizeX() - GrSizeX() * zoom);
                glob_yorg = (GrSizeY() - GrSizeY() * zoom);
            }
            break;
        case SVGDRAWMODE_PAR :
            old_width = rvb_width;
            old_height = rvb_height;
            if (ratiow > ratioh)
                rvb_width = (GrSizeX() * rvb_height) / GrSizeY();
            else
                rvb_height = (GrSizeY() * rvb_width) / GrSizeX();
            glob_thick1 = (GrSizeX() / rvb_width) * zoom;
            glob_scale_x = (rvb_width / zoom) / GrSizeX();
            glob_scale_y = (rvb_height / zoom) / GrSizeY();
            if (sdm->adj == SVGDRAWADJ_CENTER) {
                glob_xorg = ((rvb_width / zoom - old_width) /
                         (rvb_width / zoom) / 2) * GrSizeX();
                glob_yorg = ((rvb_height  / zoom - old_height) /
                         (rvb_height / zoom) / 2) * GrSizeY();
            } else if (sdm->adj == SVGDRAWADJ_RIGHT) {
                glob_xorg = ((rvb_width / zoom - old_width) /
                         (rvb_width / zoom)) * GrSizeX();
                glob_yorg = ((rvb_height  / zoom - old_height) /
                         (rvb_height / zoom)) * GrSizeY();
            }
            break;
        case SVGDRAWMODE_SCOORD :
            glob_thick1 = 1.0 * zoom;
            glob_scale_x = 1 / zoom;
            glob_scale_y = 1 / zoom;
            if (sdm->adj == SVGDRAWADJ_CENTER) {
                glob_xorg = (GrSizeX() - rvb_width * zoom) / 2;
                glob_yorg = (GrSizeY() - rvb_height * zoom) / 2;
            } else if (sdm->adj == SVGDRAWADJ_RIGHT) {
                glob_xorg = (GrSizeX() - rvb_width * zoom);
                glob_yorg = (GrSizeY() - rvb_height * zoom);
            }
            break;
        default:
            return 0;
    }

    glob_vb_min_x = root->psvgattr->vb_min_x;
    glob_vb_min_y = root->psvgattr->vb_min_y;

    glob_xorg += sdm->xdespl;
    glob_yorg += sdm->ydespl;

    // check if possible integer overflow (it happens if zoom is a big value)
    if (((rvb_width/glob_scale_x)+glob_xorg) > (INT_MAX/2) ||
        ((rvb_height/glob_scale_y)+glob_yorg) > (INT_MAX/2) ||
        glob_xorg < (INT_MIN/2) || glob_yorg < (INT_MIN/2)) {
        printf("Possible overflow %g %g   %g %g\n", glob_xorg, glob_yorg,
               (rvb_width/glob_scale_x)+glob_xorg,
               (rvb_height/glob_scale_y)+glob_yorg);
        return 0;
    }
    
    glob_bg = sdm->bg;
    if (root->psvgattr->vp_fill != NO_COLOR) {
        cfill = GrAllocColor2(root->psvgattr->vp_fill);
        GrClearContext(cfill);
        glob_bg = cfill;
    } else if (sdm->bg != NO_COLOR) {
        GrClearContext(sdm->bg);
    }

    if (!MsvgSerCookedTree(root, sufn, NULL)) return 0;

    return 1;
}

int GrDrawSVGtreeUsingDB(MsvgElement *root, GrSVGDrawMode *sdm)
{
    GrContext *grc, grcaux;
    int ret;

    if( (grc = GrCreateContext(GrSizeX(), GrSizeY(), NULL, NULL)) == NULL )
        return 0;

    GrSaveContext(&grcaux);
    GrSetContext(grc);
    ret = GrDrawSVGtree(root, sdm);
    GrSetContext(&grcaux);
    GrBitBlt(&grcaux, 0, 0, grc, 0, 0, grc->gc_xmax, grc->gc_ymax, GrWRITE);
    GrDestroyContext(grc);

    return ret;
}
