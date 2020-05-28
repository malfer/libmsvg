/* manielem.c
 * 
 * libmsvg, a minimal library to read and write svg files
 * 
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include "msvg.h"

void MsvgPruneElement(MsvgElement *el)
{
    MsvgElement *father;
    
    father = el->father;
    if (father == NULL) return; // already pruned
    
    el->father = NULL;
    if (el->psibling == NULL) { // first sibling
        father->fson = el->nsibling;
        if (el->nsibling != NULL) {
            el->nsibling->psibling = NULL;
            el->nsibling = NULL;
        }
    } else {
        el->psibling->nsibling = el->nsibling;
        if (el->nsibling != NULL) {
            el->nsibling->psibling = el->psibling;
            el->nsibling->psibling = NULL;
            el->nsibling = NULL;
        }
        el->psibling = NULL;
    }
}

static void MsvgFreeElement(MsvgElement *el)
{
    switch (el->eid) {
        case EID_SVG :
            free(el->psvgattr);
            break;
        case EID_DEFS :
            free(el->pdefsattr);
            break;
        case EID_G :
            free(el->pgattr);
            break;
        case EID_USE :
            if (el->puseattr->refel) free(el->puseattr->refel);
            free(el->puseattr);
            break;
        case EID_RECT :
            free(el->prectattr);
            break;
        case EID_CIRCLE :
            free(el->pcircleattr);
            break;
        case EID_ELLIPSE :
            free(el->pellipseattr);
            break;
        case EID_LINE :
            free(el->plineattr);
            break;
        case EID_POLYLINE :
            if (el->ppolylineattr->points) free(el->ppolylineattr->points);
            free(el->ppolylineattr);
            break;
        case EID_POLYGON :
            if (el->ppolygonattr->points) free(el->ppolygonattr->points);
            free(el->ppolygonattr);
            break;
        case EID_PATH :
            if (el->ppathattr->sp) MsvgDestroySubPath(el->ppathattr->sp);
            free(el->ppathattr);
            break;
        case EID_TEXT :
            if (el->ptextattr->font_family) free(el->ptextattr->font_family);
            free(el->ptextattr);
            break;
        default :
            break; // TODO: test error
    }

    if (el->id) free(el->id);
    free(el);
}

void MsvgDeleteElement(MsvgElement *el)
{
    MsvgPruneElement(el);
    
    while (el->fson != NULL) {
        MsvgDeleteElement(el->fson);
    }
    
    MsvgDelAllRawAttributes(el);
    MsvgDelContents(el);
    MsvgFreeElement(el);
}

int MsvgInsertSonElement(MsvgElement *el, MsvgElement *father)
{
    MsvgElement *ptr;
    
    if (father == NULL) return 0;
    if (!MsvgIsSupSonElementId(father->eid, el->eid)) return 0;
    
    el->father = father;
    el->psibling = el->nsibling = NULL;
    if (father->fson == NULL) {
        father->fson = el;
    } else {
        ptr = father->fson;
        while (ptr->nsibling != NULL)
            ptr = ptr->nsibling;
        ptr->nsibling = el;
        el->psibling = ptr;
    }
    
    return 1;
}

int MsvgInsertPSiblingElement(MsvgElement *el, MsvgElement *sibling)
{
    if (sibling == NULL) return 0;
    
    el->father = sibling->father;
    
    if (sibling->psibling == NULL) { // first son
        if (el->father != NULL) el->father->fson = el;
        el->psibling = NULL;
        el->nsibling = sibling;
        sibling->psibling = el;
    } else {
        el->psibling = sibling->psibling;
        sibling->psibling->nsibling = el;
        el->nsibling = sibling;
        sibling->psibling = el;
    }
    
    return 1;
}

int MsvgInsertNSiblingElement(MsvgElement *el, MsvgElement *sibling)
{
    if (sibling == NULL) return 0;
    
    el->father = sibling->father;
    
    el->nsibling = sibling->nsibling;
    el->nsibling = el;
    el->psibling = sibling;
    if (el->nsibling != NULL)
        el->nsibling->psibling = el;
    
    return 1;
}

MsvgElement *MsvgDupElement(MsvgElement *el)
{
    MsvgElement *newel, *ptrnew, *ptrold;

    newel = MsvgNewElement(el->eid, NULL);
    if (newel == NULL) return NULL;

    MsvgCopyRawAttributes(newel, el);
    MsvgCopyCookedAttributes(newel, el);
    MsvgCopyContents(newel, el);

    ptrold = el->fson;
    while (ptrold) {
        ptrnew = MsvgDupElement(ptrold);
        if (ptrnew) MsvgInsertSonElement(ptrnew, newel);
        ptrold = ptrold->nsibling;
    }

    return newel;
}
