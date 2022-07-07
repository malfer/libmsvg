/* paintctx.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2022 Mariano Alvarez Fernandez
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
#include "msvg.h"

MsvgPaintCtx *MsvgNewPaintCtx(const MsvgPaintCtx *src)
{
    MsvgPaintCtx *pctx = NULL;

    pctx = calloc(1, sizeof(MsvgPaintCtx));
    if (pctx == NULL) return NULL;

    pctx->fill = NODEFINED_COLOR;
    pctx->fill_iri = NULL;
    pctx->fill_opacity = NODEFINED_VALUE;
    pctx->stroke = NODEFINED_COLOR;
    pctx->stroke_iri = NULL;
    pctx->stroke_width = NODEFINED_VALUE;
    pctx->stroke_opacity = NODEFINED_VALUE;
    TMSetIdentity(&(pctx->tmatrix));
    pctx->text_anchor = NODEFINED_IVALUE;
    pctx->sfont_family = NULL;
    pctx->ifont_family = NODEFINED_IVALUE;
    pctx->font_style = NODEFINED_IVALUE;
    pctx->font_weight = NODEFINED_IVALUE;
    pctx->font_size = NODEFINED_VALUE;

    if (src) MsvgCopyPaintCtx(pctx, src);

    return pctx;
}

void MsvgCopyPaintCtx(MsvgPaintCtx *des, const MsvgPaintCtx *src)
{
    if (!des || !src) return;
    if (des->fill_iri) free(des->fill_iri);
    if (des->stroke_iri) free(des->stroke_iri);
    if (des->sfont_family) free(des->sfont_family);

    *des = *src;
    if (src->fill_iri) des->fill_iri = strdup(src->fill_iri);
    if (src->stroke_iri) des->stroke_iri = strdup(src->stroke_iri);
    if (src->sfont_family) des->sfont_family = strdup(src->sfont_family);
}

void MsvgDestroyPaintCtx(MsvgPaintCtx *pctx)
{
    if (!pctx) return;
    if (pctx->fill_iri) free(pctx->fill_iri);
    if (pctx->stroke_iri) free(pctx->stroke_iri);
    if (pctx->sfont_family) free(pctx->sfont_family);
    free(pctx);
}

void MsvgUndefPaintCtxTextAttr(MsvgPaintCtx *pctx)
{
    if (!pctx) return;
    if (pctx->sfont_family) free(pctx->sfont_family);
    pctx->sfont_family = NULL;
    pctx->ifont_family = NODEFINED_IVALUE;
    pctx->font_style = NODEFINED_IVALUE;
    pctx->font_weight = NODEFINED_IVALUE;
    pctx->font_size = NODEFINED_VALUE;
}

int MsvgGetInheritedTextAnchor(const MsvgElement *el)
{
    MsvgElement *fath;

    if (el && el->pctx) {
        if ((el->pctx->text_anchor != INHERIT_IVALUE) &&
            (el->pctx->text_anchor != NODEFINED_IVALUE))
            return el->pctx->text_anchor;
        fath = el->father;
        while (fath) {
            if (fath->pctx && (fath->pctx->text_anchor != INHERIT_IVALUE) &&
                (fath->pctx->text_anchor != NODEFINED_IVALUE))
                return fath->pctx->text_anchor;
            fath = fath->father;
        }
    }

    return TEXTANCHOR_START;
}

double MsvgGetInheritedFontSize(const MsvgElement *el)
{
    MsvgElement *fath;

    if (el && el->pctx) {
        if ((el->pctx->font_size != INHERIT_VALUE) &&
            (el->pctx->font_size != NODEFINED_VALUE))
            return el->pctx->font_size;
        fath = el->father;
        while (fath) {
            if (fath->pctx && (fath->pctx->font_size != INHERIT_VALUE) &&
                (fath->pctx->font_size != NODEFINED_VALUE))
                return fath->pctx->font_size;
            fath = fath->father;
        }
    }

    return 12.0;
}

void MsvgProcPaintCtxInheritance(MsvgPaintCtx *son, const MsvgPaintCtx *fath)
{
    TMatrix taux;

    if (son->fill == INHERIT_COLOR || son->fill == NODEFINED_COLOR) {
        son->fill = fath->fill;
        if (son->fill_iri) {
            free(son->fill_iri);
            son->fill_iri = NULL;
        }
        if (fath->fill_iri)
            son->fill_iri = strdup(fath->fill_iri);
    }

    if (son->fill_opacity == INHERIT_VALUE ||
        son->fill_opacity == NODEFINED_VALUE) {
        son->fill_opacity = fath->fill_opacity;
    }

    if (son->stroke == INHERIT_COLOR || son->stroke == NODEFINED_COLOR) {
        son->stroke = fath->stroke;
        if (son->stroke_iri) {
            free(son->stroke_iri);
            son->stroke_iri = NULL;
        }
        if (fath->stroke_iri)
            son->stroke_iri = strdup(fath->stroke_iri);
    }

    if (son->stroke_width == INHERIT_VALUE ||
        son->stroke_width == NODEFINED_VALUE) {
        son->stroke_width = fath->stroke_width;
    }

    if (son->stroke_opacity == INHERIT_VALUE ||
        son->stroke_opacity == NODEFINED_VALUE) {
        son->stroke_opacity = fath->stroke_opacity;
    }

    taux = son->tmatrix;
    TMMpy(&(son->tmatrix), &(fath->tmatrix), &taux);

    if (son->text_anchor == INHERIT_IVALUE ||
        son->text_anchor == NODEFINED_IVALUE) {
        son->text_anchor = fath->text_anchor;
    }

    if (son->ifont_family == INHERIT_IVALUE ||
        son->ifont_family == NODEFINED_IVALUE) {
        if (son->sfont_family) {
            free(son->sfont_family);
            son->sfont_family = NULL;
        }
        if (fath->sfont_family)
            son->sfont_family = strdup(fath->sfont_family);
        son->ifont_family = fath->ifont_family;
    }

    if (son->font_style == INHERIT_IVALUE ||
        son->font_style == NODEFINED_IVALUE) {
        son->font_style = fath->font_style;
    }

    if (son->font_weight == INHERIT_IVALUE ||
        son->font_weight == NODEFINED_IVALUE) {
        son->font_weight = fath->font_weight;
    }

    if (son->font_size == INHERIT_VALUE ||
        son->font_size == NODEFINED_VALUE) {
        son->font_size = fath->font_size;
    }
}

void MsvgProcPaintCtxDefaults(MsvgPaintCtx *des)
{
    /* set defaults values if not defined */
    
    if (des->fill == INHERIT_COLOR || des->fill == NODEFINED_COLOR) {
        des->fill = 0;  // black
    }

    if (des->fill_opacity == INHERIT_VALUE ||
        des->fill_opacity == NODEFINED_VALUE) {
        des->fill_opacity = 1.0;  // solid
    }

    if (des->stroke == INHERIT_COLOR || des->stroke == NODEFINED_COLOR) {
        des->stroke = NO_COLOR;
    }

    if (des->stroke_width == INHERIT_VALUE ||
        des->stroke_width == NODEFINED_VALUE) {
        des->stroke_width = 1.0;
    }

    if (des->stroke_opacity == INHERIT_VALUE ||
        des->stroke_opacity == NODEFINED_VALUE) {
        des->stroke_opacity = 1.0;  // solid
    }

    if (des->text_anchor == INHERIT_IVALUE ||
        des->text_anchor == NODEFINED_IVALUE) {
        des->text_anchor = TEXTANCHOR_START;
    }

    if (des->ifont_family == INHERIT_IVALUE ||
        des->ifont_family == NODEFINED_IVALUE) {
        des->ifont_family = FONTFAMILY_SANS;
    }

    if (des->font_style == INHERIT_IVALUE ||
        des->font_style == NODEFINED_IVALUE) {
        des->font_style = FONTSTYLE_NORMAL;
    }

    if (des->font_weight == INHERIT_IVALUE ||
        des->font_weight == NODEFINED_IVALUE) {
        des->font_weight = FONTWEIGHT_NORMAL;
    }

    if (des->font_size == INHERIT_VALUE ||
        des->font_size == NODEFINED_VALUE) {
        des->font_size = 12;
    }
}

MsvgPaintCtx *MsvgBuildPaintCtxInherited(MsvgElement *el)
{
    MsvgPaintCtx *des;
    MsvgElement *fath;

    if (!el || !el->pctx)  return NULL;

    des = MsvgNewPaintCtx(el->pctx);
    if (des == NULL) return NULL;

    fath = el->father;
    while (fath) {
        if (fath->pctx)
            MsvgProcPaintCtxInheritance(des, fath->pctx);
        fath = fath->father;
    }

    MsvgProcPaintCtxDefaults(des);

    return des;
}
