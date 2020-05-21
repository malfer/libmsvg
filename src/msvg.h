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

#ifndef __MSVG_H_INCLUDED__
#define __MSVG_H_INCLUDED__

#include <stdio.h>

#define LIBMSVG_VERSION_API 0x0016

/* define id's for supported elements */

enum EID {
    EID_NOTSUPPORTED = 0,
    EID_SVG = 1,
    EID_DEFS,
    EID_G,
    EID_USE,
    EID_RECT,
    EID_CIRCLE,
    EID_ELLIPSE,
    EID_LINE,
    EID_POLYLINE,
    EID_POLYGON,
    EID_PATH,
    EID_TEXT,
    EID_LAST = EID_TEXT
};

/* functions in tables.c */

enum EID MsvgFindElementId(const char *ename);
char * MsvgFindElementName(enum EID eid);
int MsvgIsSupSonElementId(enum EID fatherid, enum EID sonid);
int MsvgElementCanHaveContent(enum EID eid);

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

/* element pointer */

typedef struct _MsvgElement *MsvgElementPtr;

/* raw attributes */

typedef struct _MsvgRawAttribute *MsvgRawAttributePtr;

typedef struct _MsvgRawAttribute {
    char *key;                  /* key attribute */
    char *value;                /* value attribute */
    MsvgRawAttributePtr nrattr; /* pointer to next raw attribute */
} MsvgRawAttribute;

/* contents */

typedef struct _MsvgConten *MsvgContentPtr;

typedef struct _MsvgConten {
    MsvgContentPtr ncontent; /* next content */
    int len;                 /* len content */
    char s[1];               /* content (real size = len+1) */
} MsvgContent;

/* transformation matrix */

typedef struct {
    double a, b, c, d, e, f;
} TMatrix;

/* paint context: cooked heritable attributes for all elements */

typedef struct _MsvgPaintCtx {
    rgbcolor fill;         /* fill color attribute */
    double fill_opacity;   /* fill-opacity attribute */
    rgbcolor stroke;       /* stroke color attribute */
    double stroke_width;   /* stroke-width attribute */
    double stroke_opacity; /* stroke-opacity attribute */
    TMatrix tmatrix;       /* transformation matrix */
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

typedef struct _MsvgDefsAttributes {
    int dummy;
} MsvgDefsAttributes;

typedef struct _MsvgGAttributes {
    int dummy;
} MsvgGAttributes;

typedef struct _MsvgUseAttributes {
    double x;       /* x attibute */
    double y;       /* y attibute */
    char * refel;   /* referenced element */
} MsvgUseAttributes;

typedef struct _MsvgRectAttributes {
    double x;       /* x attribute */
    double y;       /* y attribute */
    double width;   /* width attribute */
    double height;  /* height attribute */
    double rx;      /* rx attribute */
    double ry;      /* ry attribute */
} MsvgRectAttributes;

typedef struct _MsvgCircleAttributes {
    double cx;      /* cx attribute */
    double cy;      /* cy attribute */
    double r;       /* r attribute */
} MsvgCircleAttributes;

typedef struct _MsvgEllipseAttributes {
    double cx;      /* cx attribute */
    double cy;      /* cy attribute */
    double rx_x;    /* axis 1 x, y coord */
    double rx_y;    /* note rx = sqrt((rx_x-cx)^2 + (rx_y-cy)^2) */
    double ry_x;    /* axis 2 x, y coord */
    double ry_y;    /* note ry = sqrt((ry_x-cx)^2 + (ry_y-cy)^2) */
} MsvgEllipseAttributes;

typedef struct _MsvgLineAttributes {
    double x1;      /* x1 attribute */
    double y1;      /* y1 attribute */
    double x2;      /* x2 attribute */
    double y2;      /* y2 attribute */
} MsvgLineAttributes;

typedef struct _MsvgPolylineAttributes {
    double *points; /* points attibute */
    int npoints;    /* number of points */
} MsvgPolylineAttributes;

typedef struct _MsvgPolygonAttributes {
    double *points; /* points attibute */
    int npoints;    /* number of points */
} MsvgPolygonAttributes;

typedef struct _MsvgPathAttributes {
    char *path;     /* path-data normalized */
} MsvgPathAttributes;

typedef struct _MsvgTextAttributes {
    double x;          /* x attibute */
    double y;          /* y attibute */
    double font_size;  /* font-size attribute */
    char *font_family; /* font-family attribute */
} MsvgTextAttributes;

/* element structure */

typedef struct _MsvgElement {
    enum EID eid;               /* element type id */
    MsvgElementPtr father;      /* pointer to father element */
    MsvgElementPtr psibling;    /* pointer to previous sibling element */
    MsvgElementPtr nsibling;    /* pointer to next sibling element */
    MsvgElementPtr fson;        /* pointer to first son element */

    MsvgRawAttributePtr frattr; /* pointer to first raw attribute */
    MsvgContentPtr fcontent;    /* pointer to first content */

    /* cooked generic attributes */
    char *id;                   /* id attribute */
    MsvgPaintCtx pctx;          /* painting context */

    /* cooked specific attributes */
    union {
        MsvgSvgAttributes *psvgattr;
        MsvgDefsAttributes *pdefsattr;
        MsvgGAttributes *pgattr;
        MsvgUseAttributes *puseattr;
        MsvgRectAttributes *prectattr;
        MsvgCircleAttributes *pcircleattr;
        MsvgEllipseAttributes *pellipseattr;
        MsvgLineAttributes *plineattr;
        MsvgPolylineAttributes *ppolylineattr;
        MsvgPolygonAttributes *ppolygonattr;
        MsvgPathAttributes *ppathattr;
        MsvgTextAttributes *ptextattr;
    };
} MsvgElement;

/* functions in elements.c */

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father);
int MsvgAllocPointsToPolylineElement(MsvgElement *el, int npoints);
int MsvgAllocPointsToPolygonElement(MsvgElement *el, int npoints);

/* functions in attribut.c */

int MsvgAddRawAttribute(MsvgElement *el, const char *key, const char *value);
int MsvgDelRawAttribute(MsvgElement *el, const char *key);
int MsvgDelAllRawAttributes(MsvgElement *el);
int MsvgCopyRawAttributes(MsvgElement *desel, MsvgElement *srcel);

int MsvgDelAllTreeRawAttributes(MsvgElement *el);

int MsvgCopyCookedAttributes(MsvgElement *desel, MsvgElement *srcel);

/* functions in content.c */

int MsvgAddContent(MsvgElement *el, int len, char *cnt);
int MsvgDelContents(MsvgElement *el);
int MsvgCopyContents(MsvgElement *desel, MsvgElement *srcel);

/* functions in manielem.c */

void MsvgPruneElement(MsvgElement *el);
void MsvgDeleteElement(MsvgElement *el);

int MsvgInsertSonElement(MsvgElement *el, MsvgElement *father);
int MsvgInsertPSiblingElement(MsvgElement *el, MsvgElement *sibling);
int MsvgInsertNSiblingElement(MsvgElement *el, MsvgElement *sibling);

MsvgElement *MsvgDupElement(MsvgElement *el);

/* functions in rdsvgf.c */

MsvgElement *MsvgReadSvgFile(const char *fname, int *error);

/* functions in wtsvgf.c */

int MsvgWriteSvgFile(MsvgElement *root, const char *fname);

/* functions in printree.c */

void MsvgPrintRawElementTree(FILE *f, MsvgElement *el, int depth);

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx);
void MsvgPrintCookedElement(FILE *f, MsvgElement *el);

/* functions in raw2cook.c */

int MsvgRaw2CookedTree(MsvgElement *root);

/* functions in cook2raw.c */

int MsvgCooked2RawTree(MsvgElement *root);

/* functions in serializ.c */

typedef void (*MsvgSerUserFn)(MsvgElement *el, MsvgPaintCtx *pctx);

#define MAX_NESTED_USE_ELEMENT 5

int MsvgSerCookedTree(MsvgElement *root, MsvgSerUserFn sufn);

/* functions in tcookel.c */

MsvgElement * MsvgTransformCookedElement(MsvgElement *el, MsvgPaintCtx *pctx, int bef);

/* functions in tmatrix.c */

void TMSetIdentity(TMatrix *des);
int TMIsIdentity(TMatrix *t);
int TMHaveRotation(TMatrix *t);
void TMSetFromArray(TMatrix *des, double *p);
void TMMpy(TMatrix *des, TMatrix *op1, TMatrix *op2);
void TMSetTranslation(TMatrix *des, double tx, double ty);
void TMSetScaling(TMatrix *des, double sx, double sy);
void TMSetRotationOrigin(TMatrix *des, double ang);
void TMSetRotation(TMatrix *des, double ang, double cx, double cy);
void TMTransformCoord(double *x, double *y, TMatrix *ctm);

/* MsvgTreeCounts structure */

typedef struct {
    int nelem[EID_LAST+1];  // num elements per type
    int totelem;            // total num elements
    int totelwid;           // num elements with id != NULL
} MsvgTreeCounts;

/* MsvgTableId structure */

typedef struct {
    char *id;
    MsvgElement *el;
} MsvgTableIdItem;

typedef struct {
    int nelem;                // nume elements in table
    MsvgTableIdItem item[1];  // real size = nelem
} MsvgTableId;

/* functions in find.c */

MsvgElement *MsvgFindFirstFather(MsvgElement *el);
void MsvgCalcCountsCookedTree(MsvgElement *el, MsvgTreeCounts *tc);
void MsvgCalcCountsRawTree(MsvgElement *el, MsvgTreeCounts *tc);
MsvgElement * MsvgFindIdCookedTree(MsvgElement *el, char *id);
MsvgElement * MsvgFindIdRawTree(MsvgElement *el, char *id);
MsvgTableId * MsvgBuildTableIdCookedTree(MsvgElement *el);
MsvgTableId * MsvgBuildTableIdRawTree(MsvgElement *el);
void MsvgDestroyTableId(MsvgTableId *tid);
MsvgElement *MsvgFindIdTableId(MsvgTableId *tid, char *id);

#endif  /* whole file */
