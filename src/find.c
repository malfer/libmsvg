/* find.c
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

MsvgElement *MsvgFindFirstFather(MsvgElement *el)
{
    MsvgElement *f, *ff;

    if (el->father == NULL) return el;

    f = el->father;
    while(f) {
        ff = f;
        f = f->father;
    }

    return ff;
}

static void addCountsCookedTree(const MsvgElement *el, MsvgTreeCounts *tc)
{
    if (el->eid > EID_SVG && el->eid <= EID_LAST) {
        tc->nelem[el->eid] += 1;
        tc->totelem += 1;
        if (el->id) tc->totelwid += 1;
    }

    if (el->fson) addCountsCookedTree(el->fson, tc);

    if (el->nsibling) addCountsCookedTree(el->nsibling, tc);
}

void MsvgCalcCountsCookedTree(const MsvgElement *el, MsvgTreeCounts *tc)
{
    int i;

    for (i=0; i<=EID_LAST; i++)
        tc->nelem[i] = 0;
    tc->totelem = 0;
    tc->totelwid = 0;

    addCountsCookedTree(el, tc);
}

static char *findRawId(const MsvgElement *el)
{
    MsvgRawAttribute *pattr;
    
    pattr = el->frattr;
    while (pattr != NULL) {
        if (strcmp(pattr->key, "id") == 0) return pattr->value;
        else if (strcmp(pattr->key, "xml:id") == 0) return pattr->value;
        pattr = pattr->nrattr;
    }

    return NULL;
}

static void addCountsRawTree(const MsvgElement *el, MsvgTreeCounts *tc)
{
    if (el->eid > EID_SVG && el->eid <= EID_LAST) {
        tc->nelem[el->eid] += 1;
        tc->totelem += 1;
        if (findRawId(el)) tc->totelwid += 1;
    }

    if (el->fson) addCountsRawTree(el->fson, tc);

    if (el->nsibling) addCountsRawTree(el->nsibling, tc);
}

void MsvgCalcCountsRawTree(const MsvgElement *el, MsvgTreeCounts *tc)
{
    int i;

    for (i=0; i<=EID_LAST; i++)
        tc->nelem[i] = 0;
    tc->totelem = 0;
    tc->totelwid = 0;

    addCountsRawTree(el, tc);
}

MsvgElement *MsvgFindIdCookedTree(MsvgElement *el, char *id)
{
    MsvgElement *aux;

    if (el->eid > EID_SVG && el->id && strcmp(el->id, id) == 0)
        return el;
    
    if (el->fson) {
        aux = MsvgFindIdCookedTree(el->fson, id);
        if (aux) return aux;
    }

    if (el->nsibling) {
        aux = MsvgFindIdCookedTree(el->nsibling, id);
        if (aux) return aux;
    }

    return NULL;
}

MsvgElement *MsvgFindIdRawTree(MsvgElement *el, char *id)
{
    MsvgElement *aux;
    char *rid;

    if (el->eid > EID_SVG) {
        rid = findRawId(el);
        if (rid && strcmp(rid, id) == 0)
            return el;
    }
    
    if (el->fson) {
        aux = MsvgFindIdRawTree(el->fson, id);
        if (aux) return aux;
    }

    if (el->nsibling) {
        aux = MsvgFindIdRawTree(el->nsibling, id);
        if (aux) return aux;
    }

    return NULL;
}

static int cmpTableIdItem(const void *t1, const void *t2)
{
    MsvgTableIdItem *it1, *it2;

    it1 = (MsvgTableIdItem *)t1;
    it2 = (MsvgTableIdItem *)t2;

    return strcmp(it1->id, it2->id);
}

static void addTableIdItemCooked(MsvgElement *el, MsvgTableId *tid, int maxelem)
{
    if (tid->nelem >= maxelem) return;

    if (el->eid > EID_SVG && el->eid <= EID_LAST && el->id) {
        tid->item[tid->nelem].id = el->id;
        tid->item[tid->nelem].el = el;
        tid->nelem++;
    }

    if (el->fson) addTableIdItemCooked(el->fson, tid, maxelem);

    if (el->nsibling) addTableIdItemCooked(el->nsibling, tid, maxelem);
}

MsvgTableId *MsvgBuildTableIdCookedTree(MsvgElement *el)
{
    MsvgTreeCounts tc;
    MsvgTableId *tid;

    MsvgCalcCountsCookedTree(el, &tc);

    if (tc.totelwid < 1) return NULL;

    tid = (MsvgTableId *)malloc(sizeof(MsvgTableId)+
                                sizeof(MsvgTableIdItem)*(tc.totelwid-1));

    if (tid == NULL) return NULL;

    tid->nelem = 0;
    addTableIdItemCooked(el, tid, tc.totelwid);

    if (tid->nelem > 1) {
        qsort(&(tid->item[0]), tid->nelem,
              sizeof(MsvgTableIdItem), cmpTableIdItem);
    }

    return tid;
}

static void addTableIdItemRaw(MsvgElement *el, MsvgTableId *tid, int maxelem)
{
    char *rid;

    if (tid->nelem >= maxelem) return;

    if (el->eid > EID_SVG && el->eid <= EID_LAST) {
        rid = findRawId(el);
        if (rid) {
            tid->item[tid->nelem].id = rid;
            tid->item[tid->nelem].el = el;
            tid->nelem++;
        }
    }

    if (el->fson) addTableIdItemRaw(el->fson, tid, maxelem);

    if (el->nsibling) addTableIdItemRaw(el->nsibling, tid, maxelem);
}

MsvgTableId *MsvgBuildTableIdRawTree(MsvgElement *el)
{
    MsvgTreeCounts tc;
    MsvgTableId *tid;

    MsvgCalcCountsRawTree(el, &tc);

    if (tc.totelwid < 1) return NULL;

    tid = (MsvgTableId *)malloc(sizeof(MsvgTableId)+
                                sizeof(MsvgTableIdItem)*(tc.totelwid-1));

    if (tid == NULL) return NULL;

    tid->nelem = 0;
    addTableIdItemRaw(el, tid, tc.totelwid);

    if (tid->nelem > 1) {
        qsort(&(tid->item[0]), tid->nelem,
              sizeof(MsvgTableIdItem), cmpTableIdItem);
    }

    return tid;
}

void MsvgDestroyTableId(MsvgTableId *tid)
{
    free(tid);
}

MsvgElement *MsvgFindIdTableId(const MsvgTableId *tid, char *id)
{
    MsvgTableIdItem key, *found;

    key.id = id;

    found = bsearch(&key, &(tid->item[0]), tid->nelem,
                    sizeof(MsvgTableIdItem), cmpTableIdItem);

    if (found) return found->el;

    return NULL;
}
