/*
* radio_manager.h
*
* This file contains an implementation of a radio manager to use on irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Jos� Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

int checkbox_create(wchar_t *text, HWND hWnd){
   if(manager.connect_size>=IRC_PROFILE_LIMIT){
      return -1;
   }
   wchar_t wprofile_name_cmp[IRC_SIZE_SMALL];
   int i;
   for(i=0;i<manager.connect_size;i++){
      Button_GetText(manager.connect_handles[i],wprofile_name_cmp,IRC_SIZE_SMALL);
      if(_wcsicmp(wprofile_name_cmp,text)==0){
         return -2;
      }
   }
   manager.connect_handles[manager.connect_size]=CreateWindowEx(0,L"BUTTON",text,BS_LEFTTEXT|BS_AUTOCHECKBOX|WS_CHILD|WS_VISIBLE,RADIO_LEFT,RADIO_TOP_DISTANCE+(RADIO_TOP*manager.connect_size),RADIO_WIDTH,RADIO_HEIGHT,hWnd,(HMENU)NULL,app_instance,NULL);
   Button_SetCheck(manager.connect_handles[manager.connect_size],BST_CHECKED);
   manager.connect_size++;
   return 0;
}

void checkbox_getselected(int *d_result, int *s_result){
   *s_result = 0;
   int i;
   for(i=0;i<manager.connect_size;i++){
      if(Button_GetCheck(manager.connect_handles[i])==BST_CHECKED){
         d_result[*s_result] = i;
         (*s_result)++;
      }
   }
}

int checkbox_delete(int index){
   if(index<0 || index>=IRC_PROFILE_LIMIT){
      return -1;
   }
   wchar_t wprofile_name[IRC_SIZE_SMALL];
   wchar_t wprofile_fullpath[IRC_SIZE_SMALL];
   Button_GetText(manager.connect_handles[index],wprofile_name,IRC_SIZE_SMALL);
   wfile_to_fullpath(wprofile_name,wprofile_fullpath);
   DeleteFile(wprofile_fullpath);
   DestroyWindow(manager.connect_handles[index]);
   manager.connect_size--;
   int i;
   for(i=index;i<manager.connect_size;i++){
      manager.connect_handles[i]=manager.connect_handles[i+1];
      MoveWindow(manager.connect_handles[i],0,5+(i*20),150,20,FALSE);
   }
   return 0;
}

void checkbox_resize_all(){
   int i;
   for(i=0;i<manager.connect_size;i++){
      MoveWindow(manager.connect_handles[i],RADIO_LEFT,RADIO_TOP_DISTANCE+(RADIO_TOP*i),RADIO_WIDTH,RADIO_HEIGHT,TRUE);
   }
}
