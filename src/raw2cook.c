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

static void cookSvgGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookGGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookRectGenAttr(MsvgElement *el, char *key, char *value)
{
  if (strcmp(key, "x") == 0) el->prectattr->x = atof(value);
  else if (strcmp(key, "y") == 0) el->prectattr->y = atof(value);
  else if (strcmp(key, "width") == 0) el->prectattr->width = atof(value);
  else if (strcmp(key, "height") == 0) el->prectattr->height = atof(value);
  else if (strcmp(key, "rx") == 0) el->prectattr->rx = atof(value);
  else if (strcmp(key, "ry") == 0) el->prectattr->ry = atof(value);
}

static void cookCircleGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookEllipseGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookLineGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookPolylineGenAttr(MsvgElement *el, char *key, char *value)
{
}

static void cookPolygonGenAttr(MsvgElement *el, char *key, char *value)
{
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
       }
       pattr = pattr->nattr;
     }
  }

  /* free generic attributes now? */
  
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