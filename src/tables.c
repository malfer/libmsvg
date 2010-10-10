/* tables.c
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

#include <string.h>
#include "msvg.h"

typedef struct {
  enum EID eid; // element id
  char *ename; // element name
  int nsset; // number of supported son element types
  enum EID sset[EID_LAST]; // list of supported son element types
} MsvgIdElement;

static MsvgIdElement supported_elements[] = {
  {EID_SVG, "svg", 7, {EID_G, EID_RECT, EID_CIRCLE, EID_ELLIPSE, EID_LINE,
                       EID_POLYLINE, EID_POLYGON} },
  {EID_G, "g", 7, {EID_G, EID_RECT, EID_CIRCLE, EID_ELLIPSE, EID_LINE,
                   EID_POLYLINE, EID_POLYGON} },
  {EID_RECT, "rect", 0},
  {EID_CIRCLE, "circle", 0},
  {EID_ELLIPSE, "ellipse", 0},
  {EID_LINE, "line", 0},
  {EID_POLYLINE, "polyline", 0},
  {EID_POLYGON, "polygon", 0}
};

enum EID MsvgFindElementId(const char *ename)
{
  int i;

  for (i=0; i<EID_LAST; i++) {
    if (strcmp(ename, supported_elements[i].ename) == 0)
      return supported_elements[i].eid;
  }
  return EID_NOTSUPPORTED;
}

char * MsvgFindElementName(enum EID eid)
{
  int i;

  for (i=0; i<EID_LAST; i++) {
    if (eid == supported_elements[i].eid)
      return supported_elements[i].ename;
  }
  return NULL;
}

int MsvgIsSupSonElementId(enum EID fatherid, enum EID sonid)
{
  int i, j;

  for (i=0; i<EID_LAST; i++) {
    if (fatherid == supported_elements[i].eid) {
      for (j=0; j<supported_elements[i].nsset; j++) {
	if (supported_elements[i].sset[j] == sonid) return 1;
      }
      return 0;
    }
  }
  return 0;
}
