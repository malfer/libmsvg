/* content.c
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

#include <stdlib.h>
#include <string.h>
#include "msvg.h"

int MsvgAddContent(MsvgElement *el, int len, const char *cnt)
{
    MsvgContent *pcnt;
    int olen, nlen;
    
    if (el->fcontent == NULL) {
        pcnt = calloc(1, sizeof(MsvgContent)+len*sizeof(char));
        if (pcnt == NULL) return 0;
        pcnt->len = len;
        strncpy(pcnt->s, cnt, len);
        pcnt->s[len] = '\0';
        el->fcontent = pcnt;
    } else {
        olen = el->fcontent->len;
        nlen = olen + len;
        pcnt = realloc(el->fcontent, sizeof(MsvgContent)+nlen*sizeof(char));
        if (pcnt == NULL) return 0;
        pcnt->len = nlen;
        strncpy(&(pcnt->s[olen]), cnt, len);
        pcnt->s[nlen] = '\0';
        el->fcontent = pcnt;
    }

    return 1;
}

int MsvgDelContents(MsvgElement *el)
{
    if (el->fcontent != NULL) {
        free(el->fcontent);
        return 1;
    }

    return 0;
}

int MsvgCopyContents(MsvgElement *desel, MsvgElement *srcel)
{
    if (srcel->fcontent == NULL) return 0;

    if (desel->fcontent != NULL) MsvgDelContents(desel);

    return MsvgAddContent(desel, srcel->fcontent->len, srcel->fcontent->s);;
}

// linked list of contents version
/*
int MsvgAddContent(MsvgElement *el, int len, char *cnt)
{
    MsvgContent **dpcnt;
    MsvgContent *pcnt;
    
    dpcnt = &(el->fcontent);
    while (*dpcnt)
        dpcnt = &((*dpcnt)->ncontent);
    
    pcnt = calloc(1, sizeof(MsvgContent)+len*sizeof(char));
    if (pcnt == NULL) return 0;

    pcnt->len = len;
    strncpy(pcnt->s, cnt, len);
    pcnt->s[len] = '\0';

    pcnt->ncontent = NULL;
    
    *dpcnt = pcnt;
    return 1;
}

int MsvgDelContents(MsvgElement *el)
{
    MsvgContent *pcnt, *paux;
    int ndel = 0;

    pcnt = el->fcontent;
    while (pcnt) {
        paux = pcnt->ncontent;
        free(pcnt);
        pcnt = paux;
        ndel++;
    }

    el->fcontent = NULL;
    return ndel;
}

int MsvgCopyContents(MsvgElement *desel, MsvgElement *srcel)
{
    MsvgContent *cnt;
    int copied = 0;
    
    cnt = srcel->fcontent;
    while (cnt) {
        copied += MsvgAddContent(desel, cnt->len, cnt->s);
        cnt = cnt->ncontent;
    }
    
    return copied;
}
*/
