/*
* iniparser.c
*
* This file contains an iniparser implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../util/util.h"
#include "../list/list.h"
#include "iniparser.h"

char *iniparser_rightstrip(char *s){
   char *p = s+strlen(s);
   while(p>s && isspace(*--p)){
      *p = '\0';
   }
   return s;
}

char *iniparser_leftstrip(char *s){
   while(*s && isspace(*s)){
      s++;
   }
   return s;
}

char *iniparser_find_any_chars(char *s, char *d_chars, int s_chars){
   int size;
   while(*s){
      size=s_chars-1;
      while(size>=0){
         if(d_chars[size]==*s){
            return (char*)s;
         }
         size--;
      }
      s++;
   }
   return (char*)s;
}

int iniparser_add_node(iniparser_t *iniparser, int index, int opcode, char *key, char *value){
   iniparsenode_t node;
   memset(&node,0,sizeof(iniparsenode_t));
   node.type = opcode;
   strncpy0(node.key,key,INIPARSER_MAX_KEY);
   strncpy0(node.value,value,INIPARSER_MAX_VALUE);
   if(index>=0){
      if(list_add_index(&iniparser->list,index,&node)==NULL){
         return -1;
      }
   }else{
      if(list_add(&iniparser->list,&node)==NULL){
         return -1;
      }
   }
   return 0;
}

iniparsenode_t *iniparser_get_node(iniparser_t *iniparser, char *section, char *key){
   iniparsenode_t *item;
   int size = list_size(&iniparser->list);
   int i;
   int found_section = 0;
   for(i=0;i<size;i++){
      item = (iniparsenode_t*)list_get_index(&iniparser->list,i,NULL,0);
      if(item!=NULL){
         if(item->type==INIPARSER_TYPE_SECTION){
            if(strncmp(item->key,section,INIPARSER_MAX_KEY)==0){
               found_section = 1;
            }else if(found_section){
               return NULL;
            }
         }else if(item->type==INIPARSER_TYPE_KEYVALUE){
            if(found_section){
               if(strncmp(item->key,key,INIPARSER_MAX_KEY)==0){
                  return item;
               }
            }
         }
      }
   }
   return NULL;
}

int iniparser_get_index(iniparser_t *iniparser, int opcode, char *data){
   iniparsenode_t *item;
   int size = list_size(&iniparser->list);
   int i;
   for(i=0;i<size;i++){
      item = (iniparsenode_t*)list_get_index(&iniparser->list,i,NULL,0);
      if(item!=NULL){
         if(item->type==opcode){
            if(strncmp(item->key,data,INIPARSER_MAX_KEY)==0){
               return i;
            }
         }
      }
   }
   return -1;
}

export int iniparser_init(iniparser_t *iniparser){
   memset(iniparser,0,sizeof(iniparser_t));
   if(list_init(&iniparser->list,sizeof(iniparsenode_t))==-1){
      return -1;
   }
   return 0;
}

export void iniparser_destroy(iniparser_t *iniparser){
   list_destroy(&iniparser->list);
   memset(iniparser,0,sizeof(iniparser_t));
}

export int iniparser_load(iniparser_t *iniparser, char *filename){
   FILE *file = fopen(filename, "r");
   if(file==NULL){
      list_destroy(&iniparser->list);
      return -1;
   }
   iniparsenode_t add;
   memset(&add,0,sizeof(iniparsenode_t));
   char line[INIPARSER_MAX_LINE];
   char section[INIPARSER_MAX_SECTION];
   char *key;
   char *value;
   char *start;
   char *end;
   while(fgets(line, INIPARSER_MAX_LINE, file) != NULL){
      start = iniparser_leftstrip(iniparser_rightstrip(line));
      if(*start == '['){
         end = iniparser_find_any_chars(start+1, "];",2);
         if(*end == ']'){
            *end = '\0';
            strncpy0(section, start+1, INIPARSER_MAX_SECTION);
            iniparser_add_node(iniparser,-1,INIPARSER_TYPE_SECTION,start+1,"");
         }else{
            *section = '\0';
            printf("erro a ver a section\n");
         }
      }else if(*start==';'){
         iniparser_add_node(iniparser,-1,INIPARSER_TYPE_COMMENT,"",start+1);
      }else if(*section!='\0' && *start!='\0'){
         end = iniparser_find_any_chars(start, "=;",2);
         if(*end == '='){
            *end = '\0';
            key = iniparser_rightstrip(start);
            value = iniparser_leftstrip(end+1);
            end = iniparser_find_any_chars(value, ";",1);
            if(*end == ';'){
               *end = '\0';
            }
            iniparser_rightstrip(value);
            iniparser_add_node(iniparser,-1,INIPARSER_TYPE_KEYVALUE,key,value);
         }else{
            printf("linha com texto mas sem ser key=data\n");
         }
      }else if(*start!='\0'){
         printf("linha, nao é coment, nao é section, nao é branca, provavelmente section invalida detectada\n");
      }else{
         iniparser_add_node(iniparser,-1,INIPARSER_TYPE_EMPTY,"","");
      }
   }
   fclose(file);
   return 0;
}

export int iniparser_store(iniparser_t *iniparser, char *filename){
   FILE *file = fopen(filename, "w");
   if(file==NULL){
      return -1;
   }
   iniparsenode_t *item;
   int size = list_size(&iniparser->list);
   int i;
   for(i=0;i<size;i++){
      item = (iniparsenode_t*)list_get_index(&iniparser->list,i,NULL,0);
      if(item!=NULL){
         if(item->type==INIPARSER_TYPE_EMPTY){
            fprintf(file,"\n");
         }else if(item->type==INIPARSER_TYPE_COMMENT){
            fprintf(file,";%.100s\n",item->value);
         }else if(item->type==INIPARSER_TYPE_SECTION){
            fprintf(file,"[%.50s]\n",item->key);
         }else if(item->type==INIPARSER_TYPE_KEYVALUE){
            fprintf(file,"%.50s=%.100s\n",item->key,item->value);
         }
      }
   }
   fclose(file);
   return 0;
}

export char *iniparser_getstring(iniparser_t *iniparser, char *section, char *key, char *def){
   iniparsenode_t *result = iniparser_get_node(iniparser,section,key);
   if(result==NULL){
      return def;
   }else{
      return result->value;
   }
}

export int iniparser_setstring(iniparser_t *iniparser, char *section, char *key, char *value){
   iniparsenode_t *result = iniparser_get_node(iniparser,section,key);
   if(result==NULL){
      iniparsenode_t nodekeyvalue;
      memset(&nodekeyvalue,0,sizeof(iniparsenode_t));
      nodekeyvalue.type = INIPARSER_TYPE_KEYVALUE;
      strncpy0(nodekeyvalue.key,key,INIPARSER_MAX_KEY);
      strncpy0(nodekeyvalue.value,value,INIPARSER_MAX_VALUE);
      int index = iniparser_get_index(iniparser,INIPARSER_TYPE_SECTION,section);
      if(index!=-1){
         return iniparser_add_node(iniparser, index+1, INIPARSER_TYPE_KEYVALUE, key, value);
      }else{
         if(iniparser_add_node(iniparser, -1, INIPARSER_TYPE_SECTION, section, "")==-1){
            return -1;
         }
         if(iniparser_add_node(iniparser, -1, INIPARSER_TYPE_KEYVALUE, key, value)==-1){
            list_remove_index(&iniparser->list,list_size(&iniparser->list)-1,NULL,0);
            return -1;
         }
      }
   }else{
      strncpy0(result->value,value,INIPARSER_MAX_VALUE);
   }
   return 0;
}

export int iniparser_getint(iniparser_t *iniparser, char *section, char *key, int def){
   char *retval = iniparser_getstring(iniparser, section, key, NULL);
   if(retval==NULL){
      return def;
   }
   int result;
   sscanf(retval,"%d",&result);
   return result;
}

export int iniparser_setint(iniparser_t *iniparser, char *section, char *key, int val){
   char value[INIPARSER_MAX_VALUE];
   sprintf(value,"%d",val);
   return iniparser_setstring(iniparser,section,key,value);
}

export double iniparser_getdouble(iniparser_t *iniparser, char *section, char *key, double def){
   char *retval = iniparser_getstring(iniparser, section, key, NULL);
   if(retval==NULL){
      return def;
   }
   double result;
   sscanf(retval,"%f",(float*)&result);
   return result;
}

export int iniparser_setdouble(iniparser_t *iniparser, char *section, char *key, double val){
   char value[INIPARSER_MAX_VALUE];
   sprintf(value,"%f",val);
   return iniparser_setstring(iniparser,section,key,value);
}
