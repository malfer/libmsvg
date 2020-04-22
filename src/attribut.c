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

int MsvgAddRawAttribute(MsvgElement *pelement, const char *key, const char *value)
{
    MsvgRawAttribute **dptr;
    MsvgRawAttribute *pattr;
    
    dptr = &(pelement->frattr);
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


int MsvgDelRawAttribute(MsvgElement *pelement, const char *key)
{
    MsvgRawAttribute **dptr;
    MsvgRawAttribute *nattr;
    
    
    dptr = &(pelement->frattr);
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

int MsvgDelAllRawAttributes(MsvgElement *pelement)
{
    MsvgRawAttribute *cattr, *nattr;
    int deleted = 0;
    
    cattr = pelement->frattr;
    while (cattr) {
        if (cattr->key) free(cattr->key);
        if (cattr->value) free(cattr->value);
        nattr = cattr->nrattr;
        free(cattr);
        deleted++;
        cattr = nattr;
    }
    
    pelement->frattr = NULL;
    return deleted;
}

int MsvgCopyRawAttributes(MsvgElement *srcelement, MsvgElement *deselement)
{
    MsvgRawAttribute *cattr;
    int copied = 0;
    
    cattr = srcelement->frattr;
    while (cattr) {
        copied += MsvgAddRawAttribute(deselement, cattr->key, cattr->value);
        cattr = cattr->nrattr;
    }
    
    return copied;
}
