/*
* list.h
*
* This file contains an list implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#ifndef LIST_T
#define LIST_T

#define LIST_MAX_SIZE 268435456
#define LIST_MIN_SIZE 1024

typedef struct list_t{
   int size;
   int data_size;
   int current_size;
   char *list;
}list_t;

export int list_init(list_t*, int);
export void list_destroy(list_t*);

export void *list_add(list_t*, void*);
export void *list_add_index(list_t*, int, void*);
export int list_get(list_t*, void*);
export void *list_get_index(list_t*, int, void*, int);
export int list_remove(list_t*, void*);
export int list_remove_index(list_t*, int, void*, int);
export int list_size(list_t*);

export int list_array_data(list_t*,void**);
export int list_array_references(list_t*,void**);
export void list_array_destroy(void*);

#endif
