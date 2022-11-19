/* rendgd.h ---- include file to render svg trees using GD library
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

#define SVGDRAWMODE_FIT      0  // fit to context
#define SVGDRAWMODE_PAR      1  // preserve aspect/ratio
#define SVGDRAWMODE_SCOORD   2  // same coordinates as svg file

#define SVGDRAWADJ_LEFT      0  // fit to left
#define SVGDRAWADJ_CENTER    1  // fit to center
#define SVGDRAWADJ_RIGHT     2  // fit to right

typedef struct {
    int width, height;
    int mode;
    int adj;
    double zoom;
    double xdespl;
    double ydespl;
    double rotang;
    int bg;
} GDSVGDrawMode;

int GDDrawSVGtree(MsvgElement *root, GDSVGDrawMode *sdm, gdImagePtr im);

