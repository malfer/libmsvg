/* rsvg.c ---- test read svg for MGRX
 *
 * This test program is a dirty hack to test the libmsvg librarie with the
 * mgrx graphics library. It is NOT part of the librarie really.
 *
 * In the future this will be a interface within mgrx and libmsvg
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mgrx.h>
#include <mgrxkeys.h>
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

static void DrawSVG(MsvgElement *root)
{
  MsvgElement *p = root;
  
  while(1) {
    if (p == NULL) return;
    DrawElement(p);
    if (p->fson != NULL)
      DrawSVG(p->fson);
    p = p->nsibling;
  }
}

MsvgElement *CreateTree(void)
{
  MsvgElement *root;
  root = MsvgNewElement(EID_SVG, NULL);
  root->psvgattr->vb_min_x = 0;
  root->psvgattr->vb_min_y = 0;
  root->psvgattr->vb_width = 640;
  root->psvgattr->vb_height = 480;
  root->psvgattr->tree_type = COOKED_SVGTREE;

  MsvgElement *son;
  son = MsvgNewElement(EID_RECT, root);
  son->prectattr->x = 50;
  son->prectattr->y = 50;
  son->prectattr->width = 300;
  son->prectattr->height = 300;
  son->prectattr->fill_color = 0XFF;
  son->prectattr->stroke_color = 0XFF0000;

  son = MsvgNewElement(EID_CIRCLE, root);
  son->pcircleattr->cx = 100;
  son->pcircleattr->cy = 100;
  son->pcircleattr->r = 80;
  son->pcircleattr->fill_color = 0XFF00;
  son->pcircleattr->stroke_color = 0X8800;
  
  return root;
}

static void TestFunc(void)
{
  GrEvent ev;
  MsvgElement *root;

  GrClearContext(GrBlack());
  root = MsvgReadSvgFile("test.svg");
  if (root == NULL) return;
  if (!MsvgRaw2CookedTree(root)) return;
  DrawSVG(root);
  GrEventWaitKeyOrClick(&ev);
  DrawSVG(CreateTree());
  GrEventWaitKeyOrClick(&ev);
}

int main(int argc,char **argv)
{
  int yhelptext;

  GrContext *ctx;
  GrSetMode(GR_default_graphics);
  GrClearScreen(GrWhite());

  yhelptext = GrScreenY() - 60;
  GrTextXY(10, yhelptext+10,
           "The test.svg file",
           GrBlack(), GrNOCOLOR);
  GrTextXY(10, yhelptext+25,
           "Press any key to show a generated tree",
           GrBlack(), GrNOCOLOR);
  GrTextXY(10, yhelptext+40,
           "And press any key again to finish",
           GrBlack(), GrNOCOLOR);

  ctx = GrCreateSubContext(10, 10, GrScreenX()-10, yhelptext, NULL, NULL);
  GrSetContext(ctx);
  GrEventInit();
  GrMouseDisplayCursor();
  TestFunc();
  GrEventUnInit();
  GrSetMode(GR_default_text);
  return(0);
}
