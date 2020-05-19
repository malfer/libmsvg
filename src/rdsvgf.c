/* rdsvgf.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
    int mem_error;
    MsvgElement *root;
    MsvgElement *active_element;
} MyUserData;

static void addAttributes(MsvgElement *ptr, const char **attr)
{
    int i;
    
    for (i = 0; attr[i]; i += 2) {
        MsvgAddRawAttribute(ptr, attr[i], attr[i + 1]);
    }
}

static void startElement(void *userData, const char *name, const char **attr)
{
    MyUserData *mudptr = userData;
    enum EID eid;
    MsvgElement *ptr;
    
    //printf("entra %s %d %d %d\n",name,mudptr->depth,mudptr->skip_depth,mudptr->svg_depth);
    if (mudptr->process_finished) return;
    
    if (!mudptr->skip_depth) {
        if (!mudptr->svg_found) {
            if (strcmp(name, "svg") == 0) {
                mudptr->root = MsvgNewElement(EID_SVG, NULL);
                if (mudptr->root == NULL) {
                    mudptr->mem_error = 1;
                    mudptr->process_finished = 1;
                    return;
                }
                mudptr->svg_found = 1;
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
                if (ptr == NULL) {
                    mudptr->mem_error = 1;
                    mudptr->process_finished = 1;
                    return;
                }
                addAttributes(ptr, attr);
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
    
    //printf("sale %s %d %d %d\n",name,mudptr->depth,mudptr->skip_depth,mudptr->svg_depth);
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

static void data(void *userData, const char *s, int len)
{
    MyUserData *mudptr = userData;
    char *saux;
    int i, fch, rlen;

    if (!MsvgElementCanHaveContent(mudptr->active_element->eid)) return;

    fch = 0;
    for (i=0; i<len; i++) {
        if (s[i] == '\n' || s[i] == '\t' || s[i] == ' ')
            fch = i + 1;
        else
            break;
    }

    if (fch >= len) return;

    rlen = len - fch;
    saux = malloc((rlen+1)*sizeof(char));
    if (saux == NULL) return;
    
    memcpy(saux, &(s[fch]), rlen);
    saux[rlen] = '\0';

    //printf("  (%d) %s\n", len, saux);

    MsvgAddContent(mudptr->active_element, len, saux);

    free(saux);
}

MsvgElement *MsvgReadSvgFile(const char *fname, int *error)
{
    #define BUFRSIZE 8192
    FILE *f;
    char buf[BUFRSIZE];
    int done;
    XML_Parser parser;
    MyUserData mud = {1, 0, 0, 0, 0, 0, NULL, NULL};

    *error = 0;
    // -1 error opening file
    // -2 memory error creating parser
    // -3 memory error building the tree
    // >0 expat error
    
    f = fopen(fname, "rt");
    if (f == NULL) {
        *error = -1;
        return NULL;
    }
    
    parser = XML_ParserCreate(NULL);
    if (parser == NULL) {
        fclose(f);
        *error = -2;
        return NULL;
    }
    
    XML_SetUserData(parser, &mud);
    XML_SetElementHandler(parser, startElement, endElement);
    XML_SetCharacterDataHandler(parser, data);
    
    do {
        size_t len = fread(buf, 1, sizeof(buf), f);
        done = len < sizeof(buf);
        if (!XML_Parse(parser, buf, len, done)) {
            *error = XML_GetErrorCode(parser);
            XML_ParserFree(parser);
            if (mud.root) MsvgDeleteElement(mud.root);
            fclose(f);
            return NULL;
        }
    } while (!done);
    
    XML_ParserFree(parser);
    fclose(f);

    if (mud.mem_error) {
        if (mud.root) MsvgDeleteElement(mud.root);
        *error = -3;
        return NULL;
    }

    return mud.root;
}
