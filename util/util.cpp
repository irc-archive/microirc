/*
* util.h
*
* This file contains functions that complement the standard c library.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

char *strncpy0(char *destination, const char *source, size_t num){
   strncpy(destination, source, num );
   destination[num  - 1] = '\0';
   return destination;
}

char *strstri(char *str1, const char *str2){
   int i, j;
   for(i=0; str1[i] != '\0'; i++){
      if(strstr(str1+i,str2)!=NULL){
         return str1+i;
      }
   }
   return NULL;
}
