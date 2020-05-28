/* rendmgrx.h ---- include file to render svg trees using MGRX library
 *
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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

#define SVGDRAWMODE_FIT      0x00  // fit to context
#define SVGDRAWMODE_PAR      0x01  // preserve aspect/ratio
#define SVGDRAWMODE_SCOORD   0x02  // same coordinates as context
#define SVGDRAWMODE_MASK     0x0F  // mode mask

#define SVGDRAWADJ_LEFT      0x00  // fit to left
#define SVGDRAWADJ_CENTER    0x10  // fit to center
#define SVGDRAWADJ_RIGHT     0x20  // fit to right
#define SVGDRAWADJ_MASK      0xF0  // adj mask

int DrawSVGtree(MsvgElement *root, int smode, double zoom, GrColor bg);
int DrawSVGtreeUsingDB(MsvgElement *root, int smode, double zoom, GrColor bg);

