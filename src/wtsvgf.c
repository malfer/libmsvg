/* wtsvgf.c
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
#include <string.h>
#include "msvg.h"

static void writeLabelElement(FILE *f, enum EID eid, int start, int depth)
{
  int i;

  for (i=0; i<depth; i++)
    fputs("  ", f);

  if (start)
    fputs("<", f);
  else
    fputs("</", f);

  fputs(MsvgFindElementName(eid), f);

  if (!start)
    fputs(">\n", f);
}

static void writeElement(FILE *f, MsvgElement *el, int depth)
{
  MsvgAttribute *pattr;

  writeLabelElement(f, el->eid, 1, depth);
  if (el->fattr != NULL) {
     pattr = el->fattr;
     while (pattr != NULL) {
       fprintf(f, " %s=\"%s\"", pattr->key, pattr->value);
       pattr = pattr->nattr;
     }
  }

  if (el->fson != NULL) {
    fputs(">\n", f);
    writeElement(f, el->fson, depth+1);
    writeLabelElement(f, el->eid, 0, depth);
  } else {
    fputs(" />\n", f);
  }

  if (el->nsibling != NULL)
    writeElement(f, el->nsibling, depth);
}

int MsvgWriteSvgFile(MsvgElement *root, const char *fname)
{
  FILE *f;

  if (root == NULL) return 0;
  if (root->eid != EID_SVG) return 0;

  f = fopen(fname, "wt");
  if (f == NULL) return 0;

  fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", f);
  writeElement(f, root, 0);

  fclose(f);
  return 1;
}
