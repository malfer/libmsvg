/* content.c
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
