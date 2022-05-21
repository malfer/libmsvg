/* showsvg.c ---- show a svg file, MGRX test
 *
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This is a test file of the libmsvg+MGRX libraries.
 * libmsvg+MGRX test files are in the Public Domain, this apply only to test
 * files, the libmsvg library itself is under the terms of the Expat license
 * and the MGRX library under the LGPL license
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <mgrx.h>
#include <mgrxkeys.h>
#include <msvg.h>
#include "rendmgrx.h"

/* default mode */

static int gwidth = 1024;
static int gheight = 728;
static int gbpp = 24;

static int DrawSvgFile(char *fname, int rotang, GrSVGDrawMode *sdm)
{
    MsvgElement *root;
    //char s[81];
    int error = 0;
    double cx, cy;
    TMatrix trot, taux;
    double gminx, gmaxx, gminy, gmaxy;
    static int print_dims = 1;
    
    root = MsvgReadSvgFile(fname, &error);
    if (root == NULL) return error;

    /*if (rotang != 0) {
        sprintf(s, "rotate(%d %d %d)", rotang, 250, 500);
        MsvgAddRawAttribute(root, "transform", s);
    }*/

    if (!MsvgRaw2CookedTree(root)) return -5;

    if (print_dims) {
        printf("Declared dimensions minx:%g  miny:%g  width:%g  height:%g\n",
               root->psvgattr->vb_min_x, root->psvgattr->vb_min_y,
               root->psvgattr->vb_width, root->psvgattr->vb_height);
        if (MsvgGetCookedDims(root, &gminx, &gmaxx, &gminy, &gmaxy)) {
            printf("Calculated dimensions minx:%g  miny:%g  width:%g  height:%g\n",
                    gminx, gminy, gmaxx-gminx, gmaxy-gminy);
        }
        print_dims = 0;
            
    }

    if (rotang != 0) {
        cx = root->psvgattr->vb_width / 2 + root->psvgattr->vb_min_x;
        cy = root->psvgattr->vb_height / 2 + root->psvgattr->vb_min_y;
        TMSetRotation(&trot, rotang, cx, cy);
        taux = root->ppctx->tmatrix;
        TMMpy(&(root->ppctx->tmatrix), &taux, &trot);
    }

    //GrDrawSVGtree(root, sdm);
    GrDrawSVGtreeUsingDB(root, sdm);
    MsvgDeleteElement(root);

    return 0;
}

int main(int argc,char **argv)
{
    GrSVGDrawMode sdm = {SVGDRAWMODE_PAR, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0};
    int rotang = 0;
    char *fname;

    if (argc <2) {
        printf("Usage: showsvg file.svg [width height bpp]\n");
        return 0;
    }

    fname = argv[1];

    if (argc >= 5) {
        gwidth = atoi(argv[2]);
        gheight = atoi(argv[3]);
        gbpp = atoi(argv[4]);
    }

    // set default driver and ask for user window resize if it is supported
    GrSetDriverExt(NULL, "rszwin");
    GrSetUserEncoding(GRENC_UTF_8);
    GrEventGenWMEnd(GR_GEN_WMEND_YES);

    while (1) {
        int exitloop = 0;
        int yhelptext;
        char s[121];
        GrEvent ev;
        GrContext *ctx = NULL;

        GrSetMode(GR_width_height_bpp_graphics, gwidth, gheight, gbpp);
        GrClearScreen(GrWhite());

        yhelptext = GrScreenY() - 60;
        sprintf(s, "file: %s", fname);
        GrTextXY(10, yhelptext+6, s, GrBlack(), GrNOCOLOR);
        GrTextXY(10, yhelptext+24,
                 "mode: [f] [p] [s]  adj: [l] [c] [r]  bgcolor: [b] [w]  quit: [Esc]",
                 GrBlack(), GrNOCOLOR);
        GrTextXY(10, yhelptext+42,
                 "zoom: [+] [-]  rotate: [<] [>]  move: [cursor-keys]  restart: [z]",
                  GrBlack(), GrNOCOLOR);

        ctx = GrCreateSubContext(10, 10, GrScreenX()-10, yhelptext, NULL, NULL);
        GrSetContext(ctx);
        GrEventInit();
        GrMouseDisplayCursor();

        setlocale(LC_NUMERIC, "C");

        while (1) {
            int error;

            error = DrawSvgFile(fname, rotang, &sdm);
            if (error) {
                printf("Error %d reading %s\n", error, fname);
                break;
            }
            GrEventWait(&ev);
            if (((ev.type == GREV_KEY) && (ev.p1 == GrKey_Escape)) ||
                 (ev.type == GREV_WMEND)) {
                exitloop = 1;
                break;
            }
            if (ev.type == GREV_KEY) {
                if (ev.p1 == 'b') sdm.bg = GrBlack();
                else if (ev.p1 == 'w') sdm.bg = GrWhite();
                else if (ev.p1 == 'f') sdm.mode = SVGDRAWMODE_FIT;
                else if (ev.p1 == 'p') sdm.mode = SVGDRAWMODE_PAR;
                else if (ev.p1 == 's') sdm.mode = SVGDRAWMODE_SCOORD;
                else if (ev.p1 == 'l') {
                    sdm.adj = SVGDRAWADJ_LEFT;
                    sdm.xdespl = sdm.ydespl = 0; }
                else if (ev.p1 == 'c') {
                    sdm.adj = SVGDRAWADJ_CENTER;
                    sdm.xdespl = sdm.ydespl = 0; }
                else if (ev.p1 == 'r') {
                    sdm.adj = SVGDRAWADJ_RIGHT;
                    sdm.xdespl = sdm.ydespl = 0; }
                else if (ev.p1 == '+') {
                    sdm.zoom = sdm.zoom * 2;
                    sdm.xdespl *= 2;
                    sdm.ydespl *= 2; }
                else if (ev.p1 == '-') {
                    sdm.zoom = sdm.zoom / 2;
                    sdm.xdespl /= 2;
                    sdm.ydespl /= 2; }
                else if (ev.p1 == '>') rotang++;
                else if (ev.p1 == '<') rotang--;
                else if (ev.p1 == GrKey_Left)  sdm.xdespl -= 10;
                else if (ev.p1 == GrKey_Right)  sdm.xdespl += 10;
                else if (ev.p1 == GrKey_Up)  sdm.ydespl -= 10;
                else if (ev.p1 == GrKey_Down)  sdm.ydespl += 10;
                else if (ev.p1 == 'z') {
                    sdm.bg = GrBlack();
                    sdm.mode = SVGDRAWMODE_PAR;
                    sdm.adj = SVGDRAWADJ_LEFT;
                    sdm.zoom = 1.0;
                    sdm.xdespl = 0;
                    sdm.ydespl = 0;
                    rotang = 0;
                }
            }
            if (ev.type == GREV_WSZCHG) {
                gwidth = ev.p3;
                gheight = ev.p4;
                break;
            }
        }

        if (ctx) GrDestroyContext(ctx);
        GrEventUnInit();
        if (exitloop) break;
    }

    GrSetMode(GR_default_text);

    return 0;
}
