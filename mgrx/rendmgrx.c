/* rendmgrx.c ---- functions to render svg trees using MGRX library
 * 
 * These test programs are a dirty hack to test the libmsvg librarie with the
 * mgrx graphics library. It is NOT part of the librarie really.
 *
 * In the future this will be added to MGRX
 *
 * libmsvg, a minimal library to read and write svg files
 * Copyright (C) 2010 Mariano Alvarez Fernandez (malfer at telefonica.net)
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

#include <mgrx.h>
#include <msvg.h>
#include "rendmgrx.h"

/*
 * typedef struct _DrawSettings {
 *  GrColor cfill;
 *  GrColor cstroke;
 * } DrawSettings;
 */

static void DrawRectElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    
    //printf("rect %d %d\n", pctx.fill, pctx.stroke);
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrUsrFilledBox(el->prectattr->x,
                       el->prectattr->y,
                       el->prectattr->x+el->prectattr->width-1,
                       el->prectattr->y+el->prectattr->height-1,
                       cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomBox(el->prectattr->x,
                 el->prectattr->y,
                 el->prectattr->x+el->prectattr->width-1,
                 el->prectattr->y+el->prectattr->height-1,
                 &lopt);
        } else {
            GrUsrBox(el->prectattr->x,
                 el->prectattr->y,
                 el->prectattr->x+el->prectattr->width-1,
                 el->prectattr->y+el->prectattr->height-1,
                 cstroke);
        }
    } 
}

static void DrawCircleElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrUsrFilledCircle(el->pcircleattr->cx,
                          el->pcircleattr->cy,
                          el->pcircleattr->r,
                          cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomCircle(el->pcircleattr->cx,
                    el->pcircleattr->cy,
                    el->pcircleattr->r,
                    &lopt);
        } else {
            GrUsrCircle(el->pcircleattr->cx,
                    el->pcircleattr->cy,
                    el->pcircleattr->r,
                    cstroke);
        }
    } 
}

static void DrawEllipseElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrUsrFilledEllipse(el->pellipseattr->cx,
                           el->pellipseattr->cy,
                           el->pellipseattr->rx,
                           el->pellipseattr->ry,
                           cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomEllipse(el->pellipseattr->cx,
                     el->pellipseattr->cy,
                     el->pellipseattr->rx,
                     el->pellipseattr->ry,
                     &lopt);
        } else {
            GrUsrEllipse(el->pellipseattr->cx,
                     el->pellipseattr->cy,
                     el->pellipseattr->rx,
                     el->pellipseattr->ry,
                     cstroke);
        }
    } 
}

static void DrawLineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cstroke;
    GrLineOption lopt;
    
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 1) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomLine(el->plineattr->x1, el->plineattr->y1,
                  el->plineattr->x2, el->plineattr->y2,
                  &lopt);
        } else {
            GrUsrLine(el->plineattr->x1, el->plineattr->y1,
                  el->plineattr->x2, el->plineattr->y2,
                  cstroke);
        }
    } 
}

static void DrawPolylineElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cstroke;
    GrLineOption lopt;
    int i, npoints, (*points)[2];
    
    npoints = el->ppolylineattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;
    for (i=0; i <npoints; i++) {
        points[i][0] = el->ppolylineattr->points[i*2];
        points[i][1] = el->ppolylineattr->points[i*2+1];
    }
    
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 0) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomPolyLine(npoints, points, &lopt);
        } else {
            GrUsrPolyLine(npoints, points, cstroke);
        }
    } 
    free(points);
}

static void DrawPolygonElement(MsvgElement *el, MsvgPaintCtx *pctx)
{
    GrColor cfill;
    GrColor cstroke;
    GrLineOption lopt;
    int i, npoints, (*points)[2];
    
    //  printf("%d puntos\n", el->ppolygonattr->npoints);
    npoints = el->ppolygonattr->npoints;
    points = calloc(npoints, sizeof(int[2]));
    if (points == NULL) return;
    for (i=0; i <npoints; i++) {
        points[i][0] = el->ppolygonattr->points[i*2];
        points[i][1] = el->ppolygonattr->points[i*2+1];
    }
    //  for (i=0;i<10;i++) printf("%d,%d\n",points[i][0],points[i][1]);
    //  for (i=0;i<10;i++) printf("%f,%f\n",el->ppolygonattr->points[i*2],el->ppolygonattr->points[i*2+1]);
    
    if (pctx->fill != NO_COLOR) {
        cfill = GrAllocColor2(pctx->fill);
        GrUsrFilledPolygon(npoints, points, cfill);
    }
    if (pctx->stroke != NO_COLOR) {
        cstroke = GrAllocColor2(pctx->stroke);
        if (el->pctx.stroke_width > 0) {
            lopt.lno_color = cstroke;
            lopt.lno_width = el->pctx.stroke_width;
            lopt.lno_pattlen = 0;
            lopt.lno_dashpat = NULL;
            GrUsrCustomPolygon(npoints, points, &lopt);
        } else {
            GrUsrPolygon(npoints, points, cstroke);
        }
    }
    free(points);
}

static void sufn(MsvgElement *el, MsvgPaintCtx *pctx)
{
    switch (el->eid) {
        case EID_RECT :
            DrawRectElement(el, pctx);
            break;
        case EID_CIRCLE :
            DrawCircleElement(el, pctx);
            break;
        case EID_ELLIPSE :
            DrawEllipseElement(el, pctx);
            break;
        case EID_LINE :
            DrawLineElement(el, pctx);
            break;
        case EID_POLYLINE :
            DrawPolylineElement(el, pctx);
            break;
        case EID_POLYGON :
            DrawPolygonElement(el, pctx);
            break;
        default :
            break;
    }
}

int DrawSVGtree(MsvgElement *root)
{
    GrColor cfill;

    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;
    
    GrSetUserWindow(root->psvgattr->vb_min_x,
                    root->psvgattr->vb_min_y,
                    root->psvgattr->vb_min_x+root->psvgattr->vb_width-1,
                    root->psvgattr->vb_min_y+root->psvgattr->vb_height-1);

    if (root->psvgattr->vp_fill != NO_COLOR) {
        cfill = GrAllocColor2(root->psvgattr->vp_fill);
        GrClearContext(cfill);
    }

    MsvgSerCookedTree(root, sufn);
    
    return 1;
}

