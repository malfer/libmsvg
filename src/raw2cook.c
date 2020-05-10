/* raw2cook.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
    else if (strcmp(color, "black") == 0) return BLACK_COLOR;
    else if (strcmp(color, "silver") == 0) return SILVER_COLOR;
    else if (strcmp(color, "gray") == 0) return GRAY_COLOR;
    else if (strcmp(color, "white") == 0) return WHITE_COLOR;
    else if (strcmp(color, "maroon") == 0) return MAROON_COLOR;
    else if (strcmp(color, "red") == 0) return RED_COLOR;
    else if (strcmp(color, "purple") == 0) return PURPLE_COLOR;
    else if (strcmp(color, "fuchsia") == 0) return FUCHSIA_COLOR;
    else if (strcmp(color, "green") == 0) return GREEN_COLOR;
    else if (strcmp(color, "lime") == 0) return LIME_COLOR;
    else if (strcmp(color, "olive") == 0) return OLIVE_COLOR;
    else if (strcmp(color, "yellow") == 0) return YELLOW_COLOR;
    else if (strcmp(color, "navy") == 0) return NAVY_COLOR;
    else if (strcmp(color, "blue") == 0) return BLUE_COLOR;
    else if (strcmp(color, "teal") == 0) return TEAL_COLOR;
    else if (strcmp(color, "aqua") == 0) return AQUA_COLOR;
    else if (color[0] == '#') {
        if (strlen(color) >= 7) { /* #rrggbb */
            rgbcolor i;
            sscanf(color, "#%6x", &i);
            return i;
        } else { /* #rgb */
            rgbcolor i;
            int r, g, b;
            sscanf(color, "#%1x%1x%1x", &r, &g, &b);
            i = (r << 20) | (r << 16) | (g << 12) | (g << 8) | (b << 4) | b;
            return i;
        }
    } // TODO: support rgb(r,g,b)
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

static void getonetmatrix(char *value, TMatrix *t)
{
    double rnum[6];
    int n;

    TMSetIdentity(t);

    if (strncmp(value, "matrix", 6) == 0) {
        n = MsvgI_read_numbers(value, rnum, 6);
        if (n == 6) TMSetFromArray(t, rnum);
    } else if (strncmp(value, "translate", 9) == 0) {
        n = MsvgI_read_numbers(value, rnum, 2);
        if (n == 1) TMSetTranslation(t, rnum[0], 0);
        else if (n == 2) TMSetTranslation(t, rnum[0], rnum[1]);
    } else if (strncmp(value, "rotate", 6) == 0) {
        n = MsvgI_read_numbers(value, rnum, 3);
        if (n == 1) TMSetRotationOrigin(t, rnum[0]);
        else if (n == 3) TMSetRotation(t, rnum[0], rnum[1], rnum[2]);
    } else if (strncmp(value, "scale", 5) == 0) {
        n = MsvgI_read_numbers(value, rnum, 2);
        if (n == 1) TMSetScaling(t, rnum[0], rnum[0]);
        else if (n == 2) TMSetScaling(t, rnum[0], rnum[1]);
    }
}

static void gettmatrix(char *value, TMatrix *t)
{
    char *valaux, *ptr, *ptrnext;
    int first = 1;
    TMatrix op1, op2;

    TMSetIdentity(t);
    valaux = strdup(value);
    ptr = valaux;

    while (1) {
        ptrnext = strchr(ptr, ')');
        if (ptrnext) {
            *ptrnext = '\0';
            getonetmatrix(ptr, &op2);
            if (first) {
                *t = op2;
                first = 0;
            } else {
                op1 = *t;
                TMMpy(t, &op1, &op2);
            }
            ptr = ptrnext + 1;
            while(*ptr == ' ' || *ptr == ',') ptr++;
        } else {
            break;
        }
    }

    free(valaux);
}

static void readpoints(char *value, double **points, int *npoints)
{
    int n;
    
    *npoints = 0;
    n = MsvgI_count_numbers(value);
    if (n < 2) return;
    *points = (double *)calloc(n, sizeof(double));
    if (*points == NULL) return;
    MsvgI_read_numbers(value, *points, n);
    *npoints = n / 2;
}

static int cookPCtxAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "id") == 0) el->id = strdup(value);
    else if (strcmp(key, "xml:id") == 0) el->id = strdup(value);
    else if (strcmp(key, "fill") == 0) el->pctx.fill = colortorgb(value);
    else if (strcmp(key, "fill-opacity") == 0) el->pctx.fill_opacity = opacitytof(value);
    else if (strcmp(key, "stroke") == 0) el->pctx.stroke = colortorgb(value);
    else if (strcmp(key, "stroke-width") == 0) el->pctx.stroke_width = widthtof(value);
    else if (strcmp(key, "stroke-opacity") == 0) el->pctx.stroke_opacity = opacitytof(value);
    else if (strcmp(key, "transform") == 0) gettmatrix(value, &(el->pctx.tmatrix));
    else return 0;
    return 1;
}

static void cookSvgGenAttr(MsvgElement *el, char *key, char *value)
{
    double daux[4];
    
    if (strcmp(key, "width") == 0) {
        el->psvgattr->width = atof(value);
    } else if (strcmp(key, "height") == 0) {
        el->psvgattr->height = atof(value);
    } else if (strcmp(key, "viewBox") == 0) {
        MsvgI_read_numbers(value, daux, 4);
        el->psvgattr->vb_min_x = daux[0];
        el->psvgattr->vb_min_y = daux[1];
        el->psvgattr->vb_width = daux[2];
        el->psvgattr->vb_height = daux[3];
    } else if (strcmp(key, "vieport-fill") == 0) {
        el->psvgattr->vp_fill = colortorgb(value);
    } else if (strcmp(key, "vieport-fill-opacity") == 0) {
        el->psvgattr->vp_fill_opacity = opacitytof(value);
    }
}

static void cookGGenAttr(MsvgElement *el, char *key, char *value)
{
    return;
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
    if (strcmp(key, "cx") == 0) el->pcircleattr->cx = atof(value);
    else if (strcmp(key, "cy") == 0) el->pcircleattr->cy = atof(value);
    else if (strcmp(key, "r") == 0) el->pcircleattr->r = atof(value);
}

static void cookEllipseGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "cx") == 0) el->pellipseattr->cx = atof(value);
    else if (strcmp(key, "cy") == 0) el->pellipseattr->cy = atof(value);
    else if (strcmp(key, "rx") == 0) el->pellipseattr->rx_x = atof(value);
    else if (strcmp(key, "ry") == 0) el->pellipseattr->ry_y = atof(value);
}

static void cookLineGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "x1") == 0) el->plineattr->x1 = atof(value);
    else if (strcmp(key, "y1") == 0) el->plineattr->y1 = atof(value);
    else if (strcmp(key, "x2") == 0) el->plineattr->x2 = atof(value);
    else if (strcmp(key, "y2") == 0) el->plineattr->y2 = atof(value);
}

static void cookPolylineGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "points") == 0) 
        readpoints(value, &(el->ppolylineattr->points), &(el->ppolylineattr->npoints));
}

static void cookPolygonGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "points") == 0) 
        readpoints(value, &(el->ppolylineattr->points), &(el->ppolylineattr->npoints));
}

static void cookTextGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "x") == 0) el->ptextattr->x = atof(value);
    else if (strcmp(key, "y") == 0) el->ptextattr->y = atof(value);
    else if (strcmp(key, "font-size") == 0) el->ptextattr->font_size = atof(value);
    else if (strcmp(key, "font-family") == 0) el->ptextattr->font_family = strdup(value);
}

static void cookDefsGenAttr(MsvgElement *el, char *key, char *value)
{
    return;
}

static void cookUseGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "x") == 0) el->puseattr->x = atof(value);
    else if (strcmp(key, "y") == 0) el->puseattr->y = atof(value);
}

static void checkSvgCookedAttr(MsvgElement *el)
{
    if (el->psvgattr->vb_width == 0) el->psvgattr->vb_width = el->psvgattr->width;
    if (el->psvgattr->vb_height == 0) el->psvgattr->vb_height = el->psvgattr->height;
    if (el->psvgattr->width == 0) el->psvgattr->width = el->psvgattr->vb_width;
    if (el->psvgattr->height == 0) el->psvgattr->height = el->psvgattr->vb_height;
}
   
static void checkRectCookedAttr(MsvgElement *el)
{
    if (el->prectattr->rx == NODEFINED_VALUE &&
        el->prectattr->ry != NODEFINED_VALUE)
        el->prectattr->rx = el->prectattr->ry;

    if (el->prectattr->ry == NODEFINED_VALUE &&
        el->prectattr->rx != NODEFINED_VALUE)
        el->prectattr->ry = el->prectattr->rx;

    if (el->prectattr->rx == NODEFINED_VALUE)
        el->prectattr->rx = 0;
        
    if (el->prectattr->ry == NODEFINED_VALUE)
        el->prectattr->ry = 0;
        
    if (el->prectattr->rx > (el->prectattr->width / 2.0))
        el->prectattr->rx = el->prectattr->width / 2.0;

    if (el->prectattr->ry > (el->prectattr->height / 2.0))
        el->prectattr->ry = el->prectattr->height / 2.0;
}

static void checkEllipseCookedAttr(MsvgElement *el)
{
    el->pellipseattr->rx_x += el->pellipseattr->cx;
    el->pellipseattr->rx_y = el->pellipseattr->cy;
    el->pellipseattr->ry_x = el->pellipseattr->cx;
    el->pellipseattr->ry_y += el->pellipseattr->cy;
}

static void cookElement(MsvgElement *el, int depth)
{
    MsvgRawAttribute *pattr;
    
    if (el->frattr != NULL) {
        pattr = el->frattr;
        while (pattr != NULL) {
            if (!cookPCtxAttr(el, pattr->key, pattr->value)) {
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
                    case EID_TEXT :
                        cookTextGenAttr(el, pattr->key, pattr->value);
                        break;
                    case EID_DEFS :
                        cookDefsGenAttr(el, pattr->key, pattr->value);
                        break;
                    case EID_USE :
                        cookUseGenAttr(el, pattr->key, pattr->value);
                        break;
                    default :
                        break;
                }
            }
            pattr = pattr->nrattr;
        }
    }
    
    switch (el->eid) {
        case EID_SVG :
            checkSvgCookedAttr(el);
            break;
        case EID_G :
            //checkGCookedAttr(el);
            break;
        case EID_RECT :
            checkRectCookedAttr(el);
            break;
        case EID_CIRCLE :
            //checkCircleCookedAttr(el);
            break;
        case EID_ELLIPSE :
            checkEllipseCookedAttr(el);
            break;
        case EID_LINE :
            //checkLineCookedAttr(el);
            break;
        case EID_POLYLINE :
            //checkPolylineCookedAttr(el);
            break;
        case EID_POLYGON :
            //checkPolygonCookedAttr(el);
            break;
        case EID_TEXT :
            //checkTextCookedAttr(el);
            break;
        case EID_DEFS :
            //checkDefsCookedAttr(el);
            break;
        case EID_USE :
            //checkUseCookedAttr(el);
            break;
        default :
            break;
    }

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
