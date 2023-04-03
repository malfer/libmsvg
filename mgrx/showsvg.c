/* showsvg.c ---- show a svg file, MGRX test
 *
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020-2023 Mariano Alvarez Fernandez
 * (malfer at telefonica.net)
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

static MsvgElement *LoadSvgFile(char *fname, int *error)
{
    // error=
    //
    // if MsvgReadSvgFile fail
    // >0 expat error
    // -1 error opening file
    // -2 memory error creating parser
    // -3 memory error building the tree
    //
    // -5 MsvgRaw2CookedTree failed

    MsvgElement *root;
    double gminx, gmaxx, gminy, gmaxy;

    *error = 0;
    root = MsvgReadSvgFile(fname, error);
    if (root == NULL) return NULL;

    if (MsvgRaw2CookedTree(root) != 1) {
        *error = -5;
        return NULL;
    }

    printf("Declared dimensions minx:%g  miny:%g  width:%g  height:%g\n",
            root->psvgattr->vb_min_x, root->psvgattr->vb_min_y,
            root->psvgattr->vb_width, root->psvgattr->vb_height);
    if (MsvgGetCookedDims(root, &gminx, &gmaxx, &gminy, &gmaxy)) {
        printf("Calculated dimensions minx:%g  miny:%g  width:%g  height:%g\n",
                gminx, gminy, gmaxx-gminx, gmaxy-gminy);
    }

    return root;
}

typedef struct _Repdata {
    int nte;            // num of EID_TEXT elements
    int max;            // max EID_TEXT elements
    MsvgElement *el[1]; // elements (not actual size)
} Repdata;

static void reptext(MsvgElement *el, void *udata)
{
    Repdata *rd;

    rd = (Repdata *)udata;

    if (el->eid == EID_TEXT) {
        if (rd->nte < rd->max) {
            rd->el[rd->nte] = el;
            rd->nte++;
        }
    }
}

static void ReplaceTextByPaths(MsvgElement *root)
{
    MsvgTreeCounts tc;
    MsvgBFont *bfont;
    Repdata *rd;
    int nte, i;

    MsvgCalcCountsCookedTree(root, &tc);
    if (tc.nelem[EID_TEXT] < 0) return; // nothing to do

    if (tc.nelem[EID_FONT] > 0) { // we have one or more internal fonts, load them
        MsvgBFontLibLoad(root);
    }

    // now load three standard fonts sans, serif & mono
    MsvgBFontLibLoadFromFile("../gfonts/rsans.svg");
    MsvgBFontLibLoadFromFile("../gfonts/rserif.svg");
    MsvgBFontLibLoadFromFile("../gfonts/rmono.svg");

    // do the work
    nte = tc.nelem[EID_TEXT];
    rd = malloc(sizeof(Repdata)+sizeof(MsvgElement *)*nte);
    if (rd == NULL) { // opss no memory
        MsvgBFontLibFree();
        return;
    }
    rd->nte = 0;
    rd->max = nte;

    MsvgWalkTree(root, reptext, rd);

    for(i=0; i<rd->nte; i++) {
        MsvgElement *group;

        if (rd->el[i]->eid == EID_TEXT) {
            bfont = MsvgBFontLibFind(rd->el[i]->pctx->sfont_family,
                                     rd->el[i]->pctx->ifont_family);
            if (bfont != NULL) {
                group = MsvgTextToPathGroup(rd->el[i], bfont);
                if (group) {
                    if (MsvgReplaceElement(rd->el[i], group))
                    MsvgDeleteElement(rd->el[i]);
                }
            }
        }
    }

    MsvgBFontLibFree();
    free(rd);
}

int main(int argc,char **argv)
{
    GrSVGDrawMode sdm = {SVGDRAWMODE_PAR, SVGDRAWADJ_LEFT, 1.0, 0, 0, 0, 0};
    char *fname;
    MsvgElement *root;
    int error;

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

    root = LoadSvgFile(fname, &error);
    if (root == NULL) {
        printf("Error %d opening %s\n", error, fname);
        return 1;
    }

    ReplaceTextByPaths(root);
    MsvgNormalizeRawGradients(root);

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
        int mouseoldx = 0, mouseoldy = 0;
        int rewrite = 1;

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

        sdm.bg = GrWhite();

        while (1) {
            int error;

            if (rewrite) {
                error = GrDrawSVGtreeUsingDB(root, &sdm);
                if (error) {
                    if (error == -4) { // Possible overflow
                        printf("Possible overflow %g %g\n",
                               root->psvgattr->vb_width* sdm.zoom,
                               root->psvgattr->vb_height* sdm.zoom);
                    } else {
                        printf("Error %d drawing %s\n", error, fname);
                        exitloop = 1;
                        break;
                    }
                }
            }
            GrEventWait(&ev);
            if (((ev.type == GREV_KEY) && (ev.p1 == GrKey_Escape)) ||
                 (ev.type == GREV_WMEND)) {
                exitloop = 1;
                break;
            }
            rewrite = 1;
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
                else if (ev.p1 == '>') sdm.rotang++;
                else if (ev.p1 == '<') sdm.rotang--;
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
                    sdm.rotang = 0; }
                else rewrite = 0;
            }
            else if (ev.type == GREV_MOUSE) {
                 if (ev.p1 == GRMOUSE_B4_RELEASED) {
                    sdm.zoom = sdm.zoom * 2;
                    sdm.xdespl *= 2;
                    sdm.ydespl *= 2; }
                else if (ev.p1 == GRMOUSE_B5_RELEASED) {
                    sdm.zoom = sdm.zoom / 2;
                    sdm.xdespl /= 2;
                    sdm.ydespl /= 2; }
                else if (ev.p1 == GRMOUSE_LB_PRESSED) {
                    mouseoldx = ev.p2;
                    mouseoldy = ev.p3;
                    rewrite = 0; }
                else if (ev.p1 == GRMOUSE_LB_RELEASED) {
                    sdm.xdespl += ev.p2 - mouseoldx;
                    sdm.ydespl += ev.p3 - mouseoldy; }
                else if (ev.p1 == GRMOUSE_RB_PRESSED) {
                    mouseoldx = ev.p2;
                    rewrite = 0; }
                else if (ev.p1 == GRMOUSE_RB_RELEASED) {
                    sdm.rotang += ev.p2 - mouseoldx; }
                else rewrite = 0;
            }
            else if (ev.type == GREV_WSZCHG) {
                gwidth = ev.p3;
                gheight = ev.p4;
                break;
            }
            else rewrite = 0;
        }

        if (ctx) GrDestroyContext(ctx);
        GrEventUnInit();
        if (exitloop) break;
    }

    GrSetMode(GR_default_text);
    MsvgDeleteElement(root);

    return 0;
}
