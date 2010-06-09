#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int grow_shrink(list_t *list, int new_size){
   char *new_list = (char*)malloc(new_size*sizeof(char*));
   if(new_list==NULL){
      return -1;
   }
   memcpy(new_list,list->list,list->size*sizeof(void*));
   free(list->list);
   list->list=new_list;
   return 0;
}

__declspec(dllexport) int list_init(list_t *list, int data_size){
   memset(list,0,sizeof(list_t));
   list->current_size = LIST_MIN_SIZE;
   list->list = (char*)malloc(list->current_size*sizeof(char*));
   if(list->list==NULL){
      return -1;
   }
   list->data_size = data_size;
   list->size = 0;
   return 0;
}

__declspec(dllexport) void list_destroy(list_t *list){
   int i;
   void *element;
   for(i=0;i<list->size;i++){
      memcpy(&element,(list->list)+(i*sizeof(char*)),sizeof(char*));
      free(element);
   }
   free(list->list);
   memset(list,0,sizeof(list_t));
}

__declspec(dllexport) void *list_add(list_t *list, void *data){
   return list_add_index(list,list->size,data);
}

__declspec(dllexport) void *list_add_index(list_t *list, int index, void *data){
   if(index <0 || index>list->size){
      return NULL;
   }
   if(list->size==list->current_size && list->current_size<LIST_MAX_SIZE){
      if(grow_shrink(list,list->current_size*2)!=0){
         return NULL;
      }
      list->current_size = list->current_size*2;
   }
   void *element = (void*)malloc(list->data_size);
   if(element==NULL){
      return NULL;
   }
   memcpy(element,data,list->data_size);
   memcpy((list->list)+((index+1)*sizeof(char*)),(list->list)+(index*sizeof(char*)),(list->size-index)*sizeof(char*));
   memcpy((list->list)+(index*sizeof(char*)),&element,sizeof(char*));
   list->size++;
   return element;
}

__declspec(dllexport) int list_get(list_t *list, void *data){
   int result = -1;
   int i;
   void *element;
   for(i=0;i<list->size;i++){
      memcpy(&element,(list->list)+(i*sizeof(char*)),sizeof(char*));
      if(memcmp(element,data,list->data_size)==0){
         result = i;
         i = list->size;
      }
   }
   return result;
}

__declspec(dllexport) void *list_get_index(list_t *list, int index, void *d_data, int s_data){
   if(index <0 || index>=list->size){
      return NULL;
   }
   void *element;
   memcpy(&element,(list->list)+(index*sizeof(char*)),sizeof(char*));
   if(d_data!=NULL && s_data>=list->data_size){
      memcpy(d_data,element,list->data_size);
   }
   return element;
}

__declspec(dllexport) int list_remove(list_t *list, void *data){
   int index = list_get(list,data);
   if(index == -1){
      return -1;
   }
   if(list_remove_index(list,index,NULL,0)!=0){
      return -1;
   }
   return 0;
}

__declspec(dllexport) int list_remove_index(list_t *list, int index, void *d_data, int s_data){
   void *element = list_get_index(list,index,d_data,s_data);
   if(element == NULL){
      return -1;
   }
   free(element);
   memcpy(list->list+(index*sizeof(char*)),list->list+((index+1)*sizeof(char*)),(list->size-index-1)*sizeof(char*));
   list->size--;
   if(list->current_size>LIST_MIN_SIZE && (list->current_size/2)>=(list->size+LIST_MIN_SIZE)){
      if(grow_shrink(list,list->current_size/2)==0){
         list->current_size = list->current_size/2;
      }
   }
   return 0;
}

__declspec(dllexport) int list_size(list_t *list){
   return list->size;
}

__declspec(dllexport) int list_array_data(list_t *list, void **datas){
   char *temp = (char*)malloc(list->size*list->data_size);
   if(temp == NULL){
      return -1;
   }
   *datas = temp;
   int i;
   void *element;
   for(i=0;i<list->size;i++){
      memcpy(&element,(list->list)+(i*sizeof(char*)),sizeof(char*));
      memcpy(temp,element,list->data_size);
      temp = temp + list->data_size;
   }
   return list->size;
}

__declspec(dllexport) int list_array_references(list_t *list, void **references){
   void *temp = (void*)malloc(list->size*sizeof(void*));
   if(temp == NULL){
      return -1;
   }
   *references = temp;
   memcpy(temp,list->list,list->size*sizeof(void*));
   return list->size;
}

__declspec(dllexport) void list_array_destroy(void *list){
   free(list);
}
