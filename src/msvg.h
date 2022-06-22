/* msvg.h
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010,2020-2022 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __MSVG_H_INCLUDED__
#define __MSVG_H_INCLUDED__

#include <stdio.h>

#define LIBMSVG_VERSION_API 0x0052

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
    EID_LINEARGRADIENT,
    EID_RADIALGRADIENT,
    EID_STOP,
    EID_FONT,
    EID_FONTFACE,
    EID_MISSINGGLYPH,
    EID_GLYPH,
    EID_V_CONTENT, // defined but not used by now
    EID_LAST = EID_V_CONTENT
};

/* functions in tables.c */

enum EID MsvgFindElementId(const char *ename);
char *MsvgFindElementName(enum EID eid);
int MsvgIsSupSonElement(enum EID fatherid, enum EID sonid);
int MsvgElementCanHaveContent(enum EID eid);
int MsvgIsVirtualElement(enum EID eid);

/* define types of svg trees */

#define RAW_SVGTREE    0   /* tree with raw attributes only */
#define COOKED_SVGTREE 1   /* tree with cooked attributes */

/* define the rgbcolor type and special values*/

typedef int rgbcolor;

#define NO_COLOR        -1
#define INHERIT_COLOR   -2
#define NODEFINED_COLOR -3
#define IRI_COLOR       -4

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

/* define special values for int attributes */

#define INHERIT_IVALUE      -1
#define NODEFINED_IVALUE    -2

/* define values for text context attributes */

#define TEXTANCHOR_START        1
#define TEXTANCHOR_MIDDLE       2
#define TEXTANCHOR_END          3

#define FONTFAMILY_SERIF        1
#define FONTFAMILY_SANS         2
#define FONTFAMILY_CURSIVE      3
#define FONTFAMILY_FANTASY      4
#define FONTFAMILY_MONOSPACE    5
#define FONTFAMILY_OTHER        6

#define FONTSTYLE_NORMAL    1
#define FONTSTYLE_ITALIC    2
#define FONTSTYLE_OBLIQUE   3

#define FONTWEIGHT_100      100
#define FONTWEIGHT_200      200
#define FONTWEIGHT_300      300
#define FONTWEIGHT_400      400
#define FONTWEIGHT_NORMAL   400
#define FONTWEIGHT_500      500
#define FONTWEIGHT_600      600
#define FONTWEIGHT_700      700
#define FONTWEIGHT_BOLD     700
#define FONTWEIGHT_800      800
#define FONTWEIGHT_900      900

/* define values for gradient units */

#define GRADUNIT_BBOX       0
#define GRADUNIT_USER       1

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
    // By now only one content, not sure in the future, when elements between
    // contents are allowed, it can be nested contents or virtual elements
    //MsvgContentPtr ncontent; /* next content */
    int len;                 /* len content */
    char s[1];               /* content (real size = len+1) */
} MsvgContent;

/* transformation matrix */

typedef struct {
    double a, b, c, d, e, f;
} TMatrix;

/* paint context: cooked heritable attributes for some elements */

typedef struct _MsvgPaintCtx *MsvgPaintCtxPtr;

typedef struct _MsvgPaintCtx {
    rgbcolor fill;         /* fill color attribute */
    char *fill_iri;        /* paint server if fill == IRI_COLOR */
    double fill_opacity;   /* fill-opacity attribute */
    rgbcolor stroke;       /* stroke color attribute */
    char *stroke_iri;      /* paint server if stroke == IRI_COLOR */
    double stroke_width;   /* stroke-width attribute */
    double stroke_opacity; /* stroke-opacity attribute */
    TMatrix tmatrix;       /* transformation matrix */
    int text_anchor;       /* text-anchor attribute */
    char *sfont_family;    /* font-family string attribute */
    int ifont_family;      /* font-family type attribute */
    int font_style;        /* font-style attribute */
    int font_weight;       /* font-weight attribute */
    double font_size;      /* font-size attribute */
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

typedef struct {
    double x;       /* absolute x */
    double y;       /* absolute y */
    char cmd;       /* one of 'M', 'L', 'C', 'Q', ' ' */
} MsvgSubPathPoint;

typedef struct _MsvgSubPath *MsvgSubPathPtr;

typedef struct _MsvgSubPath {
    int maxpoints;           /* max capacity (realloc if necesary) */
    int npoints;             /* actual number of points */
    int closed;              /* 1 = yes, 0 = no */
    int failed_realloc;      /* 1 = yes, 0 = no */
    MsvgSubPathPoint *spp;   /* SsubPath points */
    MsvgSubPathPtr next;     /* next SubPath (can be NULL) */
} MsvgSubPath;

typedef struct _MsvgPathAttributes {
    MsvgSubPath *sp;    /* path-data normalized */
} MsvgPathAttributes;

typedef struct _MsvgTextAttributes {
    double x;           /* x attibute */
    double y;           /* y attibute */
} MsvgTextAttributes;

typedef struct _MsvgLinearGradientAttributes {
    int gradunits;      /* Gradient units */
    double x1;          /* grad vector x1 coordinate */
    double y1;          /* grad vector y1 coordinate */
    double x2;          /* grad vector x2 coordinate */
    double y2;          /* grad vector y2 coordinate */
} MsvgLinearGradientAttributes;

typedef struct _MsvgRadialGradientAttributes {
    int gradunits;      /* Gradient units */
    double cx;          /* x center coordinate */
    double cy;          /* y center coordinate */
    double r;           /* radius */
} MsvgRadialGradientAttributes;

typedef struct _MsvgStopAttributes {
    double offset;      /* offset [0..1] */
    double sopacity;    /* stop opacity attribute */
    rgbcolor scolor;    /* stop color attribute */
} MsvgStopAttributes;

typedef struct _MsvgFontAttributes {
    double horiz_adv_x; /* default horizontal advance */
} MsvgFontAttributes;

typedef struct _MsvgFontFaceAttributes {
    char *sfont_family;     /* font-family string attribute */
    int ifont_family;       /* font-family type attribute */
    int font_style;         /* font-style attribute */
    int font_weight;        /* font-weight attribute */
    double units_per_em;    /* em square size */
    double ascent;          /* ascent size */
    double descent;         /* descent size */
} MsvgFontFaceAttributes;

typedef struct _MsvgGlyphAttributes {
    long unicode;       /* unicode point */
    double horiz_adv_x; /* horizontal advance */
    MsvgSubPath *sp;    /* path-data normalized */
} MsvgGlyphAttributes;

/* element structure */

typedef struct _MsvgElement {
    enum EID eid;               /* element type id */
    MsvgElementPtr father;      /* pointer to father element */
    MsvgElementPtr psibling;    /* pointer to previous sibling element */
    MsvgElementPtr nsibling;    /* pointer to next sibling element */
    MsvgElementPtr fson;        /* pointer to first son element */

    MsvgRawAttributePtr frattr; /* pointer to first raw attribute */
    MsvgContentPtr fcontent;    /* pointer to content */

    /* cooked generic attributes */
    char *id;                   /* id attribute */
    MsvgPaintCtxPtr pctx;       /* pointer to painting context */

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
        MsvgLinearGradientAttributes *plgradattr;
        MsvgRadialGradientAttributes *prgradattr;
        MsvgStopAttributes *pstopattr;
        MsvgFontAttributes *pfontattr;
        MsvgFontFaceAttributes *pfontfaceattr;
        MsvgGlyphAttributes *pglyphattr;
    };
} MsvgElement;

/* functions in elements.c */

MsvgElement *MsvgNewElement(enum EID eid, MsvgElement *father);
int MsvgAllocPointsToPolylineElement(MsvgElement *el, int npoints);
int MsvgAllocPointsToPolygonElement(MsvgElement *el, int npoints);

/* functions in attribut.c */

int MsvgAddRawAttribute(MsvgElement *el, const char *key, const char *value);
char *MsvgFindRawAttribute(const MsvgElement *el, const char *key);
int MsvgDelRawAttribute(MsvgElement *el, const char *key);
int MsvgDelAllRawAttributes(MsvgElement *el);
int MsvgCopyRawAttributes(MsvgElement *desel, const MsvgElement *srcel);

int MsvgDelAllTreeRawAttributes(MsvgElement *el);

int MsvgCopyCookedAttributes(MsvgElement *desel, const MsvgElement *srcel);

/* functions in paintctx.c */

MsvgPaintCtx *MsvgNewPaintCtx(const MsvgPaintCtx *src);
void MsvgCopyPaintCtx(MsvgPaintCtx *des, const MsvgPaintCtx *src);
void MsvgDestroyPaintCtx(MsvgPaintCtx *pctx);
void MsvgUndefPaintCtxTextAttr(MsvgPaintCtx *pctx);

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

MsvgElement *MsvgDupElement(MsvgElement *el, int copytree);

int MsvgReplaceElement(MsvgElement *old, MsvgElement *newe);

/* functions in rdsvgf.c */

MsvgElement *MsvgReadSvgFile(const char *fname, int *error);
MsvgElement *MsvgReadSvgFile2(const char *fname, int *error, FILE *report);

/* functions in wtsvgf.c */

int MsvgWriteSvgFile(MsvgElement *root, const char *fname);

/* functions in printree.c */

void MsvgPrintRawElementTree(FILE *f, MsvgElement *el, int depth);

void MsvgPrintPctx(FILE *f, MsvgPaintCtx *pctx);
void MsvgPrintCookedElement(FILE *f, MsvgElement *el);

/* functions in raw2cook.c */

int MsvgRaw2CookedTree(MsvgElement *root);

/* functions in scanpath.c */

MsvgSubPath *MsvgScanPath(char *d);
MsvgSubPath *MsvgNewSubPath(int maxpoints);
void MsvgExpandSubPath(MsvgSubPath *sp);
void MsvgAddPointToSubPath(MsvgSubPath *sp, char cmd, double x, double y);
MsvgSubPath *MsvgDupSubPath(MsvgSubPath *sp);
int MsvgCountSubPaths(MsvgSubPath *sp);
void MsvgDestroySubPath(MsvgSubPath *sp);

/* functions in cook2raw.c */

int MsvgCooked2RawTree(MsvgElement *root);

/* functions in serializ.c */

typedef void (*MsvgSerUserFn)(MsvgElement *el, MsvgPaintCtx *pctx, void *udata);

#define MAX_NESTED_USE_ELEMENT 5

int MsvgSerCookedTree(MsvgElement *root, MsvgSerUserFn sufn, void *udata);

/* functions in tcookel.c */

MsvgElement *MsvgTransformCookedElement(MsvgElement *el, MsvgPaintCtx *pctx);

/* functions in path2ply.c */

MsvgElement *MsvgPathEltoPolyEl(MsvgElement *el, int nsp, double px_x_unit);

/* functions in tmatrix.c */

void TMSetIdentity(TMatrix *des);
int TMIsIdentity(const TMatrix *t);
int TMHaveRotation(const TMatrix *t);
void TMSetFromArray(TMatrix *des, const double *p);
void TMMpy(TMatrix *des, const TMatrix *op1, const TMatrix *op2);
void TMSetTranslation(TMatrix *des, double tx, double ty);
void TMSetScaling(TMatrix *des, double sx, double sy);
void TMSetRotationOrigin(TMatrix *des, double ang);
void TMSetRotation(TMatrix *des, double ang, double cx, double cy);
void TMTransformCoord(double *x, double *y, const TMatrix *ctm);

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
    int nelem;                // num elements in table
    MsvgTableIdItem item[1];  // real size = nelem
} MsvgTableId;

/* functions in find.c */

typedef void (*MsvgWalkUserFn)(MsvgElement *el, void *udata);

void MsvgWalkTree(MsvgElement *root, MsvgWalkUserFn wufn, void *udata);
MsvgElement *MsvgFindFirstFather(MsvgElement *el);
void MsvgCalcCountsCookedTree(const MsvgElement *el, MsvgTreeCounts *tc);
void MsvgCalcCountsRawTree(const MsvgElement *el, MsvgTreeCounts *tc);
MsvgElement *MsvgFindIdCookedTree(MsvgElement *el, char *id);
MsvgElement *MsvgFindIdRawTree(MsvgElement *el, char *id);
MsvgTableId *MsvgBuildTableIdCookedTree(MsvgElement *el);
MsvgTableId *MsvgBuildTableIdRawTree(MsvgElement *el);
void MsvgDestroyTableId(MsvgTableId *tid);
MsvgElement *MsvgFindIdTableId(const MsvgTableId *tid, char *id);

/* functions in cokdims.c */

int MsvgGetCookedDims(MsvgElement *root, double *minx, double *maxx,
                      double *miny, double *maxy);

/* functions in gradnorm.c */

int MsvgNormalizeRawGradients(MsvgElement *el);

/* binary svg font struct */

typedef struct _MsvgBGlyph {
    long unicode;           /* unicode point */
    double horiz_adv_x;     /* horizontal advance */
    MsvgSubPath *sp;        /* path-data normalized */
} MsvgBGlyph;

typedef struct _MsvgBFont {
    double horiz_adv_x;     /* default horizontal advance */
    char *sfont_family;     /* font-family string attribute */
    int ifont_family;       /* font-family type attribute */
    int font_style;         /* font-style attribute */
    int font_weight;        /* font-weight attribute */
    double units_per_em;    /* em square size */
    double ascent;          /* ascent size */
    double descent;         /* descent size */
    MsvgBGlyph missing;     /* missing glyph */
    int num_glyphs;         /* number of glyphs */
    MsvgBGlyph glyph[1];    /* glyphs table, not actual size */
} MsvgBFont;

/* functions in bfont.c */

MsvgBFont *MsvgNewBFont(MsvgElement *el);
void MsvgDestroyBFont(MsvgBFont *bfont);
MsvgElement *MsvgCharToPath(long unicode, double font_size, double *advx, MsvgBFont *bfont);
MsvgElement *MsvgTextToPathGroup(MsvgElement *el, MsvgBFont *bfont);

#endif  /* whole file */
