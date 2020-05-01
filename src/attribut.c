/* attribut.c
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

int MsvgAddRawAttribute(MsvgElement *el, const char *key, const char *value)
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
        case EID_G :
            *(desel->pgattr) = *(srcel->pgattr);
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
        default :
            break;
    }

    return 1;
}
