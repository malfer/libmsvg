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

static void process_container(MsvgElement *el, SerData *sd,
                              MsvgPaintCtx *fath, int onlyfirstson);

static void build_bps(MsvgPaintCtx *pctx, SerData *sd)
{
    MsvgElement *refel;

    if (pctx->fill == IRI_COLOR) {
        refel = MsvgFindIdTableId(sd->tid, pctx->fill_iri);
        if (refel) pctx->fill_bps = MsvgNewBPServer(refel);
    }
    if (pctx->stroke == IRI_COLOR) {
        refel = MsvgFindIdTableId(sd->tid, pctx->stroke_iri);
        if (refel) pctx->stroke_bps = MsvgNewBPServer(refel);
    }
}

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

    mypctx = MsvgNewPaintCtx(el->pctx);
    if (mypctx == NULL) return;

    if (fath) MsvgProcPaintCtxInheritance(mypctx, fath);
    
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
                sonpctx = MsvgNewPaintCtx(pel->pctx);
                if (sonpctx) {
                    MsvgProcPaintCtxInheritance(sonpctx, mypctx);
                    MsvgProcPaintCtxDefaults(sonpctx);
                    build_bps(sonpctx, sd);
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
