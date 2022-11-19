/* gsvg.c ---- test generated svg for GD
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

#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <gd.h>
#include <msvg.h>
#include "rendgd.h"

#define TESTFILE_OUT "test2.png"

static MsvgElement *CreateTree(void)
{
    MsvgElement *root, *son, *gson;

    root = MsvgNewElement(EID_SVG, NULL);
    root->psvgattr->vb_min_x = 0;
    root->psvgattr->vb_min_y = 0;
    root->psvgattr->vb_width = 640;
    root->psvgattr->vb_height = 480;
    root->psvgattr->tree_type = COOKED_SVGTREE;

    son = MsvgNewElement(EID_DEFS, root);

    gson = MsvgNewElement(EID_RECT, son);
    gson->prectattr->width = 100;
    gson->prectattr->height = 50;
    gson->prectattr->rx = 0;
    gson->prectattr->ry = 0;
    gson->id = strdup("MyRect");

    son = MsvgNewElement(EID_RECT, root);
    son->prectattr->x = 50;
    son->prectattr->y = 50;
    son->prectattr->width = 300;
    son->prectattr->height = 300;
    son->prectattr->rx = 10;
    son->prectattr->ry = 10;
    son->pctx->fill = 0x0000FF;
    son->pctx->stroke = 0xFF0000;

    son = MsvgNewElement(EID_CIRCLE, root);
    son->pcircleattr->cx = 100;
    son->pcircleattr->cy = 100;
    son->pcircleattr->r = 80;
    son->pctx->fill = 0x888888;
    son->pctx->stroke = 0x00FF00;
    son->pctx->stroke_width = 5;

    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 400;
    son->puseattr->y = 200;
    son->puseattr->refel = strdup("MyRect");
    son->pctx->fill = 0xFFFF00;
    
    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 450;
    son->puseattr->y = 300;
    son->puseattr->refel = strdup("MyRect");
    son->pctx->fill = 0x008888;
    son->pctx->stroke = 0x00FFFF;
    TMSetRotation(&(son->pctx->tmatrix), 30, 500, 325);
    
    son = MsvgNewElement(EID_USE, root);
    son->puseattr->x = 500;
    son->puseattr->y = 400;
    son->puseattr->refel = strdup("MyRect");
    son->pctx->fill = 0x888800;
    son->pctx->stroke = 0xFFFFFF;
    
    return root;
}

static int TestFunc(gdImagePtr im, GDSVGDrawMode *sdm)
{
    MsvgElement *root;

    root = CreateTree();
    GDDrawSVGtree(root, sdm, im);
    //MsvgCooked2RawTree(root);
    //MsvgWriteSvgFile(root, "tgsvg.svg");
    return 0;
}

int main(int argc,char **argv)
{
    gdImagePtr im;
    GDSVGDrawMode sdm = {640, 480, SVGDRAWMODE_PAR, SVGDRAWADJ_CENTER, 1.0, 0, 0, 0, 0};
    FILE *pngout;
    int ret;

    im = gdImageCreateTrueColor(sdm.width, sdm.height);
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
