/* raw2cook.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

static void getcolorattr(char *value, rgbcolor *rgb, char **iri)
{
    char *start, *end, *viri;
    int irilen, i;
    
    if (strncmp(value, "url(#", 5) == 0) {
        *iri = NULL;
        *rgb = NODEFINED_COLOR;
        start = &value[5];
        end = strchr(start, ')');
        if (end) {
            irilen = end - start;
            viri = calloc(1, irilen+1);
            if (viri) {
                for (i=0; i<irilen; i++) viri[i] = start[i];
                start[irilen] = '\0';
                *iri = viri;
                *rgb = IRI_COLOR;
            }
        }
    }
    else {
        *iri = NULL;
        *rgb = colortorgb(value);
    }
}

static double lengthof(char *value)
{
    double v;

    if (strcmp(value, "inherit") == 0) return INHERIT_VALUE;
    v = atof(value);
    if (strstr(value, "pt") != NULL) v *= 1.25;
    else if (strstr(value, "pc") != NULL) v *= 15;
    else if (strstr(value, "mm") != NULL) v *= 3.54;
    else if (strstr(value, "cm") != NULL) v *= 35.4;
    else if (strstr(value, "in") != NULL) v *= 90; // so assuming 90 px/inch
    return v;
}

static double opacitytof(char *value)
{
    double op;

    if (strcmp(value, "inherit") == 0) return INHERIT_VALUE;
    op = atof(value);
    if (op < 0) return 0;
    if (op > 1) return 1;
    return op;
}

static double widthtof(char *value)
{
    if (strcmp(value, "inherit") == 0) return INHERIT_VALUE;
    return atof(value);
}

static void getonetmatrix(char *value, TMatrix *t)
{
    double rnum[6];
    int n = 0;

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

static int textanchor(char *value)
{
    if (strcmp(value, "inherit") == 0) return INHERIT_IVALUE;
    else if (strstr(value, "start") != NULL) return TEXTANCHOR_START;
    else if (strstr(value, "middle") != NULL) return TEXTANCHOR_MIDDLE;
    else if (strstr(value, "end") != NULL) return TEXTANCHOR_END;
    else return NODEFINED_IVALUE;
}

static int fontfamily(char *value)
{
    char *s;
    int i, len, ret;

    len = strlen(value);
    s = malloc(len+1);
    if (s == NULL) return FONTFAMILY_OTHER;
    for (i=0; i<len; i++) s[i] = tolower(value[i]);
    s[len] = '\0';

    if (strcmp(s, "inherit") == 0) ret = INHERIT_IVALUE;
    else if (strstr(s, "sans") != NULL) ret = FONTFAMILY_SANS;
    else if (strstr(s, "serif") != NULL) ret = FONTFAMILY_SERIF;
    else if (strstr(s, "cursive") != NULL) ret = FONTFAMILY_CURSIVE;
    else if (strstr(s, "fantasy") != NULL) ret = FONTFAMILY_FANTASY;
    else if (strstr(s, "mono") != NULL) ret = FONTFAMILY_MONOSPACE;
    else ret = FONTFAMILY_OTHER;

    free(s);
    return ret;
}

static int fontstyle(char *value)
{
    if (strcmp(value, "inherit") == 0) return INHERIT_IVALUE;
    else if (strcmp(value, "normal") == 0) return FONTSTYLE_NORMAL;
    else if (strcmp(value, "italic") == 0) return FONTSTYLE_ITALIC;
    else if (strcmp(value, "oblique") == 0) return FONTSTYLE_OBLIQUE;
    else return NODEFINED_IVALUE;
}

static int fontweight(char *value)
{
    if (strcmp(value, "inherit") == 0) return INHERIT_IVALUE;
    else if (strcmp(value, "100") == 0) return FONTWEIGHT_100;
    else if (strcmp(value, "200") == 0) return FONTWEIGHT_200;
    else if (strcmp(value, "300") == 0) return FONTWEIGHT_300;
    else if (strcmp(value, "400") == 0) return FONTWEIGHT_400;
    else if (strcmp(value, "500") == 0) return FONTWEIGHT_500;
    else if (strcmp(value, "600") == 0) return FONTWEIGHT_600;
    else if (strcmp(value, "700") == 0) return FONTWEIGHT_700;
    else if (strcmp(value, "800") == 0) return FONTWEIGHT_800;
    else if (strcmp(value, "900") == 0) return FONTWEIGHT_900;
    else if (strcmp(value, "normal") == 0) return FONTWEIGHT_NORMAL;
    else if (strcmp(value, "bold") == 0) return FONTWEIGHT_BOLD;
    else return NODEFINED_IVALUE;
}

static double fontsize(char *value)
{
    return lengthof(value);
    // other values TODO
}

static int cookPCtxAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "id") == 0 || strcmp(key, "xml:id") == 0) {
        if (el->id) free(el->id);
        el->id = strdup(value);
        return 1;
    }

    if (el->pctx) {
        if (strcmp(key, "fill") == 0) {
            if (el->pctx->fill_iri) free(el->pctx->fill_iri);
            getcolorattr(value, &(el->pctx->fill), &(el->pctx->fill_iri));
        }
        else if (strcmp(key, "fill-opacity") == 0) el->pctx->fill_opacity = opacitytof(value);
        else if (strcmp(key, "stroke") == 0) {
            if (el->pctx->stroke_iri) free(el->pctx->stroke_iri);
            getcolorattr(value, &(el->pctx->stroke), &(el->pctx->stroke_iri));
        }
        else if (strcmp(key, "stroke-width") == 0) el->pctx->stroke_width = widthtof(value);
        else if (strcmp(key, "stroke-opacity") == 0) el->pctx->stroke_opacity = opacitytof(value);
        else if (strcmp(key, "transform") == 0) gettmatrix(value, &(el->pctx->tmatrix));
        else if (strcmp(key, "text-anchor") == 0) el->pctx->text_anchor = textanchor(value);
        else if (strcmp(key, "font-family") == 0) {
            el->pctx->sfont_family = strdup(value);
            el->pctx->ifont_family = fontfamily(value);
        }
        else if (strcmp(key, "font-style") == 0) el->pctx->font_style = fontstyle(value);
        else if (strcmp(key, "font-weight") == 0) el->pctx->font_weight = fontweight(value);
        else if (strcmp(key, "font-size") == 0) el->pctx->font_size = fontsize(value);
        else return 0;
        return 1;
    }

    return 0;
}

static void cookSvgGenAttr(MsvgElement *el, char *key, char *value)
{
    double daux[4];
    
    if (strcmp(key, "width") == 0) {
        el->psvgattr->width = lengthof(value);
    } else if (strcmp(key, "height") == 0) {
        el->psvgattr->height = lengthof(value);
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

static void cookDefsGenAttr(MsvgElement *el, char *key, char *value)
{
    return;
}

static void cookGGenAttr(MsvgElement *el, char *key, char *value)
{
    return;
}

static void cookUseGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "x") == 0) el->puseattr->x = atof(value);
    else if (strcmp(key, "y") == 0) el->puseattr->y = atof(value);
    else if (strcmp(key, "xlink:href") == 0 && value[0] == '#')
        el->puseattr->refel = strdup(&(value[1]));
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

static void cookPathGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "d") == 0) el->ppathattr->sp = MsvgScanPath(value);
}

static void cookTextGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "x") == 0) el->ptextattr->x = atof(value);
    else if (strcmp(key, "y") == 0) el->ptextattr->y = atof(value);
}

static void cookLinearGradientGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "gradientUnits") == 0) {
        if (strcmp(value, "userSpaceOnUse") == 0)
            el->plgradattr->gradunits = GRADUNIT_USER;
        else
            el->plgradattr->gradunits = GRADUNIT_BBOX;
    } else if (strcmp(key, "x1") == 0) el->plgradattr->x1 = atof(value);
    else if (strcmp(key, "y1") == 0) el->plgradattr->y1 = atof(value);
    else if (strcmp(key, "x2") == 0) el->plgradattr->x2 = atof(value);
    else if (strcmp(key, "y2") == 0) el->plgradattr->y2 = atof(value);
}

static void cookRadialGradientGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "gradientUnits") == 0) {
        if (strcmp(value, "userSpaceOnUse") == 0)
            el->prgradattr->gradunits = GRADUNIT_USER;
        else
            el->prgradattr->gradunits = GRADUNIT_BBOX;
    } else if (strcmp(key, "cx") == 0) el->prgradattr->cx = atof(value);
    else if (strcmp(key, "cy") == 0) el->prgradattr->cy = atof(value);
    else if (strcmp(key, "r") == 0) el->prgradattr->r = atof(value);
}

static void cookStopGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "offset") == 0) el->pstopattr->offset = atof(value);
    else if (strcmp(key, "stop-opacity") == 0) el->pstopattr->sopacity = opacitytof(value);
    else if (strcmp(key, "stop-color") == 0) el->pstopattr->scolor = colortorgb(value);
}

static void cookFontGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "horiz-adv-x") == 0) el->pfontattr->horiz_adv_x = atof(value);
}

static void cookFontFaceGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "font-family") == 0) {
        el->pfontfaceattr->sfont_family = strdup(value);
        el->pfontfaceattr->ifont_family = fontfamily(value);
    }
    else if (strcmp(key, "font-style") == 0)
        el->pfontfaceattr->font_style = fontstyle(value);
    else if (strcmp(key, "font-weight") == 0)
        el->pfontfaceattr->font_weight = fontweight(value);
    else if (strcmp(key, "units-per-em") == 0)
        el->pfontfaceattr->units_per_em = atof(value);
    else if (strcmp(key, "ascent") == 0)
        el->pfontfaceattr->ascent = atof(value);
    else if (strcmp(key, "descent") == 0)
        el->pfontfaceattr->descent = atof(value);
}

static void cookMissingGlyphGenAttr(MsvgElement *el, char *key, char *value)
{
    if (strcmp(key, "horiz-adv-x") == 0) el->pglyphattr->horiz_adv_x = atof(value);
    else if (strcmp(key, "d") == 0) el->pglyphattr->sp = MsvgScanPath(value);
}

static void cookGlyphGenAttr(MsvgElement *el, char *key, char *value)
{
    int nb;

    if (strcmp(key, "unicode") == 0) {
        // if more than one unicode store 0
        el->pglyphattr->unicode = MsvgI_NextUCPfromUTF8Str((unsigned char *)value, &nb);
        if (value[nb] != '\0') el->pglyphattr->unicode = 0;
        //printf("Unicode!! %s %08lx\n", value, el->pglyphattr->unicode);
    }
    else if (strcmp(key, "horiz-adv-x") == 0) el->pglyphattr->horiz_adv_x = atof(value);
    else if (strcmp(key, "d") == 0) el->pglyphattr->sp = MsvgScanPath(value);
}

static void checkSvgCookedAttr(MsvgElement *el)
{
    if (el->psvgattr->vb_width <= 0) el->psvgattr->vb_width = el->psvgattr->width;
    if (el->psvgattr->vb_height <= 0) el->psvgattr->vb_height = el->psvgattr->height;
    if (el->psvgattr->width <= 0) el->psvgattr->width = el->psvgattr->vb_width;
    if (el->psvgattr->height <= 0) el->psvgattr->height = el->psvgattr->vb_height;

    // defaults if no declared dimensions
    if (el->psvgattr->vb_width <= 0)
        el->psvgattr->vb_width = el->psvgattr->width = 600;
    if (el->psvgattr->vb_height <= 0)
        el->psvgattr->vb_height = el->psvgattr->height = 300;
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
    
    pattr = el->frattr;
    while (pattr != NULL) {
        if (!cookPCtxAttr(el, pattr->key, pattr->value)) {
            switch (el->eid) {
                case EID_SVG :
                    cookSvgGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_DEFS :
                    cookDefsGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_G :
                    cookGGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_USE :
                    cookUseGenAttr(el, pattr->key, pattr->value);
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
                case EID_PATH :
                    cookPathGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_TEXT :
                    cookTextGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_LINEARGRADIENT :
                    cookLinearGradientGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_RADIALGRADIENT :
                    cookRadialGradientGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_STOP :
                    cookStopGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_FONT :
                    cookFontGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_FONTFACE :
                    cookFontFaceGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_MISSINGGLYPH :
                    cookMissingGlyphGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_GLYPH :
                    cookGlyphGenAttr(el, pattr->key, pattr->value);
                    break;
                case EID_TITLE :
                case EID_DESC :
                case EID_V_COMMENT :
                case EID_V_CONTENT :
                    break;
                default :
                    break;
            }
        }
        pattr = pattr->nrattr;
    }
    
    switch (el->eid) {
        case EID_SVG :
            checkSvgCookedAttr(el);
            break;
        case EID_RECT :
            checkRectCookedAttr(el);
            break;
        case EID_ELLIPSE :
            checkEllipseCookedAttr(el);
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
