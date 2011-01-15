/* rendmgrx.c ---- functions to render svg trees using MGRX library
 *
 * These test programs are a dirty hack to test the libmsvg librarie with the
 * mgrx graphics library. It is NOT part of the librarie really.
 *
 * In the future this will be added to MGRX
 *
 * libmsvg, a minimal library to read and write svg files
 * Copyright (C) 2010 Mariano Alvarez Fernandez (malfer at telefonica.net)
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

#include <mgrx.h>
#include <msvg.h>

static void DrawElement(MsvgElement *el)
{
  GrColor cfill;
  GrColor cstroke;
  
  if (el->eid == EID_SVG) {
//    printf("svg %f %f %f %f\n",el->psvgattr->vb_min_x, el->psvgattr->vb_min_y,
//           el->psvgattr->vb_width, el->psvgattr->vb_height );
    GrSetUserWindow(el->psvgattr->vb_min_x,
                    el->psvgattr->vb_min_y,
		    el->psvgattr->vb_min_x+el->psvgattr->vb_width-1,
		    el->psvgattr->vb_min_y+el->psvgattr->vb_height-1);
  } else if (el->eid == EID_RECT) {
//    printf("rect %d %d\n", el->prectattr->fill_color, el->prectattr->stroke_color);
    if (el->prectattr->fill_color != NO_COLOR) {
      cfill = GrAllocColor2(el->prectattr->fill_color);
      GrUsrFilledBox(el->prectattr->x,
		     el->prectattr->y,
		     el->prectattr->x+el->prectattr->width-1,
		     el->prectattr->y+el->prectattr->height-1,
		     cfill);
    }
    if (el->prectattr->stroke_color != NO_COLOR) {
      cstroke = GrAllocColor2(el->prectattr->stroke_color);
      GrUsrBox(el->prectattr->x,
	       el->prectattr->y,
	       el->prectattr->x+el->prectattr->width-1,
	       el->prectattr->y+el->prectattr->height-1,
	       cstroke);
    } 
  } else if (el->eid == EID_CIRCLE) {
    if (el->pcircleattr->fill_color != NO_COLOR) {
      cfill = GrAllocColor2(el->pcircleattr->fill_color);
      GrUsrFilledCircle(el->pcircleattr->cx,
		        el->pcircleattr->cy,
			el->pcircleattr->r,
		        cfill);
    }
    if (el->pcircleattr->stroke_color != NO_COLOR) {
      cstroke = GrAllocColor2(el->pcircleattr->stroke_color);
      GrUsrCircle(el->pcircleattr->cx,
	          el->pcircleattr->cy,
		  el->pcircleattr->r,
	          cstroke);
    } 
  }
}

void DrawSVGtree(MsvgElement *root)
{
  MsvgElement *p = root;
  
  while(1) {
    if (p == NULL) return;
    DrawElement(p);
    if (p->fson != NULL)
      DrawSVGtree(p->fson);
    p = p->nsibling;
  }
}
