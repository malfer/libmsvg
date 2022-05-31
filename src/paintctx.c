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
    pctx->font_family = NODEFINED_IVALUE;
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

    *des = *src;
    if (src->fill_iri) des->fill_iri = strdup(src->fill_iri);
    if (src->stroke_iri) des->stroke_iri = strdup(src->stroke_iri);
}

void MsvgDestroyPaintCtx(MsvgPaintCtx *pctx)
{
    if (!pctx) return;
    if (pctx->fill_iri) free(pctx->fill_iri);
    if (pctx->stroke_iri) free(pctx->stroke_iri);
    free(pctx);
}
