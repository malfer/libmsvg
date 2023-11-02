/* wtsvgf.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
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

#include <stdio.h>
#include <string.h>
#include "msvg.h"

static void writeLabelElement(FILE *f, enum EID eid, int start, int depth)
{
    int i;
    
    for (i=0; i<depth; i++)
        fputs("  ", f);
    
    if (start)
        fputs("<", f);
    else
        fputs("</", f);
    
    fputs(MsvgFindElementName(eid), f);
    
    if (!start)
        fputs(">\n", f);
}

static void writeString(FILE *f, char *s)
{
    while (*s) {
        switch (*s) {
            case '<' :
                fputs("&lt;", f);
                break;
            case '>' :
                fputs("&gt;", f);
                break;
            case '&' :
                fputs("&amp;", f);
                break;
            case '"' :
                fputs("&quot;", f);
                break;
            case '\'' :
                fputs("&apos;", f);
                break;
            default:
                fputc(*s, f);
        }
        s++;
    }
}

static void writeContent(FILE *f, char *s, int depth)
{
    int i;

    for (i=0; i<depth; i++)
        fputs("  ", f);

    writeString(f, s);
    fputs("\n", f);
}

static void writeElement(FILE *f, MsvgElement *el, int depth)
{
    MsvgRawAttribute *pattr;

    if (el->eid == EID_V_COMMENT) {
        for (int i=0; i<depth; i++) fputs("  ", f);
        fputs("<!--", f);
        if (el->fcontent != NULL) fputs(el->fcontent->s, f);
        fputs("-->\n", f);
        if (el->nsibling != NULL)
            writeElement(f, el->nsibling, depth);
        return;
    }

    writeLabelElement(f, el->eid, 1, depth);
    if (el->frattr != NULL) {
        pattr = el->frattr;
        while (pattr != NULL) {
            fprintf(f, " %s=\"", pattr->key);
            writeString(f, pattr->value);
            fputs("\"", f);
            pattr = pattr->nrattr;
        }
    }
    
    if (el->fson != NULL || el->fcontent != NULL) {
        fputs(">\n", f);
        if (el->fcontent != NULL) {
            writeContent(f, el->fcontent->s, depth);
        }
        if (el->fson != NULL) writeElement(f, el->fson, depth+1);
        writeLabelElement(f, el->eid, 0, depth);
    } else {
        fputs(" />\n", f);
    }
    
    if (el->nsibling != NULL)
        writeElement(f, el->nsibling, depth);
}

int MsvgWriteSvgFile(MsvgElement *root, const char *fname)
{
    FILE *f;
    
    if (root == NULL) return 0;
    if (root->eid != EID_SVG) return 0;
    
    f = fopen(fname, "wt");
    if (f == NULL) return 0;
    
    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", f);
    writeElement(f, root, 0);
    
    fclose(f);
    return 1;
}
