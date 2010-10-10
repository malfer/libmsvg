/* printree.c
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
#include "msvg.h"

static void printAttribute(FILE *f, MsvgAttribute *ptr)
{
  if (ptr == NULL) return;

  fprintf(f, " (%s = %s)", ptr->key, ptr->value);

  printAttribute(f, ptr->nattr);
}

void MsvgPrintElementTree(FILE *f, MsvgElement *ptr, int depth)
{
  int i;

  if (ptr == NULL) return;

  if (depth > 0) {
    for (i=0; i<depth; i++)
      fputs("  |", f);
    fputs("-->", f);
  }

  fprintf(f, "%s", MsvgFindElementName(ptr->eid));
  printAttribute(f, ptr->fattr);
  fputs("\n", f);

  MsvgPrintElementTree(f, ptr->fson, depth+1);

  MsvgPrintElementTree(f, ptr->nsibling, depth);
}
