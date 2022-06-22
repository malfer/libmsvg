/* bfont.c
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
#include "util.h"

static int cmpGlyph(const void *t1, const void *t2)
{
    MsvgBGlyph *g1, *g2;

    g1 = (MsvgBGlyph *)t1;
    g2 = (MsvgBGlyph *)t2;

    if (g1->unicode < g2->unicode) return -1;
    if (g1->unicode == g2->unicode) return 0;
    return 1;
}

MsvgBFont *MsvgNewBFont(MsvgElement *el)
{
    MsvgBFont *bfont;
    MsvgElement *nson;
    int nff = 0;
    int nmg = 0;
    int ng = 0;

    if (el->eid != EID_FONT) return NULL;

    nson = el->fson;
    while (nson) {
        if (nson->eid == EID_FONTFACE) {
            if (nson->pfontfaceattr->sfont_family == NULL) return NULL;
            nff++;
        } else if (nson->eid == EID_MISSINGGLYPH) {
            nmg++;
        } else if (nson->eid == EID_GLYPH) {
            ng++;
        }
        nson = nson->nsibling;
    }

    if (nff != 1 || ng < 1) return NULL;

    bfont = calloc(1, sizeof(MsvgBFont) + sizeof(MsvgBGlyph) * ng);
    if (bfont == NULL) return NULL;

    bfont->horiz_adv_x = el->pfontattr->horiz_adv_x;
    bfont->num_glyphs = 0;
    bfont->units_per_em = 1000.0;

    nson = el->fson;
    while (nson) {
        if (nson->eid == EID_FONTFACE) {
            bfont->sfont_family = strdup(nson->pfontfaceattr->sfont_family);
            bfont->ifont_family = nson->pfontfaceattr->ifont_family;
            bfont->font_style = nson->pfontfaceattr->font_style;
            bfont->font_weight = nson->pfontfaceattr->font_weight;
            bfont->units_per_em = nson->pfontfaceattr->units_per_em;
            bfont->ascent = nson->pfontfaceattr->ascent;
            bfont->descent = nson->pfontfaceattr->descent;
        } else if (nson->eid == EID_MISSINGGLYPH) {
            bfont->missing.horiz_adv_x = nson->pglyphattr->horiz_adv_x;
            bfont->missing.sp = MsvgDupSubPath(nson->pglyphattr->sp);
        } else if (nson->eid == EID_GLYPH) {
            if (bfont->num_glyphs < ng) {
                bfont->glyph[bfont->num_glyphs].unicode = nson->pglyphattr->unicode;
                bfont->glyph[bfont->num_glyphs].horiz_adv_x = nson->pglyphattr->horiz_adv_x;
                bfont->glyph[bfont->num_glyphs].sp = MsvgDupSubPath(nson->pglyphattr->sp);
                bfont->num_glyphs++;
            }
        }
        nson = nson->nsibling;
    }

    qsort(&(bfont->glyph[0]), bfont->num_glyphs, sizeof(MsvgBGlyph), cmpGlyph);

    return bfont;
}

void MsvgDestroyBFont(MsvgBFont *bfont)
{
    int i;

    if (bfont->sfont_family) free(bfont->sfont_family);
    if (bfont->missing.sp) MsvgDestroySubPath(bfont->missing.sp);
    for (i=0; i<bfont->num_glyphs; i++) {
        if (bfont->glyph[i].sp) MsvgDestroySubPath(bfont->glyph[i].sp);
    }
    free(bfont);
}

MsvgElement *MsvgCharToPath(long unicode, double font_size, double *advx, MsvgBFont *bfont)
{
    MsvgElement *path;
    MsvgBGlyph key, *found;
    TMatrix revy = {1, 0, 0, -1, 0, 0};
    TMatrix scale, final;
    double dscale;
    MsvgSubPath *sp;
    int i;

    path = MsvgNewElement(EID_PATH, NULL);
    if (path == NULL) return NULL;

    key.unicode = unicode;

    found = bsearch(&key, &(bfont->glyph[0]), bfont->num_glyphs,
                    sizeof(MsvgBGlyph), cmpGlyph);

    dscale = font_size / bfont->units_per_em;

    if (!found) {
        path->ppathattr->sp = MsvgDupSubPath(bfont->missing.sp);
        *advx = bfont->missing.horiz_adv_x;
    } else {
        path->ppathattr->sp = MsvgDupSubPath(found->sp);
        *advx = found->horiz_adv_x;
    }
    if (*advx == NODEFINED_VALUE) *advx = bfont->horiz_adv_x;
    *advx *= dscale;

    TMSetScaling(&scale, dscale, dscale);
    TMMpy(&final, &revy, &scale);

    sp = path->ppathattr->sp;
    while (sp) {
        for (i=0; i<sp->npoints; i++) {
            TMTransformCoord(&(sp->spp[i].x), &(sp->spp[i].y), &final);
        }
        sp = sp->next;
    }

    return path;
}

MsvgElement *MsvgTextToPathGroup(MsvgElement *el, MsvgBFont *bfont)
{
    MsvgElement *group, *path;
    TMatrix trans;
    MsvgSubPath *sp;
    unsigned char *p;
    double x, y, advx;
    int i, nb;
    long ucp;

    if (el->eid != EID_TEXT) return NULL;
    if (el->fcontent == NULL) return NULL;

    group = MsvgNewElement(EID_G, NULL);
    if (group == NULL) return NULL;

    MsvgCopyPaintCtx(group->pctx, el->pctx);

    x = el->ptextattr->x;
    y = el->ptextattr->y;
    p = (unsigned char *)el->fcontent->s;

    while (*p) {
        ucp = MsvgI_NextUCPfromUTF8Str(p, &nb);
        path = MsvgCharToPath(ucp, group->pctx->font_size, &advx, bfont);
        if (path) {
            TMSetTranslation(&trans, x, y);
            sp = path->ppathattr->sp;
            while (sp) {
                for (i=0; i<sp->npoints; i++) {
                    TMTransformCoord(&(sp->spp[i].x), &(sp->spp[i].y), &trans);
                }
                sp = sp->next;
            }
            x += advx;
            MsvgInsertSonElement(path, group);
        }
        p += nb;
    }

    // we don't need the text attributes now
    MsvgUndefPaintCtxTextAttr(group->pctx);

    return group;
}
