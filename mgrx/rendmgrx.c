/* rendmgrx.c ---- functions to render svg trees using MGRX library
 * 
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
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

#if MGRX_VERSION_API < 0x0135
    #error "MGRX version >= 1.35 is nedded, better >= 1.43"
#endif

typedef struct {
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    GrPattern *fill_grd;
    GrPattern *stroke_grd;
    GrLinePattern lpat;
    int istroke_width;
} RenderCtx;

static double glob_xorg;
static double glob_yorg;
static GrColor glob_bg;
static TMatrix glob_tuser;

static void get_icoord(int *x, int *y, double dx, double dy)
{
    *x = dx + 0.5 + glob_xorg;
    *y = dy + 0.5 + glob_yorg;
}

static GrPattern *convert_gradient(MsvgBPServer *bps)
{
    GrPattern *grd;
    MsvgBGradientStops *bstops;
    int xi, yi, xf, yf, i;

    if (bps->type == BPSERVER_LINEARGRADIENT) {
        get_icoord(&xi, &yi, bps->blg.x1, bps->blg.y1);
        get_icoord(&xf, &yf, bps->blg.x2, bps->blg.y2);
        grd = GrCreateLinGradient(xi, yi, xf, yf);
        bstops = &(bps->blg.stops);
    } else if (bps->type == BPSERVER_RADIALGRADIENT) {
        get_icoord(&xi, &yi, bps->brg.cx, bps->brg.cy);
        grd = GrCreateRadGradient(xi, yi, bps->brg.r);
        bstops = &(bps->brg.stops);
    } else {
        return NULL;
    }

    for (i=0; i<bstops->nstops; i++) {
        GrAddGradientStop(grd, bstops->offset[i]*255, GrAllocColor2(bstops->scolor[i]));
    }
    GrGenGradientColorTbl(grd);
    return grd;
}

static void build_renderctx(RenderCtx *r, MsvgPaintCtx *pctx)
{
    r->fill_grd = NULL;
    r->stroke_grd = NULL;

    if (pctx->fill != NO_COLOR) {
        if (pctx->fill_bps) {
            r->fill_grd = convert_gradient(pctx->fill_bps);
        } else {
            r->cfill = GrAllocColor2(pctx->fill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (pctx->stroke_bps) {
            r->stroke_grd = convert_gradient(pctx->stroke_bps);
            r->cstroke = GrBlack();
        } else {
            r->cstroke = GrAllocColor2(pctx->stroke);
        }
        r->istroke_width = pctx->stroke_width + 0.5;
        if (r->istroke_width < 1) r->istroke_width = 1;
        r->lopt.lno_color = r->cstroke;
        r->lopt.lno_width = r->istroke_width;
        r->lopt.lno_pattlen = 0;
        r->lopt.lno_dashpat = NULL;
        if (r->stroke_grd) {
            r->lpat.lnp_pattern = r->stroke_grd;
            r->lpat.lnp_option = &(r->lopt);
        }
    }
}

static void free_renderctx(RenderCtx *r)
{
    if (r->fill_grd) GrDestroyPattern(r->fill_grd);
    if (r->stroke_grd) GrDestroyPattern(r->stroke_grd);
}

static void DrawRectElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->prectattr->x, el->prectattr->y);
    get_icoord(&x2, &y2,
               el->prectattr->x+el->prectattr->width,
               el->prectattr->y+el->prectattr->height);
    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledBox(x1, y1, x2, y2, r.fill_grd);
        } else {
            GrFilledBox(x1, y1, x2, y2, r.cfill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedBox(x1, y1, x2, y2, &(r.lpat));
        } else {
            GrCustomBox(x1, y1, x2, y2, &(r.lopt));
        }
    }
    free_renderctx(&r);
}

static void DrawCircleElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int cx, cy, rx, ry;

    get_icoord(&cx, &cy, el->pcircleattr->cx, el->pcircleattr->cy);
    rx = el->pcircleattr->r + 0.5;
    ry = el->pcircleattr->r + 0.5;
    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledEllipse(cx, cy, rx, ry, r.fill_grd);
        } else {
            GrFilledEllipse(cx, cy, rx, ry, r.cfill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedEllipse(cx, cy, rx, ry, &(r.lpat));
        } else {
            GrCustomEllipse(cx, cy, rx, ry, &(r.lopt));
        }
    } 
    free_renderctx(&r);
}

static void DrawEllipseElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int points[GR_MAX_ELLIPSE_POINTS][2];
    int npoints;
    int i, x, y;
    double rx, ry, d1, d2;
    double rotang, sinang, cosang;
    int icx, icy, irx, iry;

    d1 = el->pellipseattr->rx_x - el->pellipseattr->cx;
    d2 = el->pellipseattr->rx_y - el->pellipseattr->cy;
    rotang = atan2(d2, d1);
    rx = sqrt(d1 * d1 + d2 * d2);
    d1 = el->pellipseattr->ry_x - el->pellipseattr->cx;
    d2 = el->pellipseattr->ry_y - el->pellipseattr->cy;
    ry = sqrt(d1 * d1 + d2 * d2);

    get_icoord(&icx, &icy, el->pellipseattr->cx, el->pellipseattr->cy);
    irx = rx  + 0.5;
    iry = ry  + 0.5;
    
    npoints = GrGenerateEllipse(icx, icy, irx, iry, points);

    sinang = sin(-rotang);
    cosang = cos(-rotang);
    for (i=0; i<npoints; i++) {
        x = points[i][0] - icx;
        y = points[i][1] - icy;
        points[i][0] = (cosang * x + sinang * y) + icx + 0.5;
        points[i][1] = (-sinang * x + cosang * y) + icy + 0.5;
    }
    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledPolygon(npoints, points, r.fill_grd);
        } else {
            GrFilledPolygon(npoints, points, r.cfill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedPolygon(npoints, points, &(r.lpat));
        } else {
            GrCustomPolygon(npoints, points, &(r.lopt));
        }
    }
    free_renderctx(&r);
}

static void DrawLineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int x1, y1, x2, y2;

    get_icoord(&x1, &y1, el->plineattr->x1, el->plineattr->y1);
    get_icoord(&x2, &y2, el->plineattr->x2, el->plineattr->y2);
    build_renderctx(&r, pctx);

    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedLine(x1, y1, x2, y2, &(r.lpat));
        } else {
            GrCustomLine(x1, y1, x2, y2, &(r.lopt));
        }
    }
    free_renderctx(&r);
}

static void DrawPolylineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int i, npoints, (*points)[2];
    
    npoints = el->ppolylineattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i][0]), &(points[i][1]),
                   el->ppolylineattr->points[i*2],
                   el->ppolylineattr->points[i*2+1]);
    }
    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledPolygon(npoints, points, r.fill_grd);
        } else {
            GrFilledPolygon(npoints, points, r.cfill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedPolyLine(npoints, points, &(r.lpat));
        } else {
            GrCustomPolyLine(npoints, points, &(r.lopt));
        }
    }
    free_renderctx(&r);
    free(points);
}

static void DrawPolygonElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    RenderCtx r;
    int i, npoints, (*points)[2];
    
    npoints = el->ppolygonattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;

    for (i=0; i <npoints; i++) {
        get_icoord(&(points[i][0]), &(points[i][1]),
                   el->ppolygonattr->points[i*2],
                   el->ppolygonattr->points[i*2+1]);
    }
    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledPolygon(npoints, points, r.fill_grd);
        } else {
            GrFilledPolygon(npoints, points, r.cfill);
        }
    }
    if (pctx->stroke != NO_COLOR) {
        if (r.stroke_grd) {
            GrPatternedPolygon(npoints, points, &(r.lpat));
        } else {
            GrCustomPolygon(npoints, points, &(r.lopt));
        }
    }
    free_renderctx(&r);
    free(points);
}

#if MGRX_VERSION_API >= 0x0143
static void DrawPathElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
/* we have MGRX multipolygons :-) */
    RenderCtx r;
    MsvgSubPath *sp;
    GrPath *gp;
    GrExpPointArray *pa;
    int x, y, i, k, nsp;
    GrMultiPointArray *mpa = NULL;

    nsp = MsvgCountSubPaths(el->ppathattr->sp);
    if (nsp < 1) return;

    mpa = malloc(sizeof(GrMultiPointArray)+sizeof(GrPointArray)*(nsp-1));
    if (mpa == NULL) return;
    mpa->npa = nsp;

    sp = el->ppathattr->sp;
    for (k=0; k<nsp; k++) {
        mpa->p[k].npoints = 0;
        mpa->p[k].points = NULL;
        gp = GrNewPath(sp->npoints);
        if (gp) {
            for (i=0; i< sp->npoints; i++) {
                get_icoord(&x, &y, sp->spp[i].x, sp->spp[i].y);
                GrAddPointToPath(gp, sp->spp[i].cmd, x, y);
            }
            gp->closed = sp->closed;
            pa = GrPathToExpPointArray2(gp);
            if (pa) {
                mpa->p[k].npoints = pa->npoints;
                mpa->p[k].closed = pa->closed;
                mpa->p[k].points = pa->points;
                free(pa); // we don't free pa->points!!
            }
            GrDestroyPath(gp);
        }
        sp = sp->next;
    }

    build_renderctx(&r, pctx);

    if (pctx->fill != NO_COLOR) {
        if (r.fill_grd) {
            GrPatternFilledMultiPolygon(mpa, r.fill_grd);
        } else {
            GrFilledMultiPolygon(mpa, r.cfill);
        }
    }

    if (pctx->stroke != NO_COLOR) {
        for (k=0; k<nsp; k++) {
            if (mpa->p[k].closed) {
                if (r.stroke_grd) {
                    GrPatternedPolygon(mpa->p[k].npoints, mpa->p[k].points, &(r.lpat));
                } else {
                    GrCustomPolygon(mpa->p[k].npoints, mpa->p[k].points, &(r.lopt));
                }
            } else {
                if (r.stroke_grd) {
                    GrPatternedPolyLine(mpa->p[k].npoints, mpa->p[k].points, &(r.lpat));
                } else {
                    GrCustomPolyLine(mpa->p[k].npoints, mpa->p[k].points, &(r.lopt));
                }
            }
        }
    }

    for (k=0; k<nsp; k++)
        if (mpa->p[k].points) free(mpa->p[k].points);
    free(mpa);

    free_renderctx(&r);
}

#else

static void DrawPathElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
/* if we don't have MGRX multipolygons we use a hack to detect if a polygon is
 * inside of another one and fill with the backgroud color, at least it works
 * ok drawing glyphs like "ià"
 */
    RenderCtx r;
    GrColor rcfill, bg;
    MsvgSubPath *sp;
    GrPath *gp;
    GrExpPointArray *pa, *fpa;
    int x, y, i, inside;

    build_renderctx(&r, pctx);

    fpa = NULL;
    bg = glob_bg;
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
                    if (fpa) {
                        inside = GrInsidePolygonTest(fpa->npoints, fpa->points, 
                                                     pa->points[0][0],
                                                     pa->points[0][1]);
                        rcfill = inside ? bg : r.cfill;
                        if (!inside) {
                            GrDestroyExpPointArray(fpa);
                            fpa = NULL;
                        }
                    } else {
                        //bg = GrPixel(pa->points[0][0], pa->points[0][1]);
                        rcfill = r.cfill;
                    }
                    if (r.fill_grd) {
                        GrPatternFilledPolygon(pa->npoints, pa->points, r.fill_grd);
                    } else {
                        GrFilledPolygon(pa->npoints, pa->points, rcfill);
                    }
                }
                if (pctx->stroke != NO_COLOR) {
                    if (pa->closed) {
                        if (r.stroke_grd) {
                            GrPatternedPolygon(pa->npoints, pa->points, &(r.lpat));
                        } else {
                            GrCustomPolygon(pa->npoints, pa->points, &(r.lopt));
                        }
                    } else {
                        if (r.stroke_grd) {
                            GrPatternedPolyLine(pa->npoints, pa->points, &(r.lpat));
                        } else {
                            GrCustomPolyLine(pa->npoints, pa->points, &(r.lopt));
                        }
                    }
                }
                if (!fpa) {
                    fpa = pa;
                    pa = NULL;
                } else {
                    GrDestroyExpPointArray(pa);
                }
            }
            GrDestroyPath(gp);
        }
        sp = sp->next;
    }
    if (fpa) {
        GrDestroyExpPointArray(fpa);
    }
    free_renderctx(&r);
}

#endif

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx, void *udata)
{
    MsvgElement *newel;

    newel = MsvgTransformCookedElement(el, pctx, 0);
    if (newel == NULL) return;

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

int GrDrawSVGtree(MsvgElement *root, GrSVGDrawMode *sdm)
{
    GrColor cfill;
    double ratiow, ratioh, rvb_width, rvb_height;
    int ret;
    double cx, cy;
    TMatrix taux1, taux2, taux3, tsave;
    double scale_x, scale_y;

    if (root == NULL) return -1;
    if (root->eid != EID_SVG) return -2;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return -3;

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
            scale_x = rvb_width / GrSizeX();
            scale_y = rvb_height / GrSizeY();
            break;
        case SVGDRAWMODE_PAR :
            ratiow = GrSizeX() / rvb_width;
            ratioh = GrSizeY() / rvb_height;
            if (ratiow > ratioh) {
                scale_x = rvb_height / GrSizeY();
                scale_y = rvb_height / GrSizeY();
            } else {
                scale_x = rvb_width / GrSizeX();
                scale_y = rvb_width / GrSizeX();
            }
            break;
        case SVGDRAWMODE_SCOORD :
            scale_x = 1;
            scale_y = 1;
            break;
        default:
            return -5;
    }

    if (sdm->adj == SVGDRAWADJ_LEFT) {
        glob_xorg = 0;
        glob_yorg = 0;
    } else if (sdm->adj == SVGDRAWADJ_CENTER) {
        glob_xorg = (GrSizeX() - rvb_width * sdm->zoom / scale_x) / 2;
        glob_yorg = (GrSizeY() - rvb_height * sdm->zoom / scale_y) / 2;
    } else if (sdm->adj == SVGDRAWADJ_RIGHT) {
        glob_xorg = (GrSizeX() - rvb_width * sdm->zoom / scale_x);
        glob_yorg = (GrSizeY() - rvb_height * sdm->zoom / scale_y);
    } else
        return -5;

    glob_bg = sdm->bg;
    if (root->psvgattr->vp_fill != NO_COLOR) {
        cfill = GrAllocColor2(root->psvgattr->vp_fill);
        GrClearContext(cfill);
        glob_bg = cfill;
    } else if (sdm->bg != NO_COLOR) {
        GrClearContext(sdm->bg);
    }

    TMSetTranslation(&taux1, sdm->xdespl, sdm->ydespl);
    TMSetScaling(&taux2, sdm->zoom / scale_x, sdm->zoom / scale_y);
    TMMpy(&taux3, &taux1, &taux2);

    cx = root->psvgattr->vb_width / 2;
    cy = root->psvgattr->vb_height / 2;
    TMSetRotation(&taux2, sdm->rotang, cx, cy);
    TMMpy(&taux1, &taux3, &taux2);

    TMSetTranslation(&taux2, -root->psvgattr->vb_min_x, -root->psvgattr->vb_min_y);
    TMMpy(&glob_tuser, &taux1, &taux2);

    tsave = root->pctx->tmatrix;
    TMMpy(&(root->pctx->tmatrix), &glob_tuser, &tsave);

    ret = MsvgSerCookedTree(root, sufn, NULL, 1);
    root->pctx->tmatrix = tsave;
    if (ret != 1) return -6;

    return 0;
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
