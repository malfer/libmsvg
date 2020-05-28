/* scanpath.c
 * 
 * libmsvg, a minimal library to read and write svg files
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
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
#include <string.h>
#include <ctype.h>
#include "msvg.h"

#define SCANBUFLEN 100

static char * scanNumber(char *s, char *buf)
{
    int bufpos = 0;
    int dpfound = 0;
    int expfound = 0;

    if (*s == '-' || *s == '+') {
        buf[0] = *s;
        s++;
        bufpos++;
    }

    while (*s) {
        if (isdigit(*s) || (!dpfound && *s == '.')) {
            buf[bufpos++] = *s;
            s++;
        } else if (!dpfound && *s == '.') {
            buf[bufpos++] = *s;
            s++;
            dpfound = 1;
        } else if (!expfound && (*s == 'e' || *s == 'E')) {
            buf[bufpos++] = *s;
            s++;
            dpfound = 1;
            expfound = 1;
            if (*s == '-' || *s == '+') {
                buf[bufpos++] = *s;
                s++;
            }
        } else {
            break;
        }
        if (bufpos >= SCANBUFLEN - 2) break; // or bufpos--; mmmm.....
    }

    buf[bufpos] = '\0';
    return s;
}

static char * scanItem(char *d, char *buf, int *isnumber)
{
    buf[0] = '\0';
    *isnumber = 0;

    while(*d && (isspace(*d) || *d == ',')) d++;
    if (!*d) return d;

    if (isdigit(*d) || strchr("-+.", *d)) {
        *isnumber = 1;
        return scanNumber(d, buf);
    } else {
        buf[0] = *d;
        buf[1] = '\0';
        d++;
    }

    return d;
}

static char * scanSubPath(char *d, double xorg, double yorg, MsvgSubPath **psp)
{
    MsvgSubPath *sp = NULL;
    char buf[SCANBUFLEN];
    double lcpx = 0;
    double lcpy = 0;
    char actcmd;
    int expected_pars = 2;
    double par[10]; // must be >= maximum expected_pars (or CRASH)
    int npar = 0;
    int isnumber;
    int i;

    *psp = NULL;
    // we need a M or m and 2 numbers to start
    while (*d) {
        d = scanItem(d, buf, &isnumber);
        if (!*d) return d;
        if (buf[0] == 'M' || buf[0] == 'm') {
            actcmd = buf[0];
            d = scanItem(d, buf, &isnumber);
            if (!*d || !isnumber) return d;
            par[0] = atof(buf);
            d = scanItem(d, buf, &isnumber);
            if (!*d || !isnumber) return d;
            par[1] = atof(buf);
            if (actcmd == 'M') {
                xorg = par[0];
                yorg = par[1];
                actcmd = 'L';
            } else {
                xorg += par[0];
                yorg += par[1];
                actcmd = 'l';
            }
            lcpx = xorg;
            lcpy = yorg;
            sp = MsvgNewSubPath(32);
            if (sp == NULL) return d;
            MsvgAddPointToSubPath(sp, 'M', xorg, yorg);
            expected_pars = 2;
            npar = 0;
            break;
        }
    }

    if (!*d) return d;

    // now begin process
    while (*d) {
        d = scanItem(d, buf, &isnumber);
        if (isnumber) {
            par[npar++] = atof(buf);
            if (npar >= 10) npar = 9; // to protect
            if (npar >= expected_pars) {
                switch (actcmd) {
                    case 'L' :
                        xorg = yorg = 0;
                    case 'l' :
                        xorg += par[0];
                        yorg += par[1];
                        MsvgAddPointToSubPath(sp, 'L', xorg, yorg);
                        lcpx = xorg;
                        lcpy = yorg;
                        break;
                    case 'H' :
                        xorg = 0;
                    case 'h' :
                        xorg += par[0];
                        MsvgAddPointToSubPath(sp, 'L', xorg, yorg);
                        lcpx = xorg;
                        lcpy = yorg;
                        break;
                    case 'V' :
                        yorg = 0;
                    case 'v' :
                        yorg += par[0];
                        MsvgAddPointToSubPath(sp, 'L', xorg, yorg);
                        lcpx = xorg;
                        lcpy = yorg;
                        break;
                    case 'c' :
                        for (i=0; i<3; i++) {
                            par[i*2] += xorg;
                            par[i*2+1] += yorg;
                        }
                    case 'C' :
                        MsvgAddPointToSubPath(sp, 'C', par[0], par[1]);
                        MsvgAddPointToSubPath(sp, ' ', par[2], par[3]);
                        MsvgAddPointToSubPath(sp, ' ', par[4], par[5]);
                        lcpx = par[2];
                        lcpy = par[3];
                        xorg = par[4];
                        yorg = par[5];
                        break;
                    case 's' :
                        for (i=0; i<2; i++) {
                            par[i*2] += xorg;
                            par[i*2+1] += yorg;
                        }
                    case 'S' :
                        lcpx = 2*xorg - lcpx;
                        lcpy = 2*yorg - lcpy;
                        MsvgAddPointToSubPath(sp, 'C', lcpx, lcpy);
                        MsvgAddPointToSubPath(sp, ' ', par[0], par[1]);
                        MsvgAddPointToSubPath(sp, ' ', par[2], par[3]);
                        lcpx = par[0];
                        lcpy = par[1];
                        xorg = par[2];
                        yorg = par[3];
                        break;
                    case 'q' :
                        for (i=0; i<2; i++) {
                            par[i*2] += xorg;
                            par[i*2+1] += yorg;
                        }
                    case 'Q' :
                        MsvgAddPointToSubPath(sp, 'Q', par[0], par[1]);
                        MsvgAddPointToSubPath(sp, ' ', par[2], par[3]);
                        lcpx = par[0];
                        lcpy = par[1];
                        xorg = par[2];
                        yorg = par[3];
                        break;
                    case 't' :
                        par[0] += xorg;
                        par[1] += yorg;
                    case 'T' :
                        lcpx = 2*xorg - lcpx;
                        lcpy = 2*yorg - lcpy;
                        MsvgAddPointToSubPath(sp, 'Q', lcpx, lcpy);
                        MsvgAddPointToSubPath(sp, ' ', par[0], par[1]);
                        xorg = par[0];
                        yorg = par[1];
                        break;
                    // 'A' and 'a' are not specified in SVG Tiny 1.2
                    // aproximate it by a line is better that nothing
                    case 'A' :
                        xorg = yorg = 0;
                    case 'a' :
                        xorg += par[5];
                        yorg += par[6];
                        MsvgAddPointToSubPath(sp, 'L', xorg, yorg);
                        lcpx = xorg;
                        lcpy = yorg;
                        break;
                }
                npar = 0;
            }
        } else {
            actcmd = buf[0];
            if (strchr("VvHh", actcmd)) expected_pars = 1;
            else if (strchr("LlTt", actcmd)) expected_pars = 2;
            else if (strchr("SsQq", actcmd)) expected_pars = 4;
            else if (strchr("Cc", actcmd)) expected_pars = 6;
            else if (strchr("Aa", actcmd)) expected_pars = 6;
            else if (strchr("Mm", actcmd)) {
                // we have finished an open subpath
                *psp = sp;
                d--; // because it is the beginning of next subpath
                return d;
            } else if (strchr("Zz", actcmd)) {
                // we have finished a closed subpath
                //MsvgAddPointToSubPath(sp, 'Z', sp->spp[0].x, sp->spp[0].y);
                sp->closed = 1;
                *psp = sp;
                return d;
            } else expected_pars = 2; // really an error
        }
    }
    // no more chars, so we have finished an open subpath too
    *psp = sp;
    return d;
}

MsvgSubPath * MsvgScanPath(char *d)
{
    MsvgSubPath *firstsp, **psp;
    int xorg, yorg, nextpos;

    xorg = 0;
    yorg = 0;
    psp = &firstsp;
    while (1) {
        d = scanSubPath(d, xorg, yorg, psp);
        if (*psp == NULL || !*d) break;
        nextpos = (*psp)->closed ? 0 : (*psp)->npoints-1;
        xorg = (*psp)->spp[nextpos].x;
        yorg = (*psp)->spp[nextpos].y;
        psp = &((*psp)->next);
    }

    return firstsp;
}

MsvgSubPath * MsvgNewSubPath(int maxpoints)
{
    MsvgSubPath *sp;

    if (maxpoints <= 0) maxpoints = 32;

    sp = malloc(sizeof(MsvgSubPath));
    if (sp == NULL) return NULL;
    sp->spp = malloc(sizeof(MsvgSubPathPoint)*maxpoints);
    if (sp->spp == NULL) {
        free(sp);
        return NULL;
    }

    sp->maxpoints = maxpoints;
    sp->npoints = 0;
    sp->closed = 0;
    sp->failed_realloc = 0;
    sp->next = NULL;

    return sp;
}

void MsvgExpandSubPath(MsvgSubPath *sp)
{
    MsvgSubPathPoint *newspp;
    int newmaxpoints;

    newmaxpoints = sp->maxpoints * 2;
    newspp = realloc(sp->spp, sizeof(MsvgSubPathPoint)*newmaxpoints);
    if (newspp == NULL) {
        sp->failed_realloc = 1;
        return;
    }

    sp->maxpoints = newmaxpoints;
    sp->spp = newspp;
}

void MsvgAddPointToSubPath(MsvgSubPath *sp, char cmd, double x, double y)
{
    if (sp->npoints >= sp->maxpoints) {
        if (sp->failed_realloc) return;
        MsvgExpandSubPath(sp);
        MsvgAddPointToSubPath(sp, cmd, x, y);
    }

    sp->spp[sp->npoints].cmd = cmd;
    sp->spp[sp->npoints].x = x;
    sp->spp[sp->npoints].y = y;
    sp->npoints++;
}

MsvgSubPath * MsvgDupSubPath(MsvgSubPath *srcsp)
{
    MsvgSubPath *dessp;
    int i;

    dessp = MsvgNewSubPath(srcsp->npoints);
    if (dessp == NULL) return NULL;
    dessp->npoints = srcsp->npoints;
    dessp->closed = srcsp->closed;
    for (i=0; i<srcsp->npoints; i++) {
        dessp->spp[i] = srcsp->spp[i];
    }
    if (srcsp->next) {
        dessp->next = MsvgDupSubPath(srcsp->next);
    }

    return dessp;
}

int MsvgCountSubPaths(MsvgSubPath *sp)
{
    int count = 0;

    while (sp) {
        count++;
        sp = sp->next;
    }

    return count;
}

void MsvgDestroySubPath(MsvgSubPath *sp)
{
    if (sp == NULL) return;
    MsvgDestroySubPath(sp->next);
    free(sp->spp);
    free(sp);
}
