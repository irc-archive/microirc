/*
* tab_manager_proc.h
*
* This file contains an implementation of a tab manager to use on irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

typedef enum {STATUS,CHAT}TAB_TYPE;
typedef enum {HIDE,SHOW}TAB_VISIBLE_ACTION;
typedef enum {APPEND,REMOVE}TAB_TEXT_ACTION;
typedef enum {TSTRUE,TSFALSE}TAB_TIMESTAMP;

typedef struct tab_t{
   HWND text;
   HWND nick;
}tab_t;

typedef struct style_text_t{
   int style;
   int color;
}style_text_t;

void set_style(style_text_t *style, CHARFORMAT2 *format){
   if(style==NULL){
      format->dwMask = CFM_COLOR;
      format->crTextColor = client.config.font_color;
   }else{
      format->dwMask = CFM_COLOR;
      format->crTextColor = style->color;
   }
}

int tab_get_parameters_index(HWND tab_control, int tab_index, tab_t **result){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(tab_item));
   tab_item.mask = TCIF_PARAM;
   if(SendMessage(tab_control,TCM_GETITEM,(int)tab_index,(LPARAM)&tab_item)==FALSE){
      return -1;
   }
   *result = (tab_t*)tab_item.lParam;
   return 0;
}

int tab_get_parameters_current(HWND tab_control, tab_t **result){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return tab_get_parameters_index(tab_control,tab_index,result);
}

int tab_get_name_index(HWND tab_control, int tab_index, wchar_t *d_result, int s_result){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(tab_item));
   tab_item.mask = TCIF_TEXT;
   tab_item.pszText=d_result;
   tab_item.cchTextMax=s_result;
   if(SendMessage(tab_control,TCM_GETITEM,(int)tab_index,(LPARAM)&tab_item)==FALSE){
      return -1;
   }
   return 0;
}

int tab_get_name_current(HWND tab_control, wchar_t *d_result, int s_result){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return tab_get_name_index(tab_control,tab_index,d_result,s_result);
}

int tab_insert_index(HWND tab_control, int tab_index, wchar_t *tab_name, tab_t *tab_parameters){
   TCITEM tab_item;
   memset(&tab_item,0,sizeof(tab_item));
   tab_item.mask = TCIF_TEXT | TCIF_PARAM;
   tab_item.pszText = tab_name;
   tab_item.lParam=(long)tab_parameters;
   return SendMessage(tab_control,TCM_INSERTITEM,(int)tab_index,(LPARAM)&tab_item);
}

int tab_find(HWND tab_control, wchar_t *target){
   wchar_t buffer[IRC_SIZE_SMALL];
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_name_index(tab_control,size,buffer,IRC_SIZE_SMALL)==0){
         if(wcsicmp(buffer,target)==0){
            return size;
         }
      }
   }
   return -1;
}

void tab_refresh(HWND tab_control, TAB_VISIBLE_ACTION hide){
   tab_t *refresh_tab;
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return;
   }
   if(tab_get_parameters_index(tab_control,tab_index,&refresh_tab)!=0){
      return;
   }
   if(hide==SHOW){
      SendMessage(tab_control,TCM_HIGHLIGHTITEM,SendMessage(tab_control,TCM_GETCURSEL,0,0),LOWORD(FALSE));
      ShowWindow(refresh_tab->text,SW_SHOW);
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
      new_tab->text=CreateWindowEx(WS_EX_STATICEDGE,L"RICHEDIT50W",NULL,WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_READONLY|ES_NOHIDESEL,resize.TABTALK_LEFT,resize.TABTALK_TOP,resize.TABTALK_STATUS_WIDTH,resize.TABALL_HEIGHT,hWnd,(HMENU)EDIT_CHATVIEW_TEXT,config.h_instance,NULL);
      if(new_tab->text==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=NULL;
   }else if(type==CHAT){
      new_tab->text=CreateWindowEx(WS_EX_STATICEDGE,L"RICHEDIT50W",NULL,WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_READONLY|ES_NOHIDESEL,resize.TABTALK_LEFT,resize.TABTALK_TOP,resize.TABTALK_CHAT_WIDTH,resize.TABALL_HEIGHT,hWnd,(HMENU)EDIT_CHATVIEW_TEXT,config.h_instance,NULL);
      if(new_tab->text==NULL){
         free(new_tab);
         return -1;
      }
      new_tab->nick=CreateWindowEx(0,L"LISTBOX",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_SORT|LBS_HASSTRINGS|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY,resize.TABNICK_LEFT,resize.TABNICK_TOP,resize.TABNICK_CHAT_WIDTH,resize.TABALL_HEIGHT,hWnd,(HMENU)LIST_CHATVIEW_NICK,config.h_instance,NULL);
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
   PostMessage(new_tab->text,EM_SHOWSCROLLBAR,SB_VERT,TRUE);
   SendMessage(new_tab->text,EM_AUTOURLDETECT,TRUE,0);
   SendMessage(new_tab->text,EM_SETEVENTMASK,0,ENM_LINK);
   SendMessage(new_tab->text,EM_EXLIMITTEXT,0,EDITCHATVIEWTEXT_LIMIT);
   SendMessage(new_tab->text,EM_SETBKGNDCOLOR,0,client.config.background_color);
   tab_index = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   if(tab_insert_index(tab_control,tab_index,tab_name,new_tab)==-1){
      DestroyWindow(new_tab->text);
      if(new_tab->nick!=NULL){
         DestroyWindow(new_tab->nick);
      }
      free(new_tab);
      return -1;
   }
   ShowWindow(new_tab->text,SW_HIDE);
   if(new_tab->nick!=NULL){
      SendMessage(new_tab->nick,LB_SETHORIZONTALEXTENT,SCALEX(150),0);
      ShowWindow(new_tab->nick,SW_HIDE);
   }
   tab_refresh(tab_control,SHOW);
   return 0;
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

int write_nick_index(HWND tab_control, int tab_index, wchar_t *nick, TAB_TEXT_ACTION operation){
   tab_t *write_tab;
   if(tab_get_parameters_index(tab_control, tab_index, &write_tab)!=0){
      return -1;
   }
   if(write_tab->nick==NULL){
      return -1;
   }
   if(operation==APPEND){
      if(nick!=NULL){
         SendMessage(write_tab->nick,LB_ADDSTRING,0,(LPARAM)nick);
      }
   }else if(operation==REMOVE){
      if(nick==NULL){
         SendMessage(write_tab->nick,LB_RESETCONTENT,0,0);
      }else{
         int listsize = SendMessage(write_tab->nick,LB_GETCOUNT,0,0);
         wchar_t wcurrent[IRC_SIZE_SMALL];
         char firstnick[IRC_SIZE_SMALL];
         char secondnick[IRC_SIZE_SMALL];
         char *firstptr;
         char *secondptr;
         int i;
         WideCharToMultiByte(client.config.encoding,0,nick,-1,secondnick,IRC_SIZE_SMALL,NULL,NULL);
         secondptr = irc_get_nick(&client.irc,secondnick);
         if(secondptr!=NULL){
            for(i=0;i<listsize;i++){
               SendMessage(write_tab->nick,LB_GETTEXT,i,(LPARAM)wcurrent);
               WideCharToMultiByte(client.config.encoding,0,wcurrent,-1,firstnick,IRC_SIZE_SMALL,NULL,NULL);
               firstptr = irc_get_nick(&client.irc,firstnick);
               if(firstptr!=NULL){
                  if(strcmp(firstptr,secondptr)==0){
                     SendMessage(write_tab->nick,LB_DELETESTRING,i,0);
                     return 0;
                  }
               }
            }
         }
         return -1;
      }
   }
   return 0;
}

int write_nick_name(HWND tab_control, wchar_t *tab_name, wchar_t *nick, TAB_TEXT_ACTION operation){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index==-1){
      return -1;
   }
   return write_nick_index(tab_control,tab_index,nick,operation);
}

int write_nick_current(HWND tab_control, wchar_t *nick, TAB_TEXT_ACTION operation){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return write_nick_index(tab_control,tab_index,nick,operation);
}

int write_text_index(HWND tab_control, int tab_index, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   tab_t *write_tab;
   if(tab_get_parameters_index(tab_control, tab_index, &write_tab)!=0){
      return -1;
   }
   if(write_tab->text==NULL){
      return -1;
   }
   if(SendMessage(write_tab->text, WM_GETTEXTLENGTH, 0, 0)>(EDITCHATVIEWTEXT_LIMIT-IRC_SIZE_MEDIUM)){
      SendMessage(write_tab->text, EM_SETSEL, 0, (EDITCHATVIEWTEXT_LIMIT/4));
      SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)L"");
   }
   //|CFM_FACE; wcscpy(format.szFaceName, L"Courier new");
   CHARFORMAT2 format;
   memset(&format,0,sizeof(format));
   format.cbSize = sizeof(format);
   set_style(style,&format);
   SendMessage(write_tab->text, EM_SETSEL, EDITCHATVIEWTEXT_LIMIT, EDITCHATVIEWTEXT_LIMIT);
   SendMessage(write_tab->text, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format);
   if(ts == TSTRUE){
      SYSTEMTIME timestamp;
      wchar_t wtimestamp[IRC_SIZE_SMALL];
      GetSystemTime(&timestamp);
      swprintf(wtimestamp,L"\r\n[%02d:%02d:%02d] ",(unsigned short)timestamp.wHour,(unsigned short)timestamp.wMinute,(unsigned short)timestamp.wSecond);
      SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)wtimestamp);
   }else if(ts == TSFALSE){
      SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
   }
   memset(&format,0,sizeof(format));
   format.cbSize = sizeof(format);
   set_style(style,&format);

   SendMessage(write_tab->text, EM_SETSEL, EDITCHATVIEWTEXT_LIMIT, EDITCHATVIEWTEXT_LIMIT);
   SendMessage(write_tab->text, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format);

   SendMessage(write_tab->text, EM_REPLACESEL, 0, (LPARAM)text);
   SendMessage(write_tab->text, EM_SCROLLCARET, 0, 0);

   if(tab_index != SendMessage(tab_control,TCM_GETCURSEL,0,0)){
      SendMessage(tab_control,TCM_HIGHLIGHTITEM,tab_index,LOWORD(TRUE));
   }
   return 0;
}

int write_text_name(HWND tab_control, wchar_t *tab_name, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int tab_index = tab_find(tab_control,tab_name);
   if(tab_index==-1){
      return -1;
   }
   return write_text_index(tab_control,tab_index,text,style,ts);
}

int write_text_current(HWND tab_control, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int tab_index = SendMessage(tab_control,TCM_GETCURSEL,0,0);
   if(tab_index==-1){
      return -1;
   }
   return write_text_index(tab_control,tab_index,text,style,ts);
}

int write_text_all(HWND tab_control, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      write_text_index(tab_control,size,text,style,ts);
   }
   return 0;
}

int tab_change_nick(HWND tab_control, wchar_t *oldnick, wchar_t *newnick, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(write_nick_index(tab_control,size,oldnick,REMOVE)!=-1){
         write_text_index(tab_control,size,text,style,ts);
         write_nick_index(tab_control,size,newnick,APPEND);
      }
   }
   return -1;
}

int tab_quit(HWND tab_control, wchar_t *nick, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(write_nick_index(tab_control,size,nick,REMOVE)!=-1){
         write_text_index(tab_control,size,text,style,ts);
      }
   }
   return -1;
}

int tab_disconnect(HWND tab_control, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      write_text_index(tab_control,size,text,style,ts);
      write_nick_index(tab_control,size,NULL,REMOVE);
   }
   return -1;
}

int tab_connect(HWND tab_control, wchar_t *text, style_text_t *style, TAB_TIMESTAMP ts){
   return write_text_all(tab_control,text,style,ts);
}

void tab_resize_all(HWND tab_control){
   tab_t *tab;
   int size = SendMessage(tab_control,TCM_GETITEMCOUNT,0,0);
   for(size--;size>=0;size--){
      if(tab_get_parameters_index(tab_control,size,&tab)!=-1){
         if(tab->nick==NULL){
            MoveWindow(tab->text,resize.TABTALK_LEFT,resize.TABTALK_TOP,resize.TABTALK_STATUS_WIDTH,resize.TABALL_HEIGHT,FALSE);
         }else{
            MoveWindow(tab->text,resize.TABTALK_LEFT,resize.TABTALK_TOP,resize.TABTALK_CHAT_WIDTH,resize.TABALL_HEIGHT,FALSE);
            MoveWindow(tab->nick,resize.TABNICK_LEFT,resize.TABNICK_TOP,resize.TABNICK_CHAT_WIDTH,resize.TABALL_HEIGHT,FALSE);
         }
         SendMessage(tab->text,WM_VSCROLL,SB_LINEDOWN,0);
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
            int i;
            int j;
            for(i=0;i<listsize;i++){
               SendMessage(tab->nick,LB_GETTEXT,i,(LPARAM)wcurrent);
               WideCharToMultiByte(client.config.encoding,0,wcurrent,-1,current,IRC_SIZE_SMALL,NULL,NULL);
               firstptr = irc_get_nick(&client.irc,current);
               if(firstptr!=NULL){
                  for(j=0;j<s_nicklist;j++){
                     secondptr = irc_get_nick(&client.irc,d_nicklist[j]);
                     if(secondptr!=NULL){
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
               }
            }
            for(i=0;i<updatesize;i++){
               MultiByteToWideChar(client.config.encoding,0,d_nicklist[updatelist[i]],-1,wcurrent,IRC_SIZE_MEDIUM);
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
                  MultiByteToWideChar(client.config.encoding,0,d_nicklist[i],-1,wcurrent,IRC_SIZE_MEDIUM);
                  SendMessage(tab->nick,LB_ADDSTRING,0,(LPARAM)wcurrent);
               }
            }
         }
      }
   }
}
