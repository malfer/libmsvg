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

/*
typedef struct _DrawSettings {
  GrColor cfill;
  GrColor cstroke;
} DrawSettings;
*/

static void DrawSvgElement(MsvgElement *el)
{
//printf("svg %f %f %f %f\n",el->psvgattr->vb_min_x, el->psvgattr->vb_min_y,
//        el->psvgattr->vb_width, el->psvgattr->vb_height );
  GrSetUserWindow(el->psvgattr->vb_min_x,
                  el->psvgattr->vb_min_y,
                  el->psvgattr->vb_min_x+el->psvgattr->vb_width-1,
                  el->psvgattr->vb_min_y+el->psvgattr->vb_height-1);
}

static void DrawRectElement(MsvgElement *el)
{
  GrColor cfill;
  GrColor cstroke;
  
//printf("rect %d %d\n", el->prectattr->fill_color, el->prectattr->stroke_color);
  if (el->prectattr->fill_color != NO_COLOR) {
    cfill = GrAllocColor2(el->prectattr->fill_color);
    GrUsrFilledBox(el->prectattr->x,
                   el->prectattr->y,
                   el->prectattr->x+el->prectattr->width-1,
                   el->prectattr->y+el->prectattr->height-1,
                   cfill);
  }
  if (el->prectattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->prectattr->stroke_color);
    GrUsrBox(el->prectattr->x,
             el->prectattr->y,
             el->prectattr->x+el->prectattr->width-1,
             el->prectattr->y+el->prectattr->height-1,
             cstroke);
  } 
}

static void DrawCircleElement(MsvgElement *el)
{
  GrColor cfill;
  GrColor cstroke;
  
  if (el->pcircleattr->fill_color != NO_COLOR) {
    cfill = GrAllocColor2(el->pcircleattr->fill_color);
    GrUsrFilledCircle(el->pcircleattr->cx,
                      el->pcircleattr->cy,
                      el->pcircleattr->r,
                      cfill);
  }
  if (el->pcircleattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->pcircleattr->stroke_color);
    GrUsrCircle(el->pcircleattr->cx,
                el->pcircleattr->cy,
                el->pcircleattr->r,
                cstroke);
  } 
}

static void DrawEllipseElement(MsvgElement *el)
{
  GrColor cfill;
  GrColor cstroke;
  
  if (el->pellipseattr->fill_color != NO_COLOR) {
    cfill = GrAllocColor2(el->pellipseattr->fill_color);
    GrUsrFilledEllipse(el->pellipseattr->cx,
                      el->pellipseattr->cy,
                      el->pellipseattr->rx,
                      el->pellipseattr->ry,
                      cfill);
  }
  if (el->pellipseattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->pellipseattr->stroke_color);
    GrUsrEllipse(el->pellipseattr->cx,
                el->pellipseattr->cy,
                el->pellipseattr->rx,
                el->pellipseattr->ry,
                cstroke);
  } 
}

static void DrawLineElement(MsvgElement *el)
{
  GrColor cstroke;
  
  if (el->plineattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->plineattr->stroke_color);
    GrUsrLine(el->plineattr->x1, el->plineattr->y1,
              el->plineattr->x2, el->plineattr->y2,
              cstroke);
  } 
}

static void DrawPolylineElement(MsvgElement *el)
{
  GrColor cstroke;
  int i, (*points)[2];
  
  points = calloc(el->ppolylineattr->npoints, sizeof(int[2]));
  if (points == NULL) return;
  for (i=0; i <el->ppolylineattr->npoints; i++) {
    points[i][0] = el->ppolylineattr->points[i*2];
    points[i][1] = el->ppolylineattr->points[i*2+1];
  }

  if (el->ppolylineattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->ppolylineattr->stroke_color);
    GrUsrPolyLine(el->ppolylineattr->npoints, points, cstroke);
  } 
}

static void DrawPolygonElement(MsvgElement *el)
{
  GrColor cfill;
  GrColor cstroke;
  int i, (*points)[2];
  
//  printf("%d puntos\n", el->ppolygonattr->npoints);
  points = calloc(el->ppolygonattr->npoints, sizeof(int[2]));
  if (points == NULL) return;
  for (i=0; i <el->ppolygonattr->npoints; i++) {
    points[i][0] = el->ppolygonattr->points[i*2];
    points[i][1] = el->ppolygonattr->points[i*2+1];
  }
//  for (i=0;i<10;i++) printf("%f,%f\n",points[i][0],points[i][1]);
//  for (i=0;i<10;i++) printf("%f,%f\n",el->ppolygonattr->points[i*2],el->ppolygonattr->points[i*2+1]);

  if (el->ppolygonattr->fill_color != NO_COLOR) {
    cfill = GrAllocColor2(el->ppolygonattr->fill_color);
    GrUsrFilledPolygon(el->ppolygonattr->npoints, points, cfill);
  }
  if (el->ppolygonattr->stroke_color != NO_COLOR) {
    cstroke = GrAllocColor2(el->ppolygonattr->stroke_color);
    GrUsrPolygon(el->ppolygonattr->npoints, points, cstroke);
  } 
}

static void DrawElement(MsvgElement *el)
{
  switch (el->eid) {
  case EID_SVG :
    DrawSvgElement(el);
    break;
/*
  case EID_G :
    DrawGElement(el, pattr->key, pattr->value);
    break;
*/
  case EID_RECT :
    DrawRectElement(el);
    break;
  case EID_CIRCLE :
    DrawCircleElement(el);
    break;
  case EID_ELLIPSE :
    DrawEllipseElement(el);
    break;
  case EID_LINE :
    DrawLineElement(el);
    break;
  case EID_POLYLINE :
    DrawPolylineElement(el);
    break;
  case EID_POLYGON :
    DrawPolygonElement(el);
    break;
  default :
    break;
  }
}

void DrawSVGtree(MsvgElement *root)
{
  MsvgElement *p = root;
  
  while(1) {
    if (p == NULL) return;
    DrawElement(p);
    if (p->fson != NULL)
      DrawSVGtree(p->fson);
    p = p->nsibling;
  }
}

/****
int DrawSVGtree(MsvgElement *root)
{
  if (root == NULL) return 0;
  if (root->eid != EID_SVG) return 0;
  if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;

  DrawElement(root, 0);

  return 1;
}
*/
