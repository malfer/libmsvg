/* msvg.h
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010,2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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

#define LIBMSVG_VERSION_API 0x0012

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

/* functions in tables.c */

enum EID MsvgFindElementId(const char *ename);
char * MsvgFindElementName(enum EID eid);
int MsvgIsSupSonElementId(enum EID fatherid, enum EID sonid);

/* define types of svg trees */

#define RAW_SVGTREE    0   /* tree with raw attributes only */
#define COOKED_SVGTREE 1   /* tree with cooked attributes */

/* define the rgbcolor type and special values*/

typedef int rgbcolor;

#define NO_COLOR        -1
#define INHERIT_COLOR   -2
#define NODEFINED_COLOR -3

#define BLACK_COLOR   0x000000
#define SILVER_COLOR  0xc0c0c0
#define GRAY_COLOR    0x808080
#define WHITE_COLOR   0xffffff
#define MAROON_COLOR  0x800000
#define RED_COLOR     0xff0000
#define PURPLE_COLOR  0x800080
#define FUCHSIA_COLOR 0xff00ff
#define GREEN_COLOR   0x008000
#define LIME_COLOR    0x00ff00
#define OLIVE_COLOR   0x808000
#define YELLOW_COLOR  0xffff00
#define NAVY_COLOR    0x000080
#define BLUE_COLOR    0x0000ff
#define TEAL_COLOR    0x008080
#define AQUA_COLOR    0x00ffff

/* define special values for double attributes */

#define INHERIT_VALUE   -1.0
#define NODEFINED_VALUE -2.0

/* raw attributes */

typedef struct _MsvgRawAttribute *MsvgRawAttributePtr;

typedef struct _MsvgRawAttribute {
    char *key;                  /* key attribute */
    char *value;                /* value attribute */
    MsvgRawAttributePtr nrattr; /* pointer to next raw attribute */
} MsvgRawAttribute;

/* paint context: cooked heritable attributes for all elements */

typedef struct _MsvgPaintCtx {
    rgbcolor fill;         /* fill color attribute */
    double fill_opacity;   /* fill-opacity attribute */
    rgbcolor stroke;       /* stroke color attribute */
    double stroke_width;   /* stroke-width attribute */
    double stroke_opacity; /* stroke-opacity attribute */
} MsvgPaintCtx;

/* cooked specific attributes for each element */

typedef struct _MsvgSvgAttributes {
    int tree_type;
    double width;           /* width attribute */
    double height;          /* height attribute */
    double vb_min_x;        /* viewBox attributes */
    double vb_min_y;
    double vb_width;
    double vb_height;
    rgbcolor vp_fill;       /* viewport-fill attribute */
    double vp_fill_opacity; /* viewport-fill-opacity attribute */
} MsvgSvgAttributes;

typedef struct _MsvgGAttributes {
    int dummy;
} MsvgGAttributes;

typedef struct _MsvgRectAttributes {
    double x;      /* x attribute */
    double y;      /* y attribute */
    double width;  /* width attribute */
    double height; /* height attribute */
    double rx;     /* rx attribute */
    double ry;     /* ry attribute */
} MsvgRectAttributes;

typedef struct _MsvgCircleAttributes {
    double cx; /* cx attribute */
    double cy; /* cy attribute */
    double r;  /* r attribute */
} MsvgCircleAttributes;

typedef struct _MsvgEllipseAttributes {
    double cx; /* cx attribute */
    double cy; /* cy attribute */
    double rx; /* rx attribute */
    double ry; /* ry attribute */
} MsvgEllipseAttributes;

typedef struct _MsvgLineAttributes {
    double x1; /* x1 attribute */
    double y1; /* y1 attribute */
    double x2; /* x2 attribute */
    double y2; /* y2 attribute */
} MsvgLineAttributes;

typedef struct _MsvgPolylineAttributes {
    double *points; /* points attibute */
    int npoints;    /* number of points */
} MsvgPolylineAttributes;

typedef struct _MsvgPolygonAttributes {
    double *points; /* points attibute */
    int npoints;    /* number of points */
} MsvgPolygonAttributes;

/* element structure */

typedef struct _MsvgElement *MsvgElementPtr;

typedef struct _MsvgElement {
    enum EID eid;               /* element type id */
    MsvgElementPtr father;      /* pointer to father element */
    MsvgElementPtr psibling;    /* pointer to previous sibling element */
    MsvgElementPtr nsibling;    /* pointer to next sibling element */
    MsvgElementPtr fson;        /* pointer to first son element */
    MsvgRawAttributePtr frattr; /* pointer to first raw attribute */
                                /* cooked generic attributes */
    char *id;                   /* id attribute */
    MsvgPaintCtx pctx;          /* painting context */
    union {                     /* cooked specific attributes */
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

/* functions in elements.c */

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father);

/* functions in attribut.c */

int MsvgAddRawAttribute(MsvgElement *el, const char *key, const char *value);
int MsvgDelRawAttribute(MsvgElement *el, const char *key);
int MsvgDelAllRawAttributes(MsvgElement *el);
int MsvgCopyRawAttributes(MsvgElement *desel, MsvgElement *srcel);

int MsvgDelAllTreeRawAttributes(MsvgElement *el);

int MsvgCopyCookedAttributes(MsvgElement *desel, MsvgElement *srcel);

/* functions in manielem.c */

void MsvgPruneElement(MsvgElement *el);
void MsvgDeleteElement(MsvgElement *el);

int MsvgInsertSonElement(MsvgElement *el, MsvgElement *father);
int MsvgInsertPSiblingElement(MsvgElement *el, MsvgElement *sibling);
int MsvgInsertNSiblingElement(MsvgElement *el, MsvgElement *sibling);

MsvgElement *MsvgDupElement(MsvgElement *el);

/* functions in rdsvgf.c */

MsvgElement *MsvgReadSvgFile(const char *fname);

/* functions in wtsvgf.c */

int MsvgWriteSvgFile(MsvgElement *root, const char *fname);

/* functions in printree.c */

void MsvgPrintRawElementTree(FILE *f, MsvgElement *el, int depth);

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx);
void MsvgPrintCookedElement(FILE *f, MsvgElement *el);

/* functions in raw2cook.c */

int MsvgRaw2CookedTree(MsvgElement *root);

/* functions in serializ.c */

typedef void (*MsvgSerUserFn)(MsvgElement *el, MsvgPaintCtx *pctx);

int MsvgSerCookedTree(MsvgElement *root, MsvgSerUserFn sufn);
