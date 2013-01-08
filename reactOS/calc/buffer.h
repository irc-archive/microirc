/*
* buffer.h
*
* This file contains an buffer implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#ifndef BUFFER_T
#define BUFFER_T

#define BUFFER_DEFAULT_SIZE 262144

typedef struct buffer_t{
   int actual_size;
   char *data;
   int size;
   char *d_split;
   int s_split;
   int max_data_size;
}buffer_t;

export int buffer_init(buffer_t*, char*, int, int, int);
export void buffer_destroy(buffer_t*);
export void buffer_print(buffer_t*);
export int buffer_validate_data(buffer_t *buffer, char *data);

export int buffer_get_avaiable_write_size(buffer_t*);
export char *buffer_get_avaiable_write_buffer(buffer_t*);

export int buffer_get_avaiable_data_size(buffer_t*);
export char *buffer_get_avaiable_data_buffer(buffer_t*);

export int buffer_write_size_before(buffer_t*, int);
export int buffer_write_data(buffer_t*, char*, int);
export int buffer_write_size_after(buffer_t*, int);

export int buffer_read_data(buffer_t*);
export int buffer_read_get_data(buffer_t*, char*, int);

#endif
