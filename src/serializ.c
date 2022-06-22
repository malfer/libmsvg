/* serializ.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020-2022 Mariano Alvarez Fernandez
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
#include "util.h"

typedef struct {
    MsvgSerUserFn sufn;
    MsvgTableId *tid;
    int nested_use;
    void *udata;
} SerData;

static MsvgPaintCtx *process_pctx_inheritance(const MsvgPaintCtx *fath,
                                              const MsvgPaintCtx *son)
{
    MsvgPaintCtx *des;

    des = MsvgNewPaintCtx(NULL);
    if (des == NULL) return NULL;

    if (son->fill == INHERIT_COLOR || son->fill == NODEFINED_COLOR) {
        des->fill = fath->fill;
        if (fath->fill_iri) des->fill_iri = strdup(fath->fill_iri);
    } else {
        des->fill = son->fill;
        if (son->fill_iri) des->fill_iri = strdup(son->fill_iri);
    }

    if (son->fill_opacity == INHERIT_VALUE ||
        son->fill_opacity == NODEFINED_VALUE) {
        des->fill_opacity = fath->fill_opacity;
    } else {
        des->fill_opacity = son->fill_opacity;
    }

    if (son->stroke == INHERIT_COLOR || son->stroke == NODEFINED_COLOR) {
        des->stroke = fath->stroke;
        if (fath->stroke_iri) des->stroke_iri = strdup(fath->stroke_iri);
    } else {
        des->stroke = son->stroke;
        if (son->stroke_iri) des->stroke_iri = strdup(son->stroke_iri);
    }

    if (son->stroke_width == INHERIT_VALUE ||
        son->stroke_width == NODEFINED_VALUE) {
        des->stroke_width = fath->stroke_width;
    } else {
        des->stroke_width = son->stroke_width;
    }

    if (son->stroke_opacity == INHERIT_VALUE ||
        son->stroke_opacity == NODEFINED_VALUE) {
        des->stroke_opacity = fath->stroke_opacity;
    } else {
        des->stroke_opacity = son->stroke_opacity;
    }

    TMMpy(&(des->tmatrix), &(fath->tmatrix), &(son->tmatrix));

    if (son->text_anchor == INHERIT_IVALUE ||
        son->text_anchor == NODEFINED_IVALUE) {
        des->text_anchor = fath->text_anchor;
    } else {
        des->text_anchor = son->text_anchor;
    }

    if (son->ifont_family == INHERIT_IVALUE ||
        son->ifont_family == NODEFINED_IVALUE) {
        if (fath->sfont_family) des->sfont_family = strdup(fath->sfont_family);
        des->ifont_family = fath->ifont_family;
    } else {
        if (son->sfont_family) des->sfont_family = strdup(son->sfont_family);
        des->ifont_family = son->ifont_family;
    }

    if (son->font_style == INHERIT_IVALUE ||
        son->font_style == NODEFINED_IVALUE) {
        des->font_style = fath->font_style;
    } else {
        des->font_style = son->font_style;
    }

    if (son->font_weight == INHERIT_IVALUE ||
        son->font_weight == NODEFINED_IVALUE) {
        des->font_weight = fath->font_weight;
    } else {
        des->font_weight = son->font_weight;
    }

    if (son->font_size == INHERIT_VALUE ||
        son->font_size == NODEFINED_VALUE) {
        des->font_size = fath->font_size;
    } else {
        des->font_size = son->font_size;
    }

    return des;
}

static void process_pctx_defaults(MsvgPaintCtx *des)
{
    /* now take initial values if not defined */
    
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

static void process_container(MsvgElement *el, SerData *sd,
                              MsvgPaintCtx *fath, int onlyfirstson);

static void process_use(MsvgElement *el, SerData *sd, MsvgPaintCtx *fath)
{
    MsvgElement *refel, *ghostg;
    TMatrix uset;

    if (sd->nested_use >= MAX_NESTED_USE_ELEMENT) return;
    if (sd->tid == NULL) return;

    refel = MsvgFindIdTableId(sd->tid, el->puseattr->refel);
    if (refel == NULL) return;

    // create a ghost G element
    ghostg = MsvgNewElement(EID_G, NULL);
    if (ghostg == NULL) return;

    sd->nested_use += 1;

    MsvgCopyPaintCtx(ghostg->pctx, el->pctx);

    TMSetTranslation(&uset, el->puseattr->x, el->puseattr->y);
    TMMpy(&(ghostg->pctx->tmatrix), &(el->pctx->tmatrix), &uset);

    ghostg->fson = refel;
    process_container(ghostg, sd, fath, 1);
    ghostg->fson = NULL;
    MsvgDeleteElement(ghostg);

    sd->nested_use -= 1;
}

static void process_container(MsvgElement *el, SerData *sd,
                              MsvgPaintCtx *fath, int onlyfirstson)
{
    MsvgPaintCtx *mypctx = NULL, *sonpctx = NULL;
    MsvgElement *pel;

    if (fath)
        mypctx = process_pctx_inheritance(fath, el->pctx);
    else
        mypctx = MsvgNewPaintCtx(el->pctx);

    if (mypctx == NULL) return;
    
    pel = el->fson;
    while (pel) {
        switch (pel->eid) {
            case EID_SVG :
                break;
            case EID_G :
                process_container(pel, sd, mypctx, 0);
                break;
            case EID_DEFS :
                break;
            case EID_USE :
                process_use(pel, sd, mypctx);
                break;
            case EID_RECT :
            case EID_CIRCLE :
            case EID_ELLIPSE :
            case EID_LINE :
            case EID_POLYLINE :
            case EID_POLYGON :
            case EID_PATH :
            case EID_TEXT :
                sonpctx = process_pctx_inheritance(mypctx, pel->pctx);
                if (sonpctx) {
                    process_pctx_defaults(sonpctx);
                    sd->sufn(pel, sonpctx, sd->udata);
                    MsvgDestroyPaintCtx(sonpctx);
                }
                break;
            default :
                break;
        }
        if (onlyfirstson) pel = NULL;
        else pel = pel->nsibling;
    }

    MsvgDestroyPaintCtx(mypctx);
}

int MsvgSerCookedTree(MsvgElement *root, MsvgSerUserFn sufn, void *udata)
{
    SerData sd;

    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;

    sd.sufn = sufn;
    sd.tid = MsvgBuildTableIdCookedTree(root);
    sd.nested_use = 0;
    sd.udata = udata;

    process_container(root, &sd, NULL, 0);

    if (sd.tid) MsvgDestroyTableId(sd.tid);

    return 1;
}
