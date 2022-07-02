/* util.c
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
#include <stdio.h>
#include <string.h>
//#include <locale.h>
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
        } else {
            p++;
        }
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
    //struct lconv *lcv;

    //lcv = localeconv();
    p = s;
    while (*p != '\0') {
        if (isdigit(*p) || *p == '.' || *p == '-') {
            if (n >= maxnumbers) break;
            aux[0] = *p;
            // better call setlocale(LC_NUMERIC, "C"); before calling Msvg functions
            //if (aux[0] == '.') aux[0] = lcv->decimal_point[0];
            dig = 1;
            p++;
            while (isdigit(*p) || *p == '.') {
                if (dig < MAX_DIGITS) aux[dig] = *p;
                //if (aux[dig] == '.') aux[dig] = lcv->decimal_point[0];
                dig++;
                p++;
            }
            aux[dig] = '\0';
            df[n++] = atof(aux);
        } else {
            p++;
        }
    }
    
    return n;
}

char *MsvgI_rmspaces(char *s)
{
    char *p;
    int len;

    if (s == NULL) return NULL;

    p = s;
    
    while (isspace(*p)) p++;

    len = strlen(p);
    while (len > 0 && isspace(p[len-1])) len--;
    p[len] = '\0';

    return p;
}

long MsvgI_NextUCPfromUTF8Str(const unsigned char *s, int *nb)
{
    *nb = 1;

    // ASCII (1 byte)
    if (s[0] < 0x80) {
        return s[0];
    }

    // two bytes
    if (s[0] >= 0xC2 && s[0] <= 0xDF) {
        if ((s[1] & 0xC0) != 0x80) {
            goto ILLFORMED;
        }
        *nb = 2;
        return (((long)(s[0]) & 0x1f) << 6) |
               ((long)(s[1]) & 0x3f);
    }

    // three bytes
    if ((s[0] & 0xf0) == 0xe0) {
        if (s[0] == 0xE0) {
            if (s[1] < 0xA0 || s[1] > 0xBF) {
                goto ILLFORMED;
            }
        } else if (s[0] == 0xED) {
            if (s[1] < 0x80 || s[1] > 0x9F) {
                goto ILLFORMED;
            }
        } else {
            if ((s[1] & 0xC0) != 0x80) {
            goto ILLFORMED;
            }
        }
        if ((s[2] & 0xC0) != 0x80) {
            *nb = 2;
            goto ILLFORMED;
        }
        *nb = 3;
        return (((long)(s[0]) & 0x0f) << 12) |
               (((long)(s[1]) & 0x3f) << 6) |
               ((long)(s[2]) & 0x3f);
    }

    // four bytes
    if (s[0] >= 0xF0 && s[0] <= 0xF4) {
        if (s[0] == 0xF0) {
            if (s[1] < 0x90 || s[1] > 0xBF) {
                goto ILLFORMED;
            }
        } else if (s[0] == 0xF4) {
            if (s[1] < 0x80 || s[1] > 0x8F) {
                goto ILLFORMED;
            }
        } else {
            if ((s[1] & 0xC0) != 0x80) {
            goto ILLFORMED;
            }
        }
        if ((s[2] & 0xC0) != 0x80) {
            *nb = 2;
            goto ILLFORMED;
        }
        if ((s[3] & 0xC0) != 0x80) {
            *nb = 3;
            goto ILLFORMED;
        }
        *nb = 4;
        return (((long)(s[0]) & 0x07) << 18) |
               (((long)(s[1]) & 0x3f) << 12) |
               (((long)(s[2]) & 0x3f) << 6) |
               ((long)(s[3]) & 0x3f);
    }

    // Here for forbiden bytes in UTF-8 C0-C1, F5-FF

ILLFORMED:
    return (long)0xFFFD;
}
