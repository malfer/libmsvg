/* pathmgrx.h
 * 
 * This is a dirty hack to test the libmsvg librarie with the MGRX
 * graphics library. It is NOT part of the libmsvg librarie really.
 *
 * In the future this will be added to MGRX, this is why the LGPL is aplied
 *
 * Copyright (C) 2010, 2020 Mariano Alvarez Fernandez (malfer at telefonica.net)
 *
 * This source is free software; you can redistribute it and/or
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

typedef struct {
    int x;     // x pos
    int y;     // y pos
    char cmd;  // one of 'M', 'L', 'C', 'Q', ' '
} GrPathPoint;

typedef struct {
    int maxpoints;           // max capacity (realloc if necesary)
    int npoints;             // actual number of points
    int closed;              // 1 = yes, 0 = no
    int failed_realloc;      // 1 = yes, 0 = no
    GrPathPoint *pp;         // Path points
} GrPath;

typedef struct {
    int maxpoints;           // max capacity (realloc if necesary)
    int npoints;             // actual number of points
    int closed;              // 1 = yes, 0 = no
    int failed_realloc;      // 1 = yes, 0 = no
    int (*points)[2];        // Points array
} GrExpPointArray;

GrPath * GrNewPath(int maxpoints);
void GrExpandPath(GrPath *gp);
void GrAddPointToPath(GrPath *gp, char cmd, int x, int y);
void GrDestroyPath(GrPath *gp);

GrExpPointArray * GrNewExpPointArray(int maxpoints);
void GrExpandExpPointArray(GrExpPointArray *pa);
void GrAddPointToExpPointArray(GrExpPointArray *pa, int x, int y);
void GrDestroyExpPointArray(GrExpPointArray *pa);

int GrReducePoints(int numpts, int points[][2]);
GrExpPointArray * GrPathToExpPointArray(GrPath *gp);
