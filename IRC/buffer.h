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
   char *data;
   int size;
   int actual_size;
   char *d_split;
   int s_split;
}buffer_t;

__declspec(dllexport) int buffer_init(buffer_t*, char*, int, int);
__declspec(dllexport) void buffer_destroy(buffer_t*);
__declspec(dllexport) void buffer_print(buffer_t*);
__declspec(dllexport) int buffer_line_len(buffer_t*, char*, int);

__declspec(dllexport) int buffer_read_avaiable_write_size(buffer_t*);
__declspec(dllexport) char *buffer_read_avaiable_write_buffer(buffer_t*);

__declspec(dllexport) int buffer_read_avaiable_data_size(buffer_t*);
__declspec(dllexport) char *buffer_read_avaiable_data_buffer(buffer_t*);

__declspec(dllexport) int buffer_write_size_before(buffer_t*, int);
__declspec(dllexport) int buffer_write_data(buffer_t*, char*, int);
__declspec(dllexport) int buffer_write_size_after(buffer_t*, int);

__declspec(dllexport) int buffer_read_data(buffer_t*);
__declspec(dllexport) int buffer_read_get_data(buffer_t*, char*, int);

#endif
