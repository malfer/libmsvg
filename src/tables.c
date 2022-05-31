/* tables.c
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

#include <string.h>
#include "msvg.h"

typedef struct {
    enum EID eid;            // element id
    char *ename;             // element name
    int chc;                 // element can have content (1=yes, 0=no)
    int isvirtual;           // element is virtual (1=yes, 0=no)
    int nsset;               // number of supported son element types
    enum EID sset[EID_LAST]; // list of supported son element types
} MsvgIdElement;

static MsvgIdElement supported_elements[] = {
    {EID_SVG, "svg", 0, 0, 11, {EID_DEFS, EID_G, EID_USE, EID_RECT, EID_CIRCLE,
        EID_ELLIPSE, EID_LINE, EID_POLYLINE, EID_POLYGON, EID_PATH, EID_TEXT} },
    {EID_DEFS, "defs", 0, 0, 11, {EID_G, EID_RECT, EID_CIRCLE, EID_ELLIPSE, EID_LINE,
        EID_POLYLINE, EID_POLYGON, EID_PATH, EID_TEXT, EID_LINEARGRADIENT,
        EID_RADIALGRADIENT} },
    {EID_G, "g", 0, 0, 11, {EID_DEFS, EID_G, EID_USE, EID_RECT, EID_CIRCLE,
        EID_ELLIPSE, EID_LINE, EID_POLYLINE, EID_POLYGON, EID_PATH, EID_TEXT} },
    {EID_USE, "use", 0, 0, 0},
    {EID_RECT, "rect", 0, 0, 0},
    {EID_CIRCLE, "circle", 0, 0, 0},
    {EID_ELLIPSE, "ellipse", 0, 0, 0},
    {EID_LINE, "line", 0, 0, 0},
    {EID_POLYLINE, "polyline", 0, 0, 0},
    {EID_POLYGON, "polygon", 0, 0, 0},
    {EID_PATH, "path", 0, 0, 0},
    {EID_TEXT, "text", 1, 0, 1, {EID_V_CONTENT} },
    {EID_LINEARGRADIENT, "linearGradient", 0, 0, 1, {EID_STOP} },
    {EID_RADIALGRADIENT, "radialGradient", 0, 0, 1, {EID_STOP} },
    {EID_STOP, "stop", 0, 0, 0},
    {EID_V_CONTENT, "v_content", 0, 1, 0}
};

enum EID MsvgFindElementId(const char *ename)
{
    int i;
    
    for (i=0; i<EID_LAST; i++) {
        if (strcmp(ename, supported_elements[i].ename) == 0)
            return supported_elements[i].eid;
    }
    return EID_NOTSUPPORTED;
}

char *MsvgFindElementName(enum EID eid)
{
    int i;
    
    for (i=0; i<EID_LAST; i++) {
        if (eid == supported_elements[i].eid)
            return supported_elements[i].ename;
    }
    return NULL;
}

int MsvgIsSupSonElement(enum EID fatherid, enum EID sonid)
{
    int i, j;
    
    for (i=0; i<EID_LAST; i++) {
        if (fatherid == supported_elements[i].eid) {
            for (j=0; j<supported_elements[i].nsset; j++) {
                if (supported_elements[i].sset[j] == sonid) return 1;
            }
            return 0;
        }
    }
    return 0;
}

int MsvgElementCanHaveContent(enum EID eid)
{
    int i;
    
    for (i=0; i<EID_LAST; i++) {
        if (eid == supported_elements[i].eid)
            return supported_elements[i].chc;
    }
    return 0;
}

int MsvgIsVirtualElement(enum EID eid)
{
    int i;
    
    for (i=0; i<EID_LAST; i++) {
        if (eid == supported_elements[i].eid)
            return supported_elements[i].isvirtual;
    }
    return 0;
}
