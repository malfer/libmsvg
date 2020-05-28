/* tmatrix.c
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

#include <math.h>
#include "msvg.h"

static TMatrix identity_matrix = {1, 0, 0, 1, 0, 0};

void TMSetIdentity(TMatrix *des)
{
    *des = identity_matrix;
}

int TMIsIdentity(TMatrix *t)
{
    if (t->a != identity_matrix.a) return 0;
    if (t->b != identity_matrix.b) return 0;
    if (t->c != identity_matrix.c) return 0;
    if (t->d != identity_matrix.d) return 0;
    if (t->e != identity_matrix.e) return 0;
    if (t->f != identity_matrix.f) return 0;
    return 1;
}

int TMHaveRotation(TMatrix *t)
{
    if (t->b != 0) return 1;
    if (t->c != 0) return 1;
    return 0;
}

void TMSetFromArray(TMatrix *des, double *p)
{
    des->a = p[0];
    des->b = p[1];
    des->c = p[2];
    des->d = p[3];
    des->e = p[4];
    des->f = p[5];
}

void TMMpy(TMatrix *des, TMatrix *op1, TMatrix *op2)
{
    des->a = op1->a * op2->a + op1->c * op2->b;
    des->b = op1->b * op2->a + op1->d * op2->b;
    des->c = op1->a * op2->c + op1->c * op2->d;
    des->d = op1->b * op2->c + op1->d * op2->d;
    des->e = op1->a * op2->e + op1->c * op2->f + op1->e;
    des->f = op1->b * op2->e + op1->d * op2->f + op1->f;
}

void TMSetTranslation(TMatrix *des, double tx, double ty)
{
    des->a = 1;
    des->b = 0;
    des->c = 0;
    des->d = 1;
    des->e = tx;
    des->f = ty;
}

void TMSetScaling(TMatrix *des, double sx, double sy)
{
    des->a = sx;
    des->b = 0;
    des->c = 0;
    des->d = sy;
    des->e = 0;
    des->f = 0;
}

void TMSetRotationOrigin(TMatrix *des, double ang)
{
    double cosang, sinang, rad;

    rad = ang * 0.0174532925199;    
    cosang = cos(rad);
    sinang = sin(rad);

    des->a = cosang;
    des->b = sinang;
    des->c = -sinang;
    des->d = cosang;
    des->e = 0;
    des->f = 0;
}

void TMSetRotation(TMatrix *des, double ang, double cx, double cy)
{
    TMatrix op1, op2, op3;

    TMSetTranslation(&op1, cx, cy);
    TMSetRotationOrigin(&op2, ang);
    TMMpy(&op3, &op1, &op2);
    TMSetTranslation(&op1, -cx, -cy);
    TMMpy(des, &op3, &op1);
}

void TMTransformCoord(double *x, double *y, TMatrix *ctm)
{
    double xorg, yorg;

    xorg = *x;
    yorg = *y;

    *x = ctm->a * xorg + ctm->c * yorg + ctm->e;
    *y = ctm->b * xorg + ctm->d * yorg + ctm->f;
}
