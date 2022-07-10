/* bfontlib.c
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
#include "util.h"

#define MAX_BFONTS 20
#define NO_DEFAULT -1

static MsvgBFont *bfontlib[MAX_BFONTS];
static int num_bfonts = 0;
static int default_bfont = NO_DEFAULT;

static void findfonts(MsvgElement *el, void *udata)
{
    int *nfontadded;

    if (num_bfonts >= MAX_BFONTS) return;

    nfontadded = (int *)udata;

    if (el->eid == EID_FONT) {
        bfontlib[num_bfonts] = MsvgNewBFont(el);
        if (bfontlib[num_bfonts]) {
            num_bfonts++;
            (*nfontadded)++;
        }
    }
}

int MsvgBFontLibLoad(MsvgElement *el)
{
    int nfontadded = 0;

    if (num_bfonts >= MAX_BFONTS) return 0;

    MsvgWalkTree(el, findfonts, &nfontadded);

    return nfontadded;
}

int MsvgBFontLibLoadFromFile(char *fname)
{
    MsvgElement *root;
    int nfontadded = 0;
    int error;

    if (num_bfonts >= MAX_BFONTS) return 0;

    root = MsvgReadSvgFile(fname, &error);
    if (root == NULL) return 0;
    MsvgRaw2CookedTree(root);

    nfontadded = MsvgBFontLibLoad(root);
    MsvgDeleteElement(root);
    return nfontadded;
}

MsvgBFont *MsvgBFontLibFind(char *sfont_family, int ifont_family)
{
    int i;

    if (num_bfonts < 1) return NULL;

    if (sfont_family) {
        for (i=0; i<num_bfonts; i++) {
            if (strstr(sfont_family, bfontlib[i]->sfont_family) != NULL)
                return bfontlib[i];
        }
    }

    for (i=0; i<num_bfonts; i++) {
        if (ifont_family == bfontlib[i]->ifont_family)
            return bfontlib[i];
    }

    for (i=0; i<num_bfonts; i++) {
        if (bfontlib[i]->ifont_family == FONTFAMILY_SANS)
            return bfontlib[i];
    }

    if (default_bfont >= 0 && default_bfont <= num_bfonts)
        return bfontlib[default_bfont];

    return NULL;
}

void MsvgBFontLibFree(void)
{
    int i;

    for (i=0; i<num_bfonts; i++) {
        MsvgDestroyBFont(bfontlib[i]);
    }
    num_bfonts = 0;
}

MsvgElement *MsvgTextToPathGroupUsingBFontLib(MsvgElement *el)
{
    MsvgBFont *bfont;
    int ifont_family;
    char *sfont_family;

    MsvgGetInheritedFontFamily(el, &ifont_family, &sfont_family);
    bfont = MsvgBFontLibFind(sfont_family, ifont_family);
    if (bfont == NULL) return NULL;

    return MsvgTextToPathGroup(el, bfont);
}

int MsvgBFontLibGetNumOfFonts(void)
{
    return num_bfonts;
}

MsvgBFont *MsvgBFontLibGetBFontByNumber(int nfont)
{
    if (nfont < 0 || nfont >= num_bfonts) return NULL;
    return bfontlib[nfont];
}

void MsvgBFontLibSetDefaultBfont(int nfont)
{
    if (nfont >= 0 && nfont <= num_bfonts)
        default_bfont = nfont;
    else
        default_bfont = NO_DEFAULT;
}
