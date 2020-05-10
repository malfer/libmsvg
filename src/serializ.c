/* serializ.c
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

static void process_generic_pctx(MsvgPaintCtx *des, MsvgPaintCtx *fath,
                                 MsvgPaintCtx *son)
{
    if (son->fill == INHERIT_COLOR || son->fill == NODEFINED_COLOR) {
        des->fill = fath->fill;
    } else {
        des->fill = son->fill;
    }

    if (son->fill_opacity == INHERIT_VALUE ||
        son->fill_opacity == NODEFINED_VALUE) {
        des->fill_opacity = fath->fill_opacity;
    } else {
        des->fill_opacity = son->fill_opacity;
    }

    if (son->stroke == INHERIT_COLOR || son->stroke == NODEFINED_COLOR) {
        des->stroke = fath->stroke;
    } else {
        des->stroke = son->stroke;
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
}

static void process_drawel_pctx(MsvgPaintCtx *des, MsvgPaintCtx *fath,
                                MsvgPaintCtx *son)
{
    process_generic_pctx(des, fath, son);

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
}

static void process_container(MsvgElement *el, MsvgSerUserFn sufn, MsvgPaintCtx *fath)
{
    MsvgPaintCtx mypctx, sonpctx;
    MsvgElement *pel;

    if (fath)
        process_generic_pctx(&mypctx, fath, &(el->pctx));
    else
        mypctx = el->pctx;
    
    pel = el->fson;
    while (pel) {
        switch (pel->eid) {
            case EID_SVG :
                break;
            case EID_G :
                process_container(pel, sufn, &mypctx);
                break;
            case EID_DEFS :
                break;
            case EID_USE :
                break; // TODO
            case EID_RECT :
            case EID_CIRCLE :
            case EID_ELLIPSE :
            case EID_LINE :
            case EID_POLYLINE :
            case EID_POLYGON :
            case EID_TEXT :
                process_drawel_pctx(&sonpctx, &mypctx, &(pel->pctx));
                sufn(pel, &sonpctx);
                break;
            default :
                break;
        }
        pel = pel->nsibling;
    }
}

int MsvgSerCookedTree(MsvgElement *root, MsvgSerUserFn sufn)
{
    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    if (root->psvgattr->tree_type != COOKED_SVGTREE) return 0;

    process_container(root, sufn, NULL);
    
    return 1;
}
