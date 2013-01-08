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
#include <string.h>
#include <ctype.h>
#include "util.h"

char *strncpy0(char *destination, const char *source, size_t num){
   strncpy(destination, source, num );
   destination[num  - 1] = '\0';
   return destination;
}

/*int strcmpi(const char *str1, const char *str2){
   //_memicmp can be used in microsoft windows...
   while(toupper(*str1)==toupper(*str2)){
      if(*str1 == '\0'){
         return 0;
      }
      str1++;
      str2++;
   }
   return toupper(*(unsigned const char *)str1)-toupper(*(unsigned const char *)(str2));
}*/

int strncmpi(const char *str1, const char *str2, unsigned int size){
   //_memicmp can be used in microsoft windows...
   for(;size>0;size--){
      if(toupper(*str1)!=toupper(*str2)){
         return toupper(*(unsigned const char *)str1)-toupper(*(unsigned const char *)(str2));
      }
      str1++;
      str2++;
   }
   return 0;
}

char *strstri(char *str1, const char *str2){
   int str2size = strlen(str2);
   int i;
   for(i=0; str1[i] != '\0'; i++){
      if(strncmpi(str1+i,str2,str2size)==0){
         return str1+i;
      }
   }
   return NULL;
}
