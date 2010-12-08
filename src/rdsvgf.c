/* rdsvgf.c
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
#include "xmlparse.h"
#include "msvg.h"

typedef struct {
  int depth;
  int svg_depth;
  int skip_depth;
  int svg_found;
  int process_finished;
  MsvgElement *root;
  MsvgElement *active_element;
} MyUserData;

static void addAttributes(MsvgElement *ptr, const char **attr)
{
  int i;

  for (i = 0; attr[i]; i += 2) {
    MsvgAddAttribute(ptr, attr[i], attr[i + 1]);
  }
}

static void startElement(void *userData, const char *name, const char **attr)
{
  MyUserData *mudptr = userData;
  enum EID eid;
  MsvgElement *ptr;

//  printf("entra %s %d %d %d\n",name,mudptr->depth,mudptr->skip_depth,mudptr->svg_depth);
  if (mudptr->process_finished) return;

  if (!mudptr->skip_depth) {
    if (!mudptr->svg_found) {
      if (strcmp(name, "svg") == 0) {
        mudptr->svg_found = 1;
        mudptr->root = MsvgNewElement(EID_SVG, NULL);
        // TODO: test error
	mudptr->root->psvgattr->tree_type = RAW_SVGTREE;
        addAttributes(mudptr->root, attr);
        mudptr->active_element = mudptr->root;
        mudptr->svg_depth = mudptr->depth;
      }
    }  else {
      eid = MsvgFindElementId(name);
//      printf("element %d %d\n",mudptr->active_element->eid, eid);
      if (!MsvgIsSupSonElementId(mudptr->active_element->eid, eid)) {
        mudptr->skip_depth = mudptr->depth;
      } else {
        ptr = MsvgNewElement(eid, mudptr->active_element);
        addAttributes(ptr, attr);
        // TODO: test error
        mudptr->active_element = ptr;
//        printf("new element %s\n",name);
      }
    }
  }

  mudptr->depth += 1;
}

static void endElement(void *userData, const char *name)
{
  MyUserData *mudptr = userData;

//  printf("sale %s %d %d %d\n",name,mudptr->depth,mudptr->skip_depth,mudptr->svg_depth);
  if (mudptr->process_finished) return;

  mudptr->depth -= 1;

  if (mudptr->skip_depth) { 
    if (mudptr->skip_depth == mudptr->depth)
      mudptr->skip_depth = 0;
    return;
  }

  mudptr->active_element = mudptr->active_element->father;

  if (mudptr->svg_found && (mudptr->depth == mudptr->svg_depth))
    mudptr->process_finished = 1;
}

MsvgElement *MsvgReadSvgFile(const char *fname)
{
#define BUFRSIZE 8192
  FILE *f;
  char buf[BUFRSIZE];
  int done;
  XML_Parser parser;
  MyUserData mud = {1, 0, 0, 0, 0, NULL, NULL};

  f = fopen(fname, "rt");
  if (f == NULL) return NULL;

  parser = XML_ParserCreate(NULL);
  if (parser == NULL) {
    fclose(f);
    return NULL;
  }

  XML_SetUserData(parser, &mud);
  XML_SetElementHandler(parser, startElement, endElement);

  do {
    size_t len = fread(buf, 1, sizeof(buf), f);
    done = len < sizeof(buf);
    if (!XML_Parse(parser, buf, len, done)) {
      // TODO: free root
      XML_ParserFree(parser);
      fclose(f);
      return NULL;
    }
  } while (!done);

  XML_ParserFree(parser);
  fclose(f);
  return mud.root;
}
