/* gsvg.c ---- test generated svg for MGRX
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
#include <mgrxkeys.h>
#include <msvg.h>
#include "rendmgrx.h"

static MsvgElement *CreateTree(void)
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
  son->pctx.fill = 0XFF;
  son->pctx.stroke = 0XFF0000;

  son = MsvgNewElement(EID_CIRCLE, root);
  son->pcircleattr->cx = 100;
  son->pcircleattr->cy = 100;
  son->pcircleattr->r = 80;
  son->pctx.fill = 0XFF00;
  son->pctx.stroke = 0X8800;
  
  return root;
}

static void TestFunc(void)
{
  GrEvent ev;

  GrClearContext(GrBlack());
  DrawSVGtree(CreateTree(), 0);
  GrEventWaitKeyOrClick(&ev);
}

int main(int argc,char **argv)
{
  int yhelptext;

  GrContext *ctx;
  GrSetMode(GR_default_graphics);
  GrClearScreen(GrWhite());

  yhelptext = GrScreenY() - 60;
  GrTextXY(10, yhelptext+25,
           "Test a generated MsvgElement tree",
           GrBlack(), GrNOCOLOR);
  GrTextXY(10, yhelptext+40,
           "Press any key to finish",
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
