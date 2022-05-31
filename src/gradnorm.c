/* gradnorm.c
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

static void inheritAttribute(char *key, MsvgElement *el, const MsvgElement *rel)
{
    char *value;

    value = MsvgFindRawAttribute(el, key);
    if (value) return;
    value = MsvgFindRawAttribute(rel, key);
    if (value) MsvgAddRawAttribute(el, key, value);
}

static void inheritStops(MsvgElement *el, const MsvgElement *rel)
{
    MsvgElement *stopel, *newstop;

    if (el->fson) return;

    stopel = rel->fson;
    while (stopel) {
        newstop = MsvgDupElement(stopel);
        if (newstop) MsvgInsertSonElement(newstop, el);
        MsvgDelRawAttribute(newstop, "id");
        MsvgDelRawAttribute(newstop, "xml:id");
        stopel = stopel->nsibling;
    }
}

static int normalizeGradient(MsvgElement *el, const MsvgTableId *tid)
{
    int ngn = 0;
    MsvgElement *rel;
    char *value;

    if (el->eid != EID_LINEARGRADIENT && el->eid != EID_RADIALGRADIENT) return 0;

    value = MsvgFindRawAttribute(el, "xlink:href");
    if (value && value[0] == '#') {
        rel = MsvgFindIdTableId(tid, &(value[1]));
        if (rel != NULL) {
            ngn += normalizeGradient(rel, tid);
            inheritAttribute("gradientUnits", el, rel);
            if (el->eid == EID_LINEARGRADIENT) {
                inheritAttribute("x1", el, rel);
                inheritAttribute("y1", el, rel);
                inheritAttribute("x2", el, rel);
                inheritAttribute("y2", el, rel);
            } else {
                inheritAttribute("cx", el, rel);
                inheritAttribute("cy", el, rel);
                inheritAttribute("r", el, rel);
            }
            inheritStops(el, rel);
            ngn++;
        }
        MsvgDelRawAttribute(el, "xlink:href");
    }

    return ngn;
}

static int normalize(MsvgElement *el)
{
    int ngn = 0;
    MsvgTableId *tid;
    MsvgElement *pel;

    if (el->eid != EID_DEFS) return 0;

    tid = MsvgBuildTableIdRawTree(el);
    if (tid == NULL) return 0;

    pel = el->fson;
    while (pel) {
        if (pel->eid == EID_LINEARGRADIENT || pel->eid == EID_RADIALGRADIENT) {
            ngn += normalizeGradient(pel, tid);
        }
        pel = pel->nsibling;
    }

    return ngn;
}

int MsvgNormalizeRawGradients(MsvgElement *el)
{
    int ngn = 0;
    MsvgElement *pel;

    if (el->eid != EID_SVG) return 0;

    pel = el->fson;
    while (pel) {
        if (pel->eid == EID_DEFS) {
            ngn += normalize(pel);
        }
        pel = pel->nsibling;
    }

    return ngn;
}
