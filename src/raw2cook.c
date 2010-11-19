/* raw2cook.c
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
#include "msvg.h"
#include "util.h"

static rgbcolor colortorgb(char *color)
{
  if (strcmp(color, "none") == 0) return NO_COLOR;
  else if (strcmp(color, "currentColor") == 0) return INHERIT_COLOR;
  else if (strcmp(color, "inherit") == 0) return INHERIT_COLOR;
  else if (color[0] == '#') {
    if (strlen(color) >= 7) { /* #rrggbb */
      rgbcolor i;
      sscanf(color, "#%6x", &i);
      return i;
    } else { /* #rgb */
      rgbcolor i;
      int r, g, b;
      sscanf(color, "#%x%x%x", &r, &g, &b);
      i = (r << 20) + (r << 16) + (g << 12) + (g << 8) + (r << 4) + r;
      return i;
    }
  }
  /* TODO: support color names */
  else return NO_COLOR;
}

static double opacitytof(char *value)
{
  if (strcmp(value, "inherit") == 0) return INHERIT_VALUE;
  return atof(value);
}

static double widthtof(char *value)
{
  if (strcmp(value, "inherit") == 0) return INHERIT_VALUE;
  return atof(value);
}

static void readpoints(char *value, double **points, int *npoints)
{
  /* TODO: read the points */
}

static void cookSvgGenAttr(MsvgElement *el, char *key, char *value)
{
  double daux[4];
  
  if (strcmp(key, "width") == 0) el->psvgattr->width = atof(value);
  else if (strcmp(key, "height") == 0) el->psvgattr->height = atof(value);
  else if (strcmp(key, "viewBox") == 0) {
    MsvgI_read_numbers(value, daux, 4);
    el->psvgattr->vb_min_x = daux[0];
    el->psvgattr->vb_min_y = daux[1];
    el->psvgattr->vb_width = daux[2];
    el->psvgattr->vb_height = daux[3];
  }
  else if (strcmp(key, "vieport-fill") == 0) el->psvgattr->fill_color = colortorgb(value);
  else if (strcmp(key, "vieport-fill-opacity") == 0) el->psvgattr->opacity = opacitytof(value);
}

static void cookGGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->pgattr->id = strdup(value);
  else if (strcmp(key, "fill") == 0) el->pgattr->fill_color = colortorgb(value);
  else if (strcmp(key, "fill-opacity") == 0) el->pgattr->fill_opacity = opacitytof(value);
  else if (strcmp(key, "stroke") == 0) el->pgattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->pgattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->pgattr->stroke_opacity = opacitytof(value);
}

static void cookRectGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->prectattr->id = strdup(value);
  else if (strcmp(key, "x") == 0) el->prectattr->x = atof(value);
  else if (strcmp(key, "y") == 0) el->prectattr->y = atof(value);
  else if (strcmp(key, "width") == 0) el->prectattr->width = atof(value);
  else if (strcmp(key, "height") == 0) el->prectattr->height = atof(value);
  else if (strcmp(key, "rx") == 0) el->prectattr->rx = atof(value);
  else if (strcmp(key, "ry") == 0) el->prectattr->ry = atof(value);
  else if (strcmp(key, "fill") == 0) el->prectattr->fill_color = colortorgb(value);
  else if (strcmp(key, "fill-opacity") == 0) el->prectattr->fill_opacity = opacitytof(value);
  else if (strcmp(key, "stroke") == 0) el->prectattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->prectattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->prectattr->stroke_opacity = opacitytof(value);
}

static void cookCircleGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->pcircleattr->id = strdup(value);
  else if (strcmp(key, "cx") == 0) el->pcircleattr->cx = atof(value);
  else if (strcmp(key, "cy") == 0) el->pcircleattr->cy = atof(value);
  else if (strcmp(key, "r") == 0) el->pcircleattr->r = atof(value);
  else if (strcmp(key, "fill") == 0) el->pcircleattr->fill_color = colortorgb(value);
  else if (strcmp(key, "fill-opacity") == 0) el->pcircleattr->fill_opacity = opacitytof(value);
  else if (strcmp(key, "stroke") == 0) el->pcircleattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->pcircleattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->pcircleattr->stroke_opacity = opacitytof(value);
}

static void cookEllipseGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->pellipseattr->id = strdup(value);
  else if (strcmp(key, "cx") == 0) el->pellipseattr->cx = atof(value);
  else if (strcmp(key, "cy") == 0) el->pellipseattr->cy = atof(value);
  else if (strcmp(key, "rx") == 0) el->pellipseattr->rx = atof(value);
  else if (strcmp(key, "ry") == 0) el->pellipseattr->ry = atof(value);
  else if (strcmp(key, "fill") == 0) el->pellipseattr->fill_color = colortorgb(value);
  else if (strcmp(key, "fill-opacity") == 0) el->pellipseattr->fill_opacity = opacitytof(value);
  else if (strcmp(key, "stroke") == 0) el->pellipseattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->pellipseattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->pellipseattr->stroke_opacity = opacitytof(value);
}

static void cookLineGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->plineattr->id = strdup(value);
  else if (strcmp(key, "x1") == 0) el->plineattr->x1 = atof(value);
  else if (strcmp(key, "y1") == 0) el->plineattr->y1 = atof(value);
  else if (strcmp(key, "x2") == 0) el->plineattr->x2 = atof(value);
  else if (strcmp(key, "y2") == 0) el->plineattr->y2 = atof(value);
  else if (strcmp(key, "stroke") == 0) el->plineattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->plineattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->plineattr->stroke_opacity = opacitytof(value);
}

static void cookPolylineGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->ppolylineattr->id = strdup(value);
  else if (strcmp(key, "points") == 0) 
    readpoints(value, &(el->ppolylineattr->points), &(el->ppolylineattr->npoints));
  else if (strcmp(key, "stroke") == 0) el->ppolylineattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->ppolylineattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->ppolylineattr->stroke_opacity = opacitytof(value);
}

static void cookPolygonGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "id") == 0) el->ppolygonattr->id = strdup(value);
  else if (strcmp(key, "points") == 0) 
    readpoints(value, &(el->ppolylineattr->points), &(el->ppolylineattr->npoints));
  else if (strcmp(key, "fill") == 0) el->ppolygonattr->fill_color = colortorgb(value);
  else if (strcmp(key, "fill-opacity") == 0) el->ppolygonattr->fill_opacity = opacitytof(value);
  else if (strcmp(key, "stroke") == 0) el->ppolygonattr->stroke_color = colortorgb(value);
  else if (strcmp(key, "stroke-width") == 0) el->ppolygonattr->stroke_opacity = widthtof(value);
  else if (strcmp(key, "stroke-opacity") == 0) el->ppolygonattr->stroke_opacity = opacitytof(value);
}

static void cookElement(MsvgElement *el, int depth)
{
  MsvgAttribute *pattr;

  if (el->fattr != NULL) {
     pattr = el->fattr;
     while (pattr != NULL) {
       switch (el->eid) {
       case EID_SVG :
         cookSvgGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_G :
         cookGGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_RECT :
         cookRectGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_CIRCLE :
         cookCircleGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_ELLIPSE :
         cookEllipseGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_LINE :
         cookLineGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_POLYLINE :
         cookPolylineGenAttr(el, pattr->key, pattr->value);
         break;
       case EID_POLYGON :
         cookPolygonGenAttr(el, pattr->key, pattr->value);
         break;
       default :
         break;
       }
       pattr = pattr->nattr;
     }
  }

  /* TODO: free the generic attributes now? */
  
  if (el->fson != NULL)
    cookElement(el->fson, depth+1);

  if (el->nsibling != NULL)
    cookElement(el->nsibling, depth);
}

int MsvgRaw2CookedTree(MsvgElement *root)
{
  if (root == NULL) return 0;
  if (root->eid != EID_SVG) return 0;
  if (root->psvgattr->tree_type != RAW_SVGTREE) return 0;

  cookElement(root, 0);
  root->psvgattr->tree_type = COOKED_SVGTREE;

  return 1;
}