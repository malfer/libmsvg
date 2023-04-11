/* rendmgrx.h ---- include file to render svg trees using MGRX library
 *
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * This source is free software; you can redistribute it and/or
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

#define SVGDRAWMODE_FIT      0  // fit to context
#define SVGDRAWMODE_PAR      1  // fit preserving aspect/ratio
#define SVGDRAWMODE_SCOORD   2  // same coordinates as svg file

#define SVGDRAWADJ_LEFT      0  // fit to left
#define SVGDRAWADJ_CENTER    1  // fit to center
#define SVGDRAWADJ_RIGHT     2  // fit to right

typedef struct {
    int mode;       // one of SVGDRAWMODE constants
    int adj;        // one of SVGDRAWADJ constants
    double zoom;    // zoom to apply before drawing
    double xdespl;  // x displacement to apply before drawing
    double ydespl;  // y displacement to apply before drawing
    double rotang;  // angle in degrees to rotate before drawing
    GrColor bg;     // background color to use if not defined by root
} GrSVGDrawMode;

// Draw a cooked SVG-tree in the current context
// return 0 if no error, else:
//  -1 root is NULL
//  -2 root is not a EID_SVG element
//  -3 root is not a cooked tree
//  -4 possible integer overflow (it happens if zoom is a big value)
//  -5 invalid value in sdm->mode or sdm->adj
//  -6 failed the serialization process
int GrDrawSVGtree(MsvgElement *root, GrSVGDrawMode *sdm);

// Draw a cooked SVG-tree in the current context using double buffer
int GrDrawSVGtreeUsingDB(MsvgElement *root, GrSVGDrawMode *sdm);

