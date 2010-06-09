/*
* iniparser.h
*
* This file contains an iniparser implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#ifndef INIPARSER_T
#define INIPARSER_T

#define INIPARSER_MAX_LINE 250
#define INIPARSER_MAX_SECTION 50
#define INIPARSER_MAX_KEY 50
#define INIPARSER_MAX_VALUE 250

#define INIPARSER_TYPE_ERROR 1
#define INIPARSER_TYPE_EMPTY 2
#define INIPARSER_TYPE_COMMENT 3
#define INIPARSER_TYPE_SECTION 4
#define INIPARSER_TYPE_KEYVALUE 5

typedef struct iniparser_t{
   list_t list;
}iniparser_t;

typedef struct iniparsenode_t{
   int type;
   char key[INIPARSER_MAX_KEY];
   char value[INIPARSER_MAX_VALUE];
}iniparsenode_t;

__declspec(dllexport) int iniparser_init(iniparser_t*);
__declspec(dllexport) void iniparser_destroy(iniparser_t*);

__declspec(dllexport) int iniparser_load(iniparser_t*, char*);
__declspec(dllexport) int iniparser_store(iniparser_t*, char*);

__declspec(dllexport) char *iniparser_getstring(iniparser_t*, char*, char*, char*);
__declspec(dllexport) int iniparser_setstring(iniparser_t*, char*, char*, char*);
__declspec(dllexport) int iniparser_getint(iniparser_t*, char*, char*, int);
__declspec(dllexport) int iniparser_setint(iniparser_t*, char*, char*, int);
__declspec(dllexport) double iniparser_getdouble(iniparser_t*, char*, char*, double);
__declspec(dllexport) int iniparser_setdouble(iniparser_t*, char*, char*, double);

#endif
