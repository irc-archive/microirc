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

#define INIPARSER_MAX_LINE 512
#define INIPARSER_MAX_SECTION 64
#define INIPARSER_MAX_KEY 64
#define INIPARSER_MAX_VALUE 512

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

export int iniparser_init(iniparser_t*);
export void iniparser_destroy(iniparser_t*);

export int iniparser_load(iniparser_t*, char*);
export int iniparser_store(iniparser_t*, char*);

export char *iniparser_getstring(iniparser_t*, char*, char*, char*);
export int iniparser_setstring(iniparser_t*, char*, char*, char*);
export int iniparser_getint(iniparser_t*, char*, char*, int);
export int iniparser_setint(iniparser_t*, char*, char*, int);
export double iniparser_getdouble(iniparser_t*, char*, char*, double);
export int iniparser_setdouble(iniparser_t*, char*, char*, double);

#endif
