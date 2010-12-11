/* element.c
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
#include "msvg.h"

MsvgElement *MsvgNewGenericElement(enum EID eid, MsvgElement *father)
{
  MsvgElement *element;
  MsvgElement *ptr;

  element = calloc(1, sizeof(MsvgElement));
  if (element == NULL) return NULL;

  element->eid = eid;

  if (father) {
    element->father = father;
    if (father->fson == NULL) {
      father->fson = element;
    } else {
      ptr = father->fson;
      while (ptr->nsibling != NULL)
        ptr = ptr->nsibling;
      ptr->nsibling = element;
      element->psibling = ptr;
    }
  }

  return element;
}

MsvgElement *MsvgNewSvgElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_SVG, father);
  if (element == NULL) return NULL;

  element->psvgattr = calloc(1, sizeof(MsvgSvgAttributes));
  if (element->psvgattr == NULL) {
    free(element);
    return NULL;
  }

  element->psvgattr->width = 0;
  element->psvgattr->height = 0;
  element->psvgattr->vb_min_x = 0;
  element->psvgattr->vb_min_y = 0;
  element->psvgattr->vb_width = 0;
  element->psvgattr->vb_height = 0;
  element->psvgattr->fill_color = 0; /* black */
  element->psvgattr->opacity = 1; /* solid */
  element->psvgattr->tree_type = RAW_SVGTREE;
  return element;
}

MsvgElement *MsvgNewGElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_G, father);
  if (element == NULL) return NULL;

  element->pgattr = calloc(1, sizeof(MsvgGAttributes));
  if (element->pgattr == NULL) {
    free(element);
    return NULL;
  }

  element->pgattr->id = NULL;
  element->pgattr->fill_color = NO_COLOR;
  element->pgattr->fill_opacity = INHERIT_VALUE;
  element->pgattr->stroke_color = NO_COLOR;
  element->pgattr->stroke_width = INHERIT_VALUE;
  element->pgattr->stroke_opacity = INHERIT_VALUE;

return element;
}

MsvgElement *MsvgNewRectElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_RECT, father);
  if (element == NULL) return NULL;

  element->prectattr = calloc(1, sizeof(MsvgRectAttributes));
  if (element->prectattr == NULL) {
    free(element);
    return NULL;
  }

  element->prectattr->id = NULL;
  element->prectattr->x = 0;
  element->prectattr->y = 0;
  element->prectattr->width = 0;
  element->prectattr->height = 0;
  element->prectattr->rx = 0;
  element->prectattr->ry = 0;
  element->prectattr->fill_color = NO_COLOR;
  element->prectattr->fill_opacity = INHERIT_VALUE;
  element->prectattr->stroke_color = NO_COLOR;
  element->prectattr->stroke_width = INHERIT_VALUE;
  element->prectattr->stroke_opacity = INHERIT_VALUE;
  
  return element;
}

MsvgElement *MsvgNewCircleElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_CIRCLE, father);
  if (element == NULL) return NULL;

  element->pcircleattr = calloc(1, sizeof(MsvgCircleAttributes));
  if (element->pcircleattr == NULL) {
    free(element);
    return NULL;
  }

  element->pcircleattr->id = NULL;
  element->pcircleattr->cx = 0;
  element->pcircleattr->cy = 0;
  element->pcircleattr->r = 0;
  element->pcircleattr->fill_color = NO_COLOR;
  element->pcircleattr->fill_opacity = INHERIT_VALUE;
  element->pcircleattr->stroke_color = NO_COLOR;
  element->pcircleattr->stroke_width = INHERIT_VALUE;
  element->pcircleattr->stroke_opacity = INHERIT_VALUE;
  
  return element;
}

MsvgElement *MsvgNewEllipseElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_ELLIPSE, father);
  if (element == NULL) return NULL;

  element->pellipseattr = calloc(1, sizeof(MsvgEllipseAttributes));
  if (element->pellipseattr == NULL) {
    free(element);
    return NULL;
  }

  element->pellipseattr->id = NULL;
  element->pellipseattr->cx = 0;
  element->pellipseattr->cy = 0;
  element->pellipseattr->rx = 0;
  element->pellipseattr->ry = 0;
  element->pellipseattr->fill_color = NO_COLOR;
  element->pellipseattr->fill_opacity = INHERIT_VALUE;
  element->pellipseattr->stroke_color = NO_COLOR;
  element->pellipseattr->stroke_width = INHERIT_VALUE;
  element->pellipseattr->stroke_opacity = INHERIT_VALUE;

  return element;
}

MsvgElement *MsvgNewLineElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_LINE, father);
  if (element == NULL) return NULL;

  element->plineattr = calloc(1, sizeof(MsvgLineAttributes));
  if (element->plineattr == NULL) {
    free(element);
    return NULL;
  }

  element->plineattr->id = NULL;
  element->plineattr->x1 = 0;
  element->plineattr->y1 = 0;
  element->plineattr->x2 = 0;
  element->plineattr->y2 = 0;
  element->plineattr->stroke_color = NO_COLOR;
  element->plineattr->stroke_width = INHERIT_VALUE;
  element->plineattr->stroke_opacity = INHERIT_VALUE;

  return element;
}

MsvgElement *MsvgNewPolylineElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_POLYLINE, father);
  if (element == NULL) return NULL;

  element->ppolylineattr = calloc(1, sizeof(MsvgPolylineAttributes));
  if (element->ppolylineattr == NULL) {
    free(element);
    return NULL;
  }

  element->ppolylineattr->id = NULL;
  element->ppolylineattr->points = NULL;
  element->ppolylineattr->npoints = 0;
  element->ppolylineattr->stroke_color = NO_COLOR;
  element->ppolylineattr->stroke_width = INHERIT_VALUE;
  element->ppolylineattr->stroke_opacity = INHERIT_VALUE;

  return element;
}

MsvgElement *MsvgNewPolygonElement(MsvgElement *father)
{
  MsvgElement *element;

  element = MsvgNewGenericElement(EID_POLYGON, father);
  if (element == NULL) return NULL;

  element->ppolygonattr = calloc(1, sizeof(MsvgPolygonAttributes));
  if (element->ppolygonattr == NULL) {
    free(element);
    return NULL;
  }

  element->ppolygonattr->id = NULL;
  element->ppolygonattr->points = NULL;
  element->ppolygonattr->npoints = 0;
  element->ppolygonattr->fill_color = NO_COLOR;
  element->ppolygonattr->fill_opacity = INHERIT_VALUE;
  element->ppolygonattr->stroke_color = NO_COLOR;
  element->ppolygonattr->stroke_width = INHERIT_VALUE;
  element->ppolygonattr->stroke_opacity = INHERIT_VALUE;

  return element;
}

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father)
{
  MsvgElement *element;

  switch (eid) {
  case EID_SVG :
    element = MsvgNewSvgElement(father);
    break;
  case EID_G :
    element = MsvgNewGElement(father);
    break;
  case EID_RECT :
    element = MsvgNewRectElement(father);
    break;
  case EID_CIRCLE :
    element = MsvgNewCircleElement(father);
    break;
  case EID_ELLIPSE :
    element = MsvgNewEllipseElement(father);
    break;
  case EID_LINE :
    element = MsvgNewLineElement(father);
    break;
  case EID_POLYLINE :
    element = MsvgNewPolylineElement(father);
    break;
  case EID_POLYGON :
    element = MsvgNewPolygonElement(father);
    break;
  default :
    return NULL;
  }

  return element;
}
