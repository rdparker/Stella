/*
    Copyright (c) 1994,2001-2002 Nick Ing-Simmons. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA

*/
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "charset.h"

void
init_locale(void)
{
 char *s = setlocale(LC_ALL, "");
 if (s)
  {
   s = setlocale(LC_CTYPE, NULL);
   if (s && !strcmp(s, "C"))
    {
     s = setlocale(LC_CTYPE, "iso_8859_1");
     if (!s)
      s = setlocale(LC_CTYPE, "C");
    }
  }
}

int
deaccent(int ch)
{
 /* Cast to char (type of litterals) as signedness may differ */
 switch ((char)ch)
  {
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
    return 'a';
   case '�':
    return 'c';
    break;
   case '�':
   case '�':
   case '�':
   case '�':
    return 'e';
   case '�':
   case '�':
   case '�':
   case '�':
    return 'i';
   case '�':
    return 'n';
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
    return 'o';
   case '�':
   case '�':
   case '�':
   case '�':
    return 'u';
   case '�':
   case '�':
    return 'y';
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
    return 'A';
   case '�':
    return 'C';
   case '�':
   case '�':
   case '�':
   case '�':
    return 'E';
   case '�':
   case '�':
   case '�':
   case '�':
    return 'I';
   case '�':
    return 'N';
   case '�':
   case '�':
   case '�':
   case '�':
   case '�':
    return 'O';
   case '�':
   case '�':
   case '�':
   case '�':
    return 'U';
   case '�':
    return 'Y';
  }
 if ((ch & 0xFF) > 0x7F)
  abort();
 return ch;
}

int
accent(int a, int c)
{
 if (a && c)
  {
   switch (a)
    {
     case '<':
     case ',':
      switch (c)
       {
        case 'c':
         return '�';
        case 'C':
         return '�';
        default:
         return c;
       }
     case '~':
      switch (c)
       {
        case 'n':
         return '�';
        case 'a':
         return '�';
        case 'o':
         return '�';
        case 'N':
         return '�';
        case 'A':
         return '�';
        case 'O':
         return '�';
        default:
         return c;
       }
     case '\'':
      switch (c)
       {
        case 'a':
         return '�';
        case 'e':
         return '�';
        case 'i':
         return '�';
        case 'o':
         return '�';
        case 'u':
         return '�';
        case 'y':
         return '�';
        case 'A':
         return '�';
        case 'E':
         return '�';
        case 'I':
         return '�';
        case 'O':
         return '�';
        case 'U':
         return '�';
        case 'Y':
         return '�';
        default:
         return c;
       }
     case '`':
      switch (c)
       {
        case 'a':
         return '�';
        case 'e':
         return '�';
        case 'i':
         return '�';
        case 'o':
         return '�';
        case 'u':
         return '�';
        case 'A':
         return '�';
        case 'E':
         return '�';
        case 'I':
         return '�';
        case 'O':
         return '�';
        case 'U':
         return '�';
        default:
         return c;
       }
     case '^':
      switch (c)
       {
        case 'a':
         return '�';
        case 'e':
         return '�';
        case 'i':
         return '�';
        case 'o':
         return '�';
        case 'u':
         return '�';
        case 'A':
         return '�';
        case 'E':
         return '�';
        case 'I':
         return '�';
        case 'O':
         return '�';
        case 'U':
         return '�';
        default:
         return c;
       }
     case '"':
      switch (c)
       {
        case 'a':
         return '�';
        case 'e':
         return '�';
        case 'i':
         return '�';
        case 'o':
         return '�';
        case 'u':
         return '�';
        case 'y':
         return '�';
        case 'A':
         return '�';
        case 'E':
         return '�';
        case 'I':
         return '�';
        case 'O':
         return '�';
        case 'U':
         return '�';
        default:
         return c;
       }
    }
  }
 return c;
}
