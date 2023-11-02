/* rdsvgf.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    int strip_spaces;
    int pre_space;
    MsvgElement *root;
    MsvgElement *active_element;
    FILE *report;
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

    if (mudptr->process_finished) return;

    if (mudptr->report)
        fprintf(mudptr->report, "start %s %d %d %d\n", name, mudptr->depth,
                mudptr->skip_depth, mudptr->svg_depth);
    
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
        } else {
            eid = MsvgFindElementId(name);
            //printf("element %d %d\n",mudptr->active_element->eid, eid);
            if (!MsvgIsSupSonElement(mudptr->active_element->eid, eid)) {
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
                if (mudptr->report)
                    fprintf(mudptr->report, "new %s element added\n", name);
            }
        }
    }

    mudptr->depth += 1;
}

static void endElement(void *userData, const char *name)
{
    MyUserData *mudptr = userData;
    
    if (mudptr->process_finished) return;

    mudptr->depth -= 1;
    
    if (mudptr->report)
        fprintf(mudptr->report, "end %s %d %d %d\n", name, mudptr->depth,
                mudptr->skip_depth, mudptr->svg_depth);
    
    if (!mudptr->svg_found) return;

    if (mudptr->skip_depth) { 
        if (mudptr->skip_depth == mudptr->depth)
            mudptr->skip_depth = 0;
        return;
    }

    mudptr->strip_spaces = 1;
    mudptr->pre_space = 0;

    mudptr->active_element = mudptr->active_element->father;
    
    if (mudptr->depth == mudptr->svg_depth)
        mudptr->process_finished = 1;
}

static void data(void *userData, const char *s, int len)
{
    MyUserData *mudptr = userData;
    char *saux;
    int i, fch, rlen, presp;

    if (mudptr->process_finished) return;
    if (mudptr->skip_depth) return;
    if (!mudptr->active_element) return;
    if (!MsvgElementCanHaveContent(mudptr->active_element->eid)) return;
    
    //for (i=0; i<len; i++)
    //    printf("%2x ", s[i]);
    //printf("\n");

    fch = 0;
    if (mudptr->strip_spaces) {
        for (i=0; i<len; i++) {
            if (s[i] == '\n' || s[i] == '\t' || s[i] == ' ')
                fch = i + 1;
            else
                break;
        }
    }

    if (fch >= len) return;

    mudptr->strip_spaces = 0;

    rlen = len - fch;

    if (rlen == 1 && s[fch] == 0xa) {
        mudptr->strip_spaces = 1;
        mudptr->pre_space = 1;
        return;
    }

    saux = malloc((rlen+2)*sizeof(char)); // +2 because possible pre_space
    if (saux == NULL) return;

    presp = 0;
    if (mudptr->pre_space) {
        saux[0] = ' ';
        presp = 1;
        mudptr->pre_space = 0;
    }
    memcpy(&(saux[presp]), &(s[fch]), rlen);
    rlen += presp;
    saux[rlen] = '\0';

    if (mudptr->report)
        fprintf(mudptr->report, "data (%d) %s\n", len, saux);

    MsvgAddContent(mudptr->active_element, rlen, saux);

    free(saux);
}

static void comment(void *userData, const char *s)
{
    MyUserData *mudptr = userData;
    MsvgElement *ptr;
    int len;

    if (mudptr->process_finished) return;

    if (mudptr->report)
        fprintf(mudptr->report, "comment %s\n", s);

    if (mudptr->skip_depth) return;
    if (!mudptr->active_element) return;
    if (!MsvgIsSupSonElement(mudptr->active_element->eid, EID_V_COMMENT)) return;

    ptr = MsvgNewElement(EID_V_COMMENT, mudptr->active_element);
    if (ptr == NULL) {
        mudptr->mem_error = 1;
        mudptr->process_finished = 1;
        return;
    }

    if (mudptr->report)
        fprintf(mudptr->report, "new v_comment element added\n");

    len = strlen(s);
    MsvgAddContent(ptr, len, s);
}

MsvgElement *MsvgReadSvgFile2(const char *fname, int *error, FILE *report)
{
    #define BUFRSIZE 8192
    FILE *f;
    char buf[BUFRSIZE];
    int done;
    XML_Parser parser;
    MyUserData mud = {1, 0, 0, 0, 0, 0, 1, 0, NULL, NULL, NULL};

    mud.report = report;
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
    XML_SetCommentHandler(parser, comment);

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

MsvgElement *MsvgReadSvgFile(const char *fname, int *error)
{
    return MsvgReadSvgFile2(fname, error, NULL);
}
