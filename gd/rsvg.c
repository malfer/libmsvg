/* rsvg.c ---- test read svg for GD
 * 
 * This is a dirty hack to test the libmsvg librarie with the GD
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * Copyright (C) 2022 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <gd.h>
#include <msvg.h>
#include "rendgd.h"

#define TESTFILE_IN "../svgpics/test.svg"
#define TESTFILE_OUT "test.png"

static int TestFunc(gdImagePtr im, GDSVGDrawMode *sdm)
{
    MsvgElement *root;
    int error;
    
    root = MsvgReadSvgFile(TESTFILE_IN, &error);
    if (root == NULL) {
        printf("Error %d reading %s\n", error, TESTFILE_IN);
        return 1;
    }
    if (!MsvgRaw2CookedTree(root)) return 1;
    GDDrawSVGtree(root, sdm, im);
    MsvgDeleteElement(root);
    return 0;
}

int main(int argc,char **argv)
{
    gdImagePtr im;
    GDSVGDrawMode sdm = {SVGDRAWMODE_FIT, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0, 0};
    FILE *pngout;
    int ret;

    im = gdImageCreateTrueColor(500, 1000);
    if (im == NULL) {
        printf("Error creating GD image\n");
        return 1;
    }

    ret = TestFunc(im, &sdm);
    if (!ret) {
        pngout = fopen(TESTFILE_OUT, "wb");
        if (pngout == NULL) {
            printf("Error writing %s\n", TESTFILE_OUT);
            ret = 1;
        } else {
            gdImagePng(im, pngout);
            fclose(pngout);
        }
    }

    gdImageDestroy(im);

    return ret;
}
