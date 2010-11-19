/* util.c
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
#include <ctype.h>
#include "util.h"

int MsvgI_count_numbers(char *s)
{
  char *p;
  int n = 0;

  p = s;
  while (*p != '\0') {
    if (isdigit(*p) || *p == '.') {
      n++;
      p++;
      while (isdigit(*p) || *p == '.')
        p++;
    }
    p++;
  }

  return n;
}

int MsvgI_read_numbers(char *s, double *df, int maxnumbers)
{
#define MAX_DIGITS 100
  char aux[MAX_DIGITS+1];
  char *p;
  int n = 0;
  int dig;

  p = s;
  while (*p != '\0') {
    if (isdigit(*p) || *p == '.') {
      if (n >= maxnumbers) break;
      p++;
      dig = 0;
      aux[dig++] = *p;
      while (isdigit(*p) || *p == '.') {
        p++;
        if (dig < MAX_DIGITS) aux[dig++] = *p;
      }
      aux[dig] = '\0';
      df[n++] = atof(aux);
    }
    p++;
  }

  return n;
}
      
