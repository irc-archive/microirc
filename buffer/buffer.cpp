#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

int grow(buffer_t *buffer, int needed_size, int start_new_size, int n_times){
   while(start_new_size <= needed_size){
      start_new_size *= n_times;
   }
   char *new_data = (char*)malloc(start_new_size);
   if(new_data == NULL){
      return -1;
   }
   memcpy(new_data, buffer->data, buffer->size);
   free(buffer->data);
   buffer->data = new_data;
   buffer->actual_size = start_new_size;
   return 0;
}

int buffer_grow_shrink(buffer_t *buffer, int n_size){
   int new_size = buffer->size+n_size;
   if(new_size > buffer->actual_size){
      return grow(buffer, new_size, BUFFER_DEFAULT_SIZE, 2);
   }else if(new_size == buffer->actual_size){
      return grow(buffer, new_size, BUFFER_DEFAULT_SIZE, 4);
   }else if(new_size < (buffer->actual_size/2) && buffer->actual_size > BUFFER_DEFAULT_SIZE){
      new_size = buffer->actual_size/2;
      char *new_data = (char*)malloc(new_size);
      if(new_data == NULL){
         return 0;
      }
      memcpy(new_data, buffer->data, buffer->size);
      free(buffer->data);
      buffer->data = new_data;
      buffer->actual_size = new_size;
      return 0;
   }else{
      return 0;
   }
}

__declspec(dllexport) int buffer_init(buffer_t *buffer, char *d_split, int s_split, int max_size){
   memset(buffer,0,sizeof(buffer_t));
   if(max_size<=0){
      max_size = BUFFER_DEFAULT_SIZE;
   }
   buffer->data = (char*)malloc(max_size);
   if(buffer->data==NULL){
      return -1;
   }
   buffer->d_split = (char*)malloc(s_split);
   if(buffer->d_split==NULL){
      free(buffer->data);
      return -1;
   }
   memcpy(buffer->d_split,d_split,s_split);
   buffer->s_split = s_split;
   buffer->actual_size = max_size;
   buffer->size = 0;
   return 0;
}

__declspec(dllexport) void buffer_destroy(buffer_t *buffer){
   free(buffer->data);
   free(buffer->d_split);
   memset(buffer,0,sizeof(buffer_t));
}

__declspec(dllexport) void buffer_print(buffer_t *buffer){
   int i;
   char *data = buffer_read_avaiable_data_buffer(buffer);
   int size = buffer_read_avaiable_data_size(buffer);
   printf("BUFFER_T size:%d ",size);
   for(i=0;i<size;i++){
      printf("%c",*(data+i));
   }
   printf(".\n");
}

__declspec(dllexport) int buffer_line_len(buffer_t *buffer, char *optional_data, int optional_size){
   int start = 0;
   if(optional_data==NULL || optional_size==0){
      optional_data = buffer->data;
      optional_size = buffer->actual_size - buffer->s_split;
   }
   while(memcmp(optional_data,buffer->d_split,buffer->s_split) && start<=optional_size){
      optional_data++;
      start++;
   }
   return start;
}

__declspec(dllexport) int buffer_read_avaiable_write_size(buffer_t *buffer){
   return (buffer->actual_size)-(buffer->size);
}

__declspec(dllexport) char *buffer_read_avaiable_write_buffer(buffer_t *buffer){
   return (buffer->data)+(buffer->size);
}

__declspec(dllexport) int buffer_read_avaiable_data_size(buffer_t *buffer){
   int retval = buffer_line_len(buffer, NULL, 0);
   if(retval <= (buffer->size-buffer->s_split)){
      retval += buffer->s_split;
      return retval;
   }else{
      return 0;
   }
}

__declspec(dllexport) char *buffer_read_avaiable_data_buffer(buffer_t *buffer){
   return buffer->data;
}

__declspec(dllexport) int buffer_write_size_before(buffer_t *buffer, int size){
   if(buffer_grow_shrink(buffer,size)<0){
      return -1;
   }
   return buffer_read_avaiable_data_size(buffer);
}

__declspec(dllexport) int buffer_write_data(buffer_t *buffer, char *d_data, int s_data){
   if(buffer_grow_shrink(buffer,s_data)<0){
      return -1;
   }
   memcpy((buffer->data)+(buffer->size),d_data,s_data);
   buffer->size = buffer->size + s_data;
   return buffer_read_avaiable_data_size(buffer);
}

__declspec(dllexport) int buffer_write_size_after(buffer_t *buffer, int size){
   buffer->size = buffer->size + size;
   buffer_grow_shrink(buffer,0);
   return buffer_read_avaiable_data_size(buffer);
}

__declspec(dllexport) int buffer_read_data(buffer_t *buffer){
   int retval = buffer_read_avaiable_data_size(buffer);
   if(retval>0){
      memcpy(buffer->data,buffer->data+retval,buffer->size-retval);
      buffer->size = buffer->size - retval;
      buffer_grow_shrink(buffer,0);
   }
   return retval;
}

__declspec(dllexport) int buffer_read_get_data(buffer_t *buffer, char *data, int size){
   int retval = buffer_read_avaiable_data_size(buffer);
   if(retval>size){
      return -1;
   }else if(retval>0){
      memcpy(data,buffer->data,retval);
      memcpy(buffer->data,buffer->data+retval,buffer->size-retval);
      buffer->size = buffer->size - retval;
      buffer_grow_shrink(buffer,0);
   }
   return retval;
}
