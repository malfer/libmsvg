/* attribut.c
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
#include <string.h>
#include "msvg.h"
#include "util.h"

static int addRawAttribute(MsvgElement *el, const char *key, const char *value)
{
    MsvgRawAttribute **dptr;
    MsvgRawAttribute *pattr;
    
    dptr = &(el->frattr);
    while (*dptr)
        dptr = &((*dptr)->nrattr);
    
    pattr = calloc(1, sizeof(MsvgRawAttribute));
    if (pattr == NULL) return 0;
    
    pattr->key = strdup(key);
    if (pattr->key == NULL) {
        free(pattr);
        return 0;
    }
    
    pattr->value = strdup(value);
    if (pattr->value == NULL) {
        free(pattr->key);
        free(pattr);
        return 0;
    }
    
    pattr->nrattr = NULL;
    
    *dptr = pattr;
    return 1;
}

int MsvgAddRawAttribute(MsvgElement *el, const char *key, const char *value)
{
    char *sdup, *token1, *token2;
    
    if (strcmp(key, "style") != 0)
        return addRawAttribute(el, key, value);

    // style is not a valid Tiny 1.2 parameter, but it is widelly used
    sdup = strdup(value);
    if (sdup == NULL) return 0;

    token1 = strtok(sdup, ":");
    while (token1) {
        token2 = strtok(NULL,";");
        if (token2) {
            if (!addRawAttribute(el, MsvgI_rmspaces(token1),
                                 MsvgI_rmspaces(token2))) {
                free(sdup);
                return 0;
            }
        } else {
            break;
        }
        token1 = strtok(NULL,":");
    }

    free(sdup);

    return 1;
}

int MsvgDelRawAttribute(MsvgElement *el, const char *key)
{
    MsvgRawAttribute **dptr;
    MsvgRawAttribute *nattr;
    
    
    dptr = &(el->frattr);
    while (*dptr) {
        if (strcmp((*dptr)->key,key) == 0) {
            if ((*dptr)->key) free((*dptr)->key);
            if ((*dptr)->value) free((*dptr)->value);
            nattr = (*dptr)->nrattr;
            free(*dptr);
            *dptr = nattr;
            return 1;
        }
        dptr = &((*dptr)->nrattr);
    }
    
    return 0;
}

int MsvgDelAllRawAttributes(MsvgElement *el)
{
    MsvgRawAttribute *cattr, *nattr;
    int deleted = 0;
    
    cattr = el->frattr;
    while (cattr) {
        if (cattr->key) free(cattr->key);
        if (cattr->value) free(cattr->value);
        nattr = cattr->nrattr;
        free(cattr);
        deleted++;
        cattr = nattr;
    }
    
    el->frattr = NULL;
    return deleted;
}

int MsvgDelAllTreeRawAttributes(MsvgElement *el)
{
    MsvgElement *ptr;
    int deleted = 0;
    
    deleted = MsvgDelAllRawAttributes(el);

    ptr = el->fson;
    while (ptr) {
        deleted += MsvgDelAllTreeRawAttributes(ptr);
        ptr = ptr->nsibling;
    }

    return deleted;
}

int MsvgCopyRawAttributes(MsvgElement *desel, MsvgElement *srcel)
{
    MsvgRawAttribute *cattr;
    int copied = 0;
    
    cattr = srcel->frattr;
    while (cattr) {
        copied += MsvgAddRawAttribute(desel, cattr->key, cattr->value);
        cattr = cattr->nrattr;
    }
    
    return copied;
}

int MsvgCopyCookedAttributes(MsvgElement *desel, MsvgElement *srcel)
{
    int i;

    if (srcel->eid != desel->eid) return 0;

    if (desel->id) free(desel->id);
    if (srcel->id) desel->id = strdup(srcel->id);
    desel->pctx = srcel->pctx;

    switch (srcel->eid) {
        case EID_SVG :
            *(desel->psvgattr) = *(srcel->psvgattr);
            break;
        case EID_DEFS :
            *(desel->pdefsattr) = *(srcel->pdefsattr);
            break;
        case EID_G :
            *(desel->pgattr) = *(srcel->pgattr);
            break;
        case EID_USE :
            if (desel->puseattr->refel) free(desel->puseattr->refel);
            *(desel->puseattr) = *(srcel->puseattr);
            if (srcel->puseattr->refel) {
                desel->puseattr->refel = strdup(srcel->puseattr->refel);
            }
            break;
        case EID_RECT :
            *(desel->prectattr) = *(srcel->prectattr);
            break;
        case EID_CIRCLE :
            *(desel->pcircleattr) = *(srcel->pcircleattr);
            break;
        case EID_ELLIPSE :
            *(desel->pellipseattr) = *(srcel->pellipseattr);
            break;
        case EID_LINE :
            *(desel->plineattr) = *(srcel->plineattr);
            break;
        case EID_POLYLINE :
            if (!MsvgAllocPointsToPolylineElement(desel,
                srcel->ppolylineattr->npoints)) return 0;
            desel->ppolylineattr->npoints = srcel->ppolylineattr->npoints;
            for (i=0; i< desel->ppolylineattr->npoints; i++) {
                desel->ppolylineattr->points[i*2] =
                    srcel->ppolylineattr->points[i*2];
                desel->ppolylineattr->points[i*2+1] =
                    srcel->ppolylineattr->points[i*2+1];
            }
            break;
        case EID_POLYGON :
            if (!MsvgAllocPointsToPolygonElement(desel,
                srcel->ppolygonattr->npoints)) return 0;
            desel->ppolygonattr->npoints = srcel->ppolygonattr->npoints;
            for (i=0; i< desel->ppolygonattr->npoints; i++) {
                desel->ppolygonattr->points[i*2] =
                    srcel->ppolygonattr->points[i*2];
                desel->ppolygonattr->points[i*2+1] =
                    srcel->ppolygonattr->points[i*2+1];
            }
            break;
        case EID_PATH :
            if (desel->ppathattr->sp) MsvgDestroySubPath(desel->ppathattr->sp);
            *(desel->ppathattr) = *(srcel->ppathattr);
            if (srcel->ppathattr->sp) {
                desel->ppathattr->sp = MsvgDupSubPath(srcel->ppathattr->sp);
            }
            break;
        case EID_TEXT :
            *(desel->ptextattr) = *(srcel->ptextattr);
            break;
        default :
            break;
    }

    return 1;
}
