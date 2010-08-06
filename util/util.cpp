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
      for(j=0; str2[j] != '\0'; j++){
         if(toupper(str2[j])==toupper(str1[i+j])){
            continue; 
         }else{
            break;
         }
      }
      if (str2[j] == '\0'){
         break;
      }
   }
   if (str2[j] == '\0'){
      return (i+str1);
   }else{
      return '\0';
   }
}
