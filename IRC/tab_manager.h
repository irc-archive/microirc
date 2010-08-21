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
   HWND text;
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

int tab_get_parameters_current(HWND tab_control, tab_t **result){
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

int tab_get_name_current(HWND tab_control, wchar_t *d_result, int s_result){
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
   if(tab_get_parameters_current(tab_control,&refresh_tab)!=0){
      return;
   }
   if(hide==SHOW){
      ShowWindow(refresh_tab->text,SW_SHOW);
      SendMessage(refresh_tab->text, EM_SCROLLCARET, 0, 0);
      if(refresh_tab->nick!=NULL){
         ShowWindow(refresh_tab->nick,SW_SHOW);
      }
   }else if(hide==HIDE){
      ShowWindow(refresh_tab->text,SW_HIDE);
      if(refresh_tab->nick!=NULL){
         ShowWindow(refresh_tab->nick,SW_HIDE);
      }
   }
}

WNDPROC old_ChatViewTextProc;
LRESULT CALLBACK ChatViewTextProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_LBUTTONUP:{
         SetFocus(edit_chatinput_handle);
         break;
      }
   }
   return CallWindowProc(old_ChatViewTextProc, hWnd, uMsg, wParam, lParam);
}

WNDPROC old_ChatViewNickProc;
LRESULT CALLBACK ChatViewNickProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_COMMAND:{
         int wmEvent = HIWORD(wParam);
         switch (LOWORD(wParam)){
            case IDM_CHATBOX_KICK:{
               wchar_t wnick[IRC_SIZE_SMALL];
               wchar_t wchannel[IRC_SIZE_SMALL];
               char nick[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
               char *nick_ptr = irc_get_nick(&irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(tabcontrol_chatview_handle,wchannel,IRC_SIZE_MEDIUM);
               WideCharToMultiByte(config.encoding,0,wchannel,-1,channel,IRC_SIZE_MEDIUM,NULL,NULL);
               char *send[3] = {channel,nick_ptr,config.kick};
               irc_send_message(&irc,SEND_KICK,send,3);
               break;
            }
            case IDM_CHATBOX_KICKBAN:{
               wchar_t wnick[IRC_SIZE_SMALL];
               wchar_t wchannel[IRC_SIZE_SMALL];
               char nick[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
               char *nick_ptr = irc_get_nick(&irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(tabcontrol_chatview_handle,wchannel,IRC_SIZE_MEDIUM);
               WideCharToMultiByte(config.encoding,0,wchannel,-1,channel,IRC_SIZE_MEDIUM,NULL,NULL);
               char *send[6] = {channel,nick_ptr,config.kick,channel,"+b",nick_ptr};
               irc_send_message(&irc,SEND_KICK,send,3);
               irc_send_message(&irc,SEND_CHANNEL_MODE,&send[3],3);
               break;
            }
            case IDM_CHATBOX_BAN:{
               wchar_t wnick[IRC_SIZE_SMALL];
               wchar_t wchannel[IRC_SIZE_SMALL];
               char nick[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
               char *nick_ptr = irc_get_nick(&irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(tabcontrol_chatview_handle,wchannel,IRC_SIZE_MEDIUM);
               WideCharToMultiByte(config.encoding,0,wchannel,-1,channel,IRC_SIZE_MEDIUM,NULL,NULL);
               char *send[3] = {channel,"+b",nick_ptr};
               irc_send_message(&irc,SEND_CHANNEL_MODE,send,3);
               break;
            }
            case IDM_CHATBOX_WHOIS:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            case IDM_CHATBOX_COPYNICK:{
               wchar_t wnick[IRC_SIZE_SMALL];
               char nick[IRC_SIZE_SMALL];
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
               char *nick_ptr = irc_get_nick(&irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,nick_ptr,-1,wnick,IRC_SIZE_SMALL);
               wchar_t wtext[IRC_SIZE_MEDIUM];
               Edit_GetText(edit_chatinput_handle,wtext,IRC_SIZE_MEDIUM);
               wcscat(wtext,wnick);
               wcscat(wtext,L" ");
               Edit_SetText(edit_chatinput_handle,wtext);
               element = Edit_GetTextLength(edit_chatinput_handle);
               SendMessage(edit_chatinput_handle, EM_SETSEL, element, element);
               SetFocus(edit_chatinput_handle);
               break;
            }
         }
         break;
      }
      case WM_LBUTTONDOWN:{
         SHRGINFO shrg;
         shrg.cbSize = sizeof(shrg);
         shrg.hwndClient = hWnd;
         shrg.ptDown.x = LOWORD(lParam);
         shrg.ptDown.y = HIWORD(lParam);
         shrg.dwFlags = SHRG_RETURNCMD;
         if(SHRecognizeGesture(&shrg) == GN_CONTEXTMENU){
            HMENU menu = LoadMenu(app_instance, MAKEINTRESOURCE(IDR_CHATBOX_MENU));
            menu = GetSubMenu(menu, 0);
            TrackPopupMenuEx(menu,TPM_LEFTALIGN,TABNICK_LEFT*window_width+LOWORD(lParam),TABNICK_TOP*window_height+HIWORD(lParam),hWnd,NULL);
         }
         break;
      }
   }
   return CallWindowProc(old_ChatViewNickProc, hWnd, uMsg, wParam, lParam);
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
      new_tab->text=CreateWindowEx(0,L"edit",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|ES_MULTILINE,TABTALK_LEFT*window_width,TABTALK_TOP*window_height,TABTALK_STATUS_WIDTH*window_width,TABALL_HEIGHT*window_height,hWnd,(HMENU)EDIT_CHATVIEW_TEXT,app_instance,NULL);
      if(new_tab->text==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=NULL;
   }else if(type==CHAT){
      new_tab->text=CreateWindowEx(0,L"edit",NULL,ES_AUTOVSCROLL|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|ES_MULTILINE,TABTALK_LEFT*window_width,TABTALK_TOP*window_height,TABTALK_CHAT_WIDTH*window_width,TABALL_HEIGHT*window_height,hWnd,(HMENU)EDIT_CHATVIEW_TEXT,app_instance,NULL);
      if(new_tab->text==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=CreateWindowEx(0,L"listbox",NULL,LBS_NOINTEGRALHEIGHT|ES_AUTOVSCROLL|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_SORT|LBS_HASSTRINGS|LBS_NOTIFY,TABNICK_LEFT*window_width,TABNICK_TOP*window_height,TABNICK_CHAT_WIDTH*window_width,TABALL_HEIGHT*window_height,hWnd,(HMENU)LIST_CHATVIEW_NICK,app_instance,NULL);
      if(new_tab->nick==NULL){
         DestroyWindow(new_tab->text);
         free(new_tab);
         return -1;
      }
      old_ChatViewNickProc = (WNDPROC)GetWindowLong(new_tab->nick,GWL_WNDPROC);
      SetWindowLong(new_tab->nick,GWL_WNDPROC,(LONG)ChatViewNickProc);
   }else{
      return -1;
   }
   old_ChatViewTextProc = (WNDPROC)GetWindowLong(new_tab->text,GWL_WNDPROC);
   SetWindowLong(new_tab->text,GWL_WNDPROC,(LONG)ChatViewTextProc);

   tab_index = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   if(tab_insert_both_index(tab_control,tab_index,tab_name,new_tab)==-1){
      DestroyWindow(new_tab->text);
      if(new_tab->nick!=NULL){
         DestroyWindow(new_tab->nick);
      }
      free(new_tab);
      return -1;
   }
   SendMessage(new_tab->text,EM_FMTLINES,FALSE,0);
   SendMessage(new_tab->text,EM_SETREADONLY,TRUE,0);
   ShowWindow(new_tab->text,SW_HIDE);
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
         if(SendMessage(write_tab->text,EM_GETLIMITTEXT,0,0)-Edit_GetTextLength(write_tab->text)<EDITCHATVIEWTEXT_DELETE){
            SendMessage(write_tab->text, EM_SETSEL, 0, EDITCHATVIEWTEXT_DELETE);
            SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)"");
         }
         int len = Edit_GetTextLength(write_tab->text);
         SendMessage(write_tab->text, EM_SETSEL, len, len);
         SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)text);
         SendMessage(write_tab->text, EM_SCROLLCARET, 0, 0);
      }
   }else if(window==NICK){
      if(operation==APPEND){
         SendMessage(write_tab->nick,LB_ADDSTRING,0,(LPARAM)text);
      }else if(operation==REMOVE){
         if(text==NULL){
            SendMessage(write_tab->nick,LB_RESETCONTENT,0,0);
         }else{
            int listsize = SendMessage(write_tab->nick,LB_GETCOUNT,0,0);
            wchar_t wcurrent[IRC_SIZE_SMALL];
            char firstnick[IRC_SIZE_SMALL];
            char secondnick[IRC_SIZE_SMALL];
            char *firstptr;
            char *secondptr;
            LRESULT result;
            int i;
            WideCharToMultiByte(config.encoding,0,text,-1,secondnick,IRC_SIZE_SMALL,NULL,NULL);
            secondptr = irc_get_nick(&irc,secondnick);
            for(i=0;i<listsize;i++){
               result = SendMessage(write_tab->nick,LB_GETTEXT,i,(LPARAM)wcurrent);
               WideCharToMultiByte(config.encoding,0,wcurrent,-1,firstnick,IRC_SIZE_SMALL,NULL,NULL);
               firstptr = irc_get_nick(&irc,firstnick);
               if(strcmp(firstptr,secondptr)==0){
                  SendMessage(write_tab->nick,LB_DELETESTRING,i,0);
                  return 0;
               }
            }
            return -1;
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

int tab_write_current(HWND tab_control, wchar_t *text, TAB_TEXT_TYPE window, TAB_TEXT_ACTION operation){
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
   DestroyWindow(delete_tab->text);
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

int tab_delete_current(HWND tab_control){
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
            tab_write_index(tabcontrol_chatview_handle,size,NULL,NICK,REMOVE);
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
            MoveWindow(tab->text,TABTALK_LEFT*window_width,TABTALK_TOP*window_height,TABTALK_STATUS_WIDTH*window_width,TABALL_HEIGHT*window_height,TRUE);
         }else{
            MoveWindow(tab->text,TABTALK_LEFT*window_width,TABTALK_TOP*window_height,TABTALK_CHAT_WIDTH*window_width,TABALL_HEIGHT*window_height,TRUE);
            MoveWindow(tab->nick,TABNICK_LEFT*window_width,TABNICK_TOP*window_height,TABNICK_CHAT_WIDTH*window_width,TABALL_HEIGHT*window_height,TRUE);
         }
      }
   }
}

void tab_refresh_nicklist(HWND tab_control, wchar_t *channel, char **d_nicklist, int s_nicklist){
   int index = tab_find(tab_control,channel);
   if(index!=-1){
      tab_t *tab;
      if(tab_get_parameters_index(tab_control,index,&tab)==0){
         if(tab->nick!=NULL){
            int updatelist[IRCPROTOCOL_MAX_NICKS_PER_MESSAGE];
            int updatesize=0;
            int containlist[IRCPROTOCOL_MAX_NICKS_PER_MESSAGE];
            int containsize=0;
            int listsize = SendMessage(tab->nick,LB_GETCOUNT,0,0);
            wchar_t wcurrent[IRC_SIZE_SMALL];
            char current[IRC_SIZE_SMALL];
            char *firstptr;
            char *secondptr;
            LRESULT result;
            int i;
            int j;
            for(i=0;i<listsize;i++){
               result = SendMessage(tab->nick,LB_GETTEXT,i,(LPARAM)wcurrent);
               WideCharToMultiByte(config.encoding,0,wcurrent,-1,current,IRC_SIZE_SMALL,NULL,NULL);
               firstptr = irc_get_nick(&irc,current);
               for(j=0;j<s_nicklist;j++){
                  secondptr = irc_get_nick(&irc,d_nicklist[j]);
                  if(strcmp(firstptr,secondptr)==0){
                     if(strcmp(current,d_nicklist[j])!=0){
                        updatelist[updatesize]=j;
                        updatesize++;
                        SendMessage(tab->nick,LB_DELETESTRING,i,0);
                        listsize--;
                        i--;
                     }
                     containlist[containsize]=j;
                     containsize++;
                     break;
                  }
               }
            }
            for(i=0;i<updatesize;i++){
               MultiByteToWideChar(config.encoding,0,d_nicklist[updatelist[i]],-1,wcurrent,IRC_SIZE_MEDIUM);
               SendMessage(tab->nick,LB_ADDSTRING,0,(LPARAM)wcurrent);
            }
            for(i=0;i<s_nicklist;i++){
               listsize = 0;
               for(j=0;j<containsize;j++){
                  if(containlist[j]==i){
                     listsize = 1;
                     break;
                  }
               }
               if(listsize==0){
                  MultiByteToWideChar(config.encoding,0,d_nicklist[i],-1,wcurrent,IRC_SIZE_MEDIUM);
                  SendMessage(tab->nick,LB_ADDSTRING,0,(LPARAM)wcurrent);
               }
            }
         }
      }
   }
}
