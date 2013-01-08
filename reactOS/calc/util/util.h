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

#ifndef UTIL_T
#define UTIL_T

#ifdef _WIN32
   #define export __declspec(dllexport)
#else
   #define export extern
#endif

char *strncpy0(char *destination, const char *source, size_t num);
//int strcmpi(const char *str1, const char *str2);
int strncmpi(const char *str1, const char *str2, unsigned int size);
char *strstri(char *str1, const char *str2);

#endif
