/*
* tab_manager.h
*
* This file contains an implementation of a tab manager to use on irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#define TAB_LITTLE_BUFFER 256

enum TAB_TYPE{STATUS,CHAT};
enum TAB_VISIBLE_ACTION{HIDE,SHOW};
enum TAB_TEXT_TYPE{TEXT,NICK};
enum TAB_TEXT_ACTION{APPEND,REMOVE};

typedef struct tab_t{
   HWND talk;
   HWND nick;
}tab_t;

int tab_get_parameters_index(HWND tab_control, int tab_index, tab_t **result){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(TCITEM));
   tab_item.mask = TCIF_PARAM;
   if(SendMessage(tab_control,TCM_GETITEM,(int)tab_index,(LPARAM)&tab_item)==FALSE){
      return -1;
   }
   *result = (tab_t*)tab_item.lParam;
   return 0;
}

int tab_get_parameters_actual(HWND tab_control, tab_t **result){
   return tab_get_parameters_index(tab_control,SendMessage(tab_control,TCM_GETCURSEL,0,0),result);
}

int tab_get_name_index(HWND tab_control, int tab_index, wchar_t *d_result, int s_result){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(TCITEM));
   tab_item.mask = TCIF_TEXT;
   tab_item.pszText=d_result;
   tab_item.cchTextMax=s_result;
   if(SendMessage(tab_control,TCM_GETITEM,(int)tab_index,(LPARAM)&tab_item)==FALSE){
      return -1;
   }
   return 0;
}

int tab_get_name_actual(HWND tab_control, wchar_t *d_result, int s_result){
   return tab_get_name_index(tab_control,SendMessage(tab_control,TCM_GETCURSEL,0,0),d_result,s_result);
}

int tab_insert_both_index(HWND tab_control, int tab_index, wchar_t *tab_name, tab_t *tab_parameters){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(TCITEM));
   tab_item.mask = TCIF_TEXT | TCIF_PARAM;
   tab_item.pszText = tab_name;
   tab_item.lParam=(long)tab_parameters;
   return SendMessage(tab_control,TCM_INSERTITEM,(int)tab_index,(LPARAM)&tab_item);
}

int tab_find(HWND tab_control, wchar_t *target){
   wchar_t buffer[TAB_LITTLE_BUFFER];
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_name_index(tab_control,size,buffer,TAB_LITTLE_BUFFER)==0){
         if(wcsicmp(buffer,target)==0){
            return size;
         }
      }
   }
   return -1;
}

void tab_refresh(HWND tab_control, TAB_VISIBLE_ACTION hide){
   tab_t *refresh_tab;
   if(tab_get_parameters_actual(tab_control,&refresh_tab)!=0){
      return;
   }
   if(hide==SHOW){
      ShowWindow(refresh_tab->talk,SW_SHOW);
      SendMessage(refresh_tab->talk, EM_SCROLLCARET, 0, 0);
      if(refresh_tab->nick!=NULL){
         ShowWindow(refresh_tab->nick,SW_SHOW);
      }
   }else if(hide==HIDE){
      ShowWindow(refresh_tab->talk,SW_HIDE);
      if(refresh_tab->nick!=NULL){
         ShowWindow(refresh_tab->nick,SW_HIDE);
      }
   }
}

WNDPROC old_TalkBoxProc;
HWND hWnd_TalkBox;
LRESULT CALLBACK TalkBoxProc(HWND hWnd, UINT event_id, WPARAM element_id, LPARAM param_id){
   switch (event_id){
      case WM_LBUTTONUP:{
         SetFocus(hWnd_EditChat);
         break;
      }
   }
   return CallWindowProc(old_TalkBoxProc, hWnd, event_id, element_id, param_id);
}

WNDPROC old_NickBoxProc;
HWND hWnd_NickBox;
LRESULT CALLBACK NickBoxProc(HWND hWnd, UINT event_id, WPARAM element_id, LPARAM param_id){
   switch(event_id){
      case WM_COMMAND:{
         int wmEvent = HIWORD(element_id);
         switch (LOWORD(element_id)){
            case IDM_CHATBOX_KICK:{
               wchar_t wnick[IRC_SIZE_MEDIUM];
               wchar_t wchannel[IRC_SIZE_MEDIUM];
               char nick[IRC_SIZE_MEDIUM];
               char channel[IRC_SIZE_MEDIUM];
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)!=0){
                  tab_get_name_actual(hWnd_TabControlChat,wchannel,IRC_SIZE_MEDIUM);
                  WideCharToMultiByte(CP_UTF8,0,wchannel,-1,channel,IRC_SIZE_MEDIUM,NULL,NULL);
                  WideCharToMultiByte(CP_UTF8,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
                  char *send[3] = {channel,nick,"no reason"};
                  irc_send_message(&irc,SEND_KICK,send,3);
               }
               break;
            }
            case IDM_CHATBOX_KICKBAN:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            case IDM_CHATBOX_BAN:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            case IDM_CHATBOX_WHOIS:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            case IDM_CHATBOX_COPYNICK:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
         }
         break;
      }
      case WM_LBUTTONDOWN:{
         SHRGINFO shrg;
         shrg.cbSize = sizeof(shrg);
         shrg.hwndClient = hWnd;
         shrg.ptDown.x = LOWORD(param_id);
         shrg.ptDown.y = HIWORD(param_id);
         shrg.dwFlags = SHRG_RETURNCMD;
         if(SHRecognizeGesture(&shrg) == GN_CONTEXTMENU){
            HMENU menu = LoadMenu(hInstance_Main, MAKEINTRESOURCE(IDR_CHATBOX_MENU));
            menu = GetSubMenu(menu, 0);
            TrackPopupMenuEx(menu,TPM_LEFTALIGN,TABNICK_LEFT*wWidth+LOWORD(param_id),TABNICK_TOP*wHeight+HIWORD(param_id),hWnd,NULL);
         }
         break;
      }
   }
   return CallWindowProc(old_NickBoxProc, hWnd, event_id, element_id, param_id);
}

int tab_create(HWND hWnd, HWND tab_control, wchar_t *tab_name, TAB_TYPE type){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index!=-1){
      return -1;
   }
   tab_t *new_tab=(tab_t*)malloc(sizeof(tab_t));
   if(new_tab==NULL){
      return -1;
   }
   if(type==STATUS){
      new_tab->talk=CreateWindowEx(0,L"edit",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|ES_MULTILINE,TABTALK_LEFT*wWidth,TABTALK_TOP*wHeight,TABTALK_STATUS_WIDTH*wWidth,TABALL_HEIGHT*wHeight,hWnd,(HMENU)TALK_BOX,hInstance_Main,NULL);
      if(new_tab->talk==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=NULL;
   }else if(type==CHAT){
      new_tab->talk=CreateWindowEx(0,L"edit",NULL,ES_AUTOVSCROLL|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|ES_MULTILINE,TABTALK_LEFT*wWidth,TABTALK_TOP*wHeight,TABTALK_CHAT_WIDTH*wWidth,TABALL_HEIGHT*wHeight,hWnd,(HMENU)TALK_BOX,hInstance_Main,NULL);
      if(new_tab->talk==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=CreateWindowEx(0,L"listbox",NULL,LBS_NOINTEGRALHEIGHT|ES_AUTOVSCROLL|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_SORT|LBS_HASSTRINGS|LBS_NOTIFY,TABNICK_LEFT*wWidth,TABNICK_TOP*wHeight,TABNICK_CHAT_WIDTH*wWidth,TABALL_HEIGHT*wHeight,hWnd,(HMENU)LIST_BOX,hInstance_Main,NULL);
      if(new_tab->nick==NULL){
         DestroyWindow(new_tab->talk);
         free(new_tab);
         return -1;
      }
      hWnd_NickBox = hWnd;
      old_NickBoxProc = (WNDPROC)GetWindowLong(new_tab->nick,GWL_WNDPROC);
      SetWindowLong(new_tab->nick,GWL_WNDPROC,(LONG)NickBoxProc);
   }else{
      return -1;
   }
   hWnd_TalkBox = hWnd;
   old_TalkBoxProc = (WNDPROC)GetWindowLong(new_tab->talk,GWL_WNDPROC);
   SetWindowLong(new_tab->talk,GWL_WNDPROC,(LONG)TalkBoxProc);

   tab_index = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   if(tab_insert_both_index(tab_control,tab_index,tab_name,new_tab)==-1){
      DestroyWindow(new_tab->talk);
      if(new_tab->nick!=NULL){
         DestroyWindow(new_tab->nick);
      }
      free(new_tab);
      return -1;
   }
   SendMessage(new_tab->talk,EM_FMTLINES,FALSE,0);
   SendMessage(new_tab->talk,EM_SETREADONLY,TRUE,0);
   ShowWindow(new_tab->talk,SW_HIDE);
   if(new_tab->nick!=NULL){
      SendMessage(new_tab->nick,LB_SETHORIZONTALEXTENT,150,0);
      ShowWindow(new_tab->nick,SW_HIDE);
   }
   tab_refresh(tab_control,SHOW);
   return 0;
}

int tab_write_index(HWND tab_control, int tab_index, wchar_t *text, TAB_TEXT_TYPE window, TAB_TEXT_ACTION operation){
   tab_t *write_tab;
   if(tab_get_parameters_index(tab_control, tab_index, &write_tab)!=0){
      return -1;
   }
   if(window==TEXT){
      if(operation==APPEND){
         if(SendMessage(write_tab->talk,EM_GETLIMITTEXT,0,0)-Edit_GetTextLength(write_tab->talk)<DELETE_TEXT_MARGIN){
            SendMessage(write_tab->talk, EM_SETSEL, 0, DELETE_TEXT_MARGIN);
            SendMessage(write_tab->talk, EM_REPLACESEL, 0, (LPARAM)"");
         }
         int len = GetWindowTextLength(write_tab->talk);
         SendMessage(write_tab->talk, EM_SETSEL, len, len);
         SendMessage(write_tab->talk, EM_REPLACESEL, 0, (LPARAM)text);
         SendMessage(write_tab->talk, EM_SCROLLCARET, 0, 0);
      }
   }else if(window==NICK){
      if(operation==APPEND){
         if(wcsncmp(L"@",text,1)==0 || wcsncmp(L"+",text,1)==0 || wcsncmp(L"!",text,1)==0 || wcsncmp(L"%",text,1)==0){//MISSING..
            wcscpy(text,text+1);
         }
         SendMessage(write_tab->nick,LB_ADDSTRING,0,(LPARAM)text);
      }else if(operation==REMOVE){
         if(text==NULL){
            SendMessage(write_tab->nick,LB_RESETCONTENT,0,0);
         }else{
            int remove = SendMessage(write_tab->nick,LB_FINDSTRINGEXACT,-1,(LPARAM)text);//LB_FINDSTRING
            if(remove!=LB_ERR){
               SendMessage(write_tab->nick,LB_DELETESTRING,remove,0);
            }else{
               return -1;
            }
         }
      }
   }
   return 0;
}

int tab_write_name(HWND tab_control, wchar_t *tab_name, wchar_t *text, TAB_TEXT_TYPE window, TAB_TEXT_ACTION operation){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index==-1){
      return -1;
   }
   return tab_write_index(tab_control,tab_index,text,window,operation);
}

int tab_write_actual(HWND tab_control, wchar_t *text, TAB_TEXT_TYPE window, TAB_TEXT_ACTION operation){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return tab_write_index(tab_control,tab_index,text,window,operation);
}

int tab_delete_index(HWND tab_control, int tab_index){
   tab_t *delete_tab;
   if(tab_get_parameters_index(tab_control, tab_index, &delete_tab)!=0){
      return -1;
   }
   DestroyWindow(delete_tab->talk);
   if(delete_tab->nick!=NULL){
      DestroyWindow(delete_tab->nick);
   }
   free(delete_tab);
   SendMessage(tab_control,TCM_DELETEITEM,(int)tab_index,0);
   SendMessage(tab_control,TCM_SETCURSEL,(int)tab_index-1,0);
   tab_refresh(tab_control,SHOW);
   return 0;
}

int tab_delete_name(HWND tab_control, wchar_t *tab_name){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index==-1){
      return -1;
   }
   return tab_delete_index(tab_control,tab_index);
}

int tab_delete_actual(HWND tab_control){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return tab_delete_index(tab_control,tab_index);
}

int tab_select_index(HWND tab_control, int tab_index){
   tab_refresh(tab_control,HIDE);
   int index = SendMessage(tab_control,TCM_SETCURSEL,tab_index,0);
   if(index==-1){
      return -1;
   }
   tab_refresh(tab_control,SHOW);
   return index;
}

int tab_select_name(HWND tab_control, wchar_t *tab_name){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index==-1){
      return -1;
   }
   return tab_select_index(tab_control,tab_index);
}

int tab_nickchange(HWND tab_control, wchar_t *timestamp, wchar_t *oldnick, wchar_t *newnick){
   tab_t *tab;
   wchar_t buffer[IRC_SIZE_MEDIUM];
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_parameters_index(tab_control,size,&tab)!=-1){
         if(tab->nick!=NULL){
            if(tab_write_index(tab_control,size,oldnick,NICK,REMOVE)!=-1){
               swprintf(buffer,L"\r\n%s nickchange %s -> %s",timestamp,oldnick,newnick);
               tab_write_index(tab_control,size,buffer,TEXT,APPEND);
               tab_write_index(tab_control,size,newnick,NICK,APPEND);
            }
         }
      }
   }
   return -1;
}

int tab_quit(HWND tab_control, wchar_t *timestamp, wchar_t *nick, wchar_t *mensagem){
   tab_t *tab;
   wchar_t buffer[IRC_SIZE_MEDIUM];
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_parameters_index(tab_control,size,&tab)!=-1){
         if(tab->nick!=NULL){
            if(tab_write_index(tab_control,size,nick,NICK,REMOVE)!=-1){
               swprintf(buffer,L"\r\n%s %s quit (%s)",timestamp,nick,mensagem);
               tab_write_index(tab_control,size,buffer,TEXT,APPEND);
            }
         }
      }
   }
   return -1;
}

int tab_disconnect(HWND tab_control){
   tab_t *tab;
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_parameters_index(tab_control,size,&tab)!=-1){
         tab_write_index(tab_control,size,L"\r\nDISCONNECTED",TEXT,APPEND);
         if(tab->nick!=NULL){
            tab_write_index(hWnd_TabControlChat,size,NULL,NICK,REMOVE);
         }
      }
   }
   return -1;
}

int tab_connect(HWND tab_control){
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      tab_write_index(tab_control,size,L"\r\nCONNECTED",TEXT,APPEND);
   }
   return -1;
}

void tab_resize_all(HWND tab_control){
   tab_t *tab;
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_parameters_index(tab_control,size,&tab)!=-1){
         if(tab->nick==NULL){
            MoveWindow(tab->talk,TABTALK_LEFT*wWidth,TABTALK_TOP*wHeight,TABTALK_STATUS_WIDTH*wWidth,TABALL_HEIGHT*wHeight,TRUE);
         }else{
            MoveWindow(tab->talk,TABTALK_LEFT*wWidth,TABTALK_TOP*wHeight,TABTALK_CHAT_WIDTH*wWidth,TABALL_HEIGHT*wHeight,TRUE);
            MoveWindow(tab->nick,TABNICK_LEFT*wWidth,TABNICK_TOP*wHeight,TABNICK_CHAT_WIDTH*wWidth,TABALL_HEIGHT*wHeight,TRUE);
         }
      }
   }
}
