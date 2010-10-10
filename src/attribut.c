/* attribut.c
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
#include "msvg.h"

int MsvgAddAttribute(MsvgElement *pelement, const char *key, const char *value)
{
  MsvgAttribute **dptr;
  MsvgAttribute *pattr;


  dptr = &(pelement->fattr);
  while (*dptr)
    dptr = &((*dptr)->nattr);

  pattr = calloc(1, sizeof(MsvgAttribute));
  if (pattr == NULL) return 0;

  pattr->key = strdup(key);
  if (pattr->key == NULL) {
    free(pattr);
    return 0;
  }

  pattr->value = strdup(value);
  if (pattr->value == NULL) {
    free(pattr->key);
    free(pattr);
    return 0;
  }

  *dptr = pattr;
  return 1;
}
