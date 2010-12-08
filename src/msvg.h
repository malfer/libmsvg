/* msvg.h
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

#include <stdio.h>

#define LIBMSVG_VERSION_API 0x0001

/* define id's for supported elements */
enum EID {
  EID_NOTSUPPORTED = 0,
  EID_SVG = 1,
  EID_G,
  EID_RECT,
  EID_CIRCLE,
  EID_ELLIPSE,
  EID_LINE,
  EID_POLYLINE,
  EID_POLYGON,
  EID_LAST = EID_POLYGON
};

enum EID MsvgFindElementId(const char *ename);
char * MsvgFindElementName(enum EID eid);
int MsvgIsSupSonElementId(enum EID fatherid, enum EID sonid);

/* define types of svg trees */

#define RAW_SVGTREE 0      /* tree with generic attributes only */
#define COOKED_SVGTREE 1   /* tree with specific attributes */

/* define the rgbcolor type */

typedef int rgbcolor;

#define NO_COLOR -1
#define INHERIT_COLOR -2

/* define inherit value for double attributes */

#define INHERIT_VALUE -1.0

/* specific attributes for specific elements */

typedef struct _MsvgSvgAttributes {
  int tree_type;
  double width; /* width attribute */
  double height; /* height attribute */
  double vb_min_x; /* viewBox attribute */
  double vb_min_y;
  double vb_width;
  double vb_height;
  rgbcolor fill_color; /* viewport-fill attribute */
  double opacity; /* viewport-fill-opacity attribute */
} MsvgSvgAttributes;

typedef struct _MsvgGAttributes {
  char *id; /* id attribute */
  rgbcolor fill_color; /* fill attribute */
  double fill_opacity; /* fill-opacity attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgGAttributes;

typedef struct _MsvgRectAttributes {
  char *id; /* id attribute */
  double x; /* x attribute */
  double y; /* y attribute */
  double width; /* width attribute */
  double height; /* height attribute */
  double rx; /* rx attribute */
  double ry; /* ry attribute */
  rgbcolor fill_color; /* fill attribute */
  double fill_opacity; /* fill-opacity attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgRectAttributes;

typedef struct _MsvgCircleAttributes {
  char *id; /* id attribute */
  double cx; /* cx attribute */
  double cy; /* cy attribute */
  double r; /* r attribute */
  rgbcolor fill_color; /* fill attribute */
  double fill_opacity; /* fill-opacity attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgCircleAttributes;

typedef struct _MsvgEllipseAttributes {
  char *id; /* id attribute */
  double cx; /* cx attribute */
  double cy; /* cy attribute */
  double rx; /* rx attribute */
  double ry; /* ry attribute */
  rgbcolor fill_color; /* fill attribute */
  double fill_opacity; /* fill-opacity attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgEllipseAttributes;

typedef struct _MsvgLineAttributes {
  char *id; /* id attribute */
  double x1; /* x1 attribute */
  double y1; /* y1 attribute */
  double x2; /* x2 attribute */
  double y2; /* y2 attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgLineAttributes;

typedef struct _MsvgPolylineAttributes {
  char *id; /* id attribute */
  double *points; /* points attibute */
  int npoints; /* number of points */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgPolylineAttributes;

typedef struct _MsvgPolygonAttributes {
  char *id; /* id attribute */
  double *points; /* points attibute */
  int npoints; /* number of points */
  rgbcolor fill_color; /* fill attribute */
  double fill_opacity; /* fill-opacity attribute */
  rgbcolor stroke_color; /* stroke attribute */
  double stroke_width; /* stroke-width attribute */
  double stroke_opacity; /* stroke-opacity attribute */
} MsvgPolygonAttributes;

/* generic attributes */

typedef struct _MsvgAttribute *MsvgAttributePtr;

typedef struct _MsvgAttribute {
  char *key; /* key attribute */
  char *value; /* value attribute */
  MsvgAttributePtr nattr; /* pointer to next attribute */
} MsvgAttribute;

/* element structure */

typedef struct _MsvgElement *MsvgElementPtr;

typedef struct _MsvgElement {
  enum EID eid;
  MsvgElementPtr father; /* pointer to father element */
  MsvgElementPtr psibling; /* pointer to previous sibling element */
  MsvgElementPtr nsibling; /* pointer to next sibling element */
  MsvgElementPtr fson; /* pointer to first son element */
  MsvgAttributePtr fattr; /* pointer to first generic attribute */
  union { /* specific attributes */
    MsvgSvgAttributes *psvgattr;
    MsvgGAttributes *pgattr;
    MsvgRectAttributes *prectattr;
    MsvgCircleAttributes *pcircleattr;
    MsvgEllipseAttributes *pellipseattr;
    MsvgLineAttributes *plineattr;
    MsvgPolylineAttributes *ppolylineattr;
    MsvgPolygonAttributes *ppolygonattr;
  };
} MsvgElement;

int MsvgAddAttribute(MsvgElement *pelement, const char *key, const char *value);

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father);

void MsvgPruneElement(MsvgElement *el);
void MsvgDeleteElement(MsvgElement *el);
int MsvgInsertSonElement(MsvgElement *el, MsvgElement *father);
int MsvgInsertPSiblingElement(MsvgElement *el, MsvgElement *sibling);
int MsvgInsertNSiblingElement(MsvgElement *el, MsvgElement *sibling);

MsvgElement *MsvgReadSvgFile(const char *fname);

int MsvgWriteSvgFile(MsvgElement *root, const char *fname);

void MsvgPrintElementTree(FILE *f, MsvgElement *ptr, int depth);

int MsvgRaw2CookedTree(MsvgElement *root);
