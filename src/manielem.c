/* manielem.c
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
#include "msvg.h"

void MsvgPruneElement(MsvgElement *el)
{
  MsvgElement *father;
  
  father = el->father;
  if (father == NULL) return; // already pruned
    
  el->father = NULL;
  if (el->psibling == NULL) { // first sibling
    father->fson = el->nsibling;
    if (el->nsibling != NULL) {
      el->nsibling->psibling = NULL;
      el->nsibling = NULL;
    }
  } else {
    el->psibling->nsibling = el->nsibling;
    if (el->nsibling != NULL) {
      el->nsibling->psibling = el->psibling;
      el->nsibling->psibling = NULL;
      el->nsibling = NULL;
    }
    el->psibling = NULL;
  }
}

static void MsvgFreeElement(MsvgElement *el)
{
  switch (el->eid) {
  case EID_SVG :
    free(el->psvgattr);
    break;
  case EID_G :
    free(el->pgattr);
    break;
  case EID_RECT :
    free(el->prectattr);
    break;
  case EID_CIRCLE :
    free(el->pcircleattr);
    break;
  case EID_ELLIPSE :
    free(el->pellipseattr);
    break;
  case EID_LINE :
    free(el->plineattr);
    break;
  case EID_POLYLINE :
    free(el->ppolylineattr);
    break;
  case EID_POLYGON :
   free(el->ppolygonattr);
    break;
  default :
    break; // TODO: test error
  }

  free(el);
}

void MsvgDeleteElement(MsvgElement *el)
{
  MsvgElement *son;

  MsvgPruneElement(el);
  
  while (el->fson != NULL) {
    son = el->fson;
    MsvgDeleteElement(el);
  }
  
  MsvgFreeElement(el);
}

int MsvgInsertSonElement(MsvgElement *el, MsvgElement *father)
{
  MsvgElement *ptr;

  if (father == NULL) return 0;
  if (!MsvgIsSupSonElementId(father->eid, el->eid)) return 0;
  
  el->father = father;
  el->psibling = el->nsibling = NULL;
  if (father->fson == NULL) {
    father->fson = el;
  } else {
    ptr = father->fson;
    while (ptr->nsibling != NULL)
      ptr = ptr->nsibling;
    ptr->nsibling = el;
    el->psibling = ptr;
  }
  
  return 1;
}

int MsvgInsertPSiblingElement(MsvgElement *el, MsvgElement *sibling)
{
  if (sibling == NULL) return 0;
  
  el->father = sibling->father;
  
  if (sibling->psibling == NULL) { // first son
    if (el->father != NULL) el->father->fson = el;
    el->psibling = NULL;
    el->nsibling = sibling;
    sibling->psibling = el;
  } else {
    el->psibling = sibling->psibling;
    sibling->psibling->nsibling = el;
    el->nsibling = sibling;
    sibling->psibling = el;
  }

  return 1;
}

int MsvgInsertNSiblingElement(MsvgElement *el, MsvgElement *sibling)
{
  if (sibling == NULL) return 0;
  
  el->father = sibling->father;

  el->nsibling = sibling->nsibling;
  el->nsibling = el;
  el->psibling = sibling;
  if (el->nsibling != NULL)
    el->nsibling->psibling = el;
  
  return 1;
}
