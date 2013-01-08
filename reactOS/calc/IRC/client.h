/*
* client.h
*
* This file contains functions to use by client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void *receiverThreadProc(void *window_handle){
   HWND hWnd = (HWND)window_handle;
   int i;
   int recv_buffer_size;
   char *recv_buffer_ptr[IRCPROTOCOL_RECV_MAX_TOKENS];
   wchar_t recv_buffer[IRCPROTOCOL_RECV_MAX_TOKENS][IRC_SIZE_MEDIUM];
   wchar_t wresult[IRC_SIZE_MEDIUM];
   //style_text_t formatted;
   int recv_result;
   while(receiver_active==1){
      if(WaitForSingleObject(receiver_thread_event,INFINITE)!=WAIT_OBJECT_0){
         recv_result = 0;
      }else{
         recv_result = 1;
      }
      while(recv_result > 0){
         recv_result = irc_recv_message(&irc,recv_buffer_ptr,&recv_buffer_size);
         if(recv_result!=-1){
            for(i=0;i<recv_buffer_size;i++){
               MultiByteToWideChar(config.encoding,0,recv_buffer_ptr[i],-1,recv_buffer[i],IRC_SIZE_MEDIUM);
            }
         }
         switch(recv_result){
            case RECV_CHANNEL_MODE:{//[host OR nick user host] channel modes [arguments OR null]
               if(recv_buffer_size<5){
                  if(recv_buffer_size==3){
                     swprintf(wresult,L"%s sets modes: %s",recv_buffer[0],recv_buffer[2]);
                  }else{
                     swprintf(wresult,L"%s sets modes: %s %s",recv_buffer[0],recv_buffer[2],recv_buffer[3]);
                  }
                  style_text_t style = {0,RGB(0,0,255)};
                  write_text_name(tabcontrol_chatview_handle,recv_buffer[1],wresult,&style,TSTRUE);
               }else if(recv_buffer_size<7){
                  char *modes = recv_buffer_ptr[4];
                  while(*modes!='\0'){
                     if(strchr(irc.prefix_mode,*modes)!=NULL){
                        char *send[1]={recv_buffer_ptr[3]};
                        irc_send_message(&irc,SEND_NAMES,send,1);
                        break;
                     }
                     modes++;
                  }
                  if(recv_buffer_size==5){
                     swprintf(wresult,L"%s sets modes: %s",recv_buffer[0],recv_buffer[4]);
                  }else{
                     swprintf(wresult,L"%s sets modes: %s %s",recv_buffer[0],recv_buffer[4],recv_buffer[5]);
                  }
                  style_text_t style = {0,RGB(0,0,255)};
                  write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               }
               break;
            }
            case RECV_CTCP:{//nick user host destination message
               if(recv_buffer_size<5){
                  break;
               }
               if(memcmp(recv_buffer_ptr[4],"ACTION ",7)!=0){
                  swprintf(wresult,L"%s",recv_buffer[4]);
                  //style_text_t style = {0,RGB(0,0,0)};
                  write_text_name(tabcontrol_chatview_handle,L".status",wresult,NULL,TSTRUE);
                  break;
               }
               recv_buffer_ptr[4]+=7;
               MultiByteToWideChar(config.encoding,0,recv_buffer_ptr[4],-1,recv_buffer[4],IRC_SIZE_MEDIUM);
            }
            case RECV_PRIVMSG:{//nick user host destination message
            }
            case RECV_NOTICE:{//nick user host destination message
               if(recv_buffer_size<5){
                  break;
               }
               if(strstri(recv_buffer_ptr[4],irc.nick)!=NULL){
                  if(config.sounds!=0)
                     PlaySound(sound_alert,NULL,SND_ASYNC|SND_FILENAME);
                  if(config.led_number>=0)
                     //activate_led();
                  if(GetForegroundWindow()!=hWnd){
                     /*SHNOTIFICATIONDATA sn;
                     memset(&sn, 0, sizeof(SHNOTIFICATIONDATA));
                     sn.cbStruct = sizeof(sn);
                     sn.dwID = BUBBLE_NOTIFICATION;
                     sn.npPriority = SHNP_INFORM;
                     sn.csDuration = config.bubble;
                     sn.grfFlags = SHNF_DISPLAYON|SHNF_SILENT;
                     sn.hwndSink = hWnd;
                     sn.pszHTML = L"<html><body>You have received a message on IRC.</body></html>";
                     sn.pszTitle = window_title;
                     sn.rgskn[0].pszTitle = L"Dismiss";
                     sn.rgskn[0].skc.wpCmd = 100; //should be NOTIF_SOFTKEY_FLAGS_DISMISS, but that doesn't work...
                     SHNotificationAdd(&sn);*/
                  }
               }
               style_text_t style = {0,0};
               style_text_t *pstyle = NULL;
               if(recv_result==RECV_CTCP){
                  swprintf(wresult,L"%s %s",recv_buffer[0],recv_buffer[4]);
                  style.color = RGB(192,192,0);
                  pstyle = &style;
               }else if(recv_result==RECV_PRIVMSG){
                  swprintf(wresult,L"%s: %s",recv_buffer[0],recv_buffer[4]);
               }else if(recv_result==RECV_NOTICE){
                  swprintf(wresult,L"%s: %s",recv_buffer[0],recv_buffer[4]);
                  style.color = RGB(0,255,255);
                  pstyle = &style;
               }
               if(irc_validate_channel(&irc,recv_buffer_ptr[3])==0){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)recv_buffer[3]);
                  write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,pstyle,TSTRUE);
               }else{
                  if(recv_result==RECV_NOTICE){
                     write_text_name(tabcontrol_chatview_handle,L".status",wresult,pstyle,TSTRUE);
                  }else{
                     SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)recv_buffer[0]);
                     write_text_name(tabcontrol_chatview_handle,recv_buffer[0],wresult,pstyle,TSTRUE);
                  }
               }
               break;
            }
            case RECV_JOIN:{//nick user host channel
               if(recv_buffer_size<4){
                  break;
               }
               if(strcmp(recv_buffer_ptr[0],irc.nick)==0){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)recv_buffer[3]);
               }else{
                  swprintf(wresult,L"%s joined",recv_buffer[0]);
                  style_text_t style = {0,RGB(0,128,128)};
                  write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
                  write_nick_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],APPEND);
               }
               break;
            }
            case RECV_KICK:{//nick user host channel victim [message OR null]
               if(recv_buffer_size<6){
                  break;
               }
               if(strcmp(recv_buffer_ptr[4],irc.nick)==0){
                  write_nick_name(tabcontrol_chatview_handle,recv_buffer[3],NULL,REMOVE);
               }
               if(recv_buffer_size==5){
                  swprintf(wresult,L"%s kicked by %s",recv_buffer[4],recv_buffer[0]);
               }else{
                  swprintf(wresult,L"%s kicked by %s (%s)",recv_buffer[4],recv_buffer[0],recv_buffer[5]);
               }
               style_text_t style = {0,RGB(128,128,128)};
               write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               write_nick_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[4],REMOVE);
               break;
            }
            case RECV_NICK:{//nick user host newnick
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"nickchange %s -> %s",recv_buffer[0],recv_buffer[3]);
               style_text_t style = {0,RGB(128,0,128)};
               tab_change_nick(tabcontrol_chatview_handle,recv_buffer[0],recv_buffer[3],wresult,&style,TSTRUE);
               break;
            }
            case RECV_NICK_LIST:{//host channel nicklist
               if(recv_buffer_size<3){
                  break;
               }
               char *nicks = recv_buffer_ptr[2];
               char *d_tokens[IRCPROTOCOL_MAX_NICKS_PER_MESSAGE];
               int s_tokens=0;
               irc_tokenize_nicklist(&irc,nicks,d_tokens,&s_tokens);
               tab_refresh_nicklist(tabcontrol_chatview_handle,recv_buffer[1],d_tokens,s_tokens);
               break;
            }
            case RECV_NICK_MODE:{//nick nick modes
               if(recv_buffer_size<3){
                  break;
               }
               swprintf(wresult,L"%s sets modes: %s %s",recv_buffer[0],recv_buffer[2],recv_buffer[1]);
               style_text_t style = {0,RGB(0,0,255)};
               write_text_name(tabcontrol_chatview_handle,L".status",wresult,&style,TSTRUE);
               break;
            }
            case RECV_NICK_TAKEN:{//host actualnick failednick [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"nick already in use");
               style_text_t style = {0,RGB(255,0,255)};
               write_text_current(tabcontrol_chatview_handle,wresult,&style,TSTRUE);
               break;
            }
            case RECV_PART:{//nick user host channel [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(strcmp(recv_buffer_ptr[0],irc.nick)==0){
                  write_nick_name(tabcontrol_chatview_handle,recv_buffer[3],NULL,REMOVE);
               }
               if(recv_buffer_size==4){
                  swprintf(wresult,L"%s parted",recv_buffer[0]);
               }else{
                  swprintf(wresult,L"%s parted (%s)",recv_buffer[0],recv_buffer[4]);
               }
               style_text_t style = {0,RGB(128,0,0)};
               write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               write_nick_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],REMOVE);
               break;
            }
            case RECV_OTHER:{//host message
               if(recv_buffer_size<2){
                  break;
               }
               swprintf(wresult,L"");
               int i;
               for(i=0;i<recv_buffer_size;i++){
                  swprintf(wresult+wcslen(wresult),L"%s ",recv_buffer[i]);
               }
               write_text_name(tabcontrol_chatview_handle,L".status",wresult,NULL,TSTRUE);
               break;
            }
            case RECV_QUIT:{//nick user host [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(recv_buffer_size==3){
                  swprintf(wresult,L"%s quit",recv_buffer[0]);
               }else{
                  swprintf(wresult,L"%s quit (%s)",recv_buffer[0],recv_buffer[3]);
               }
               style_text_t style = {0,RGB(255,0,0)};
               tab_quit(tabcontrol_chatview_handle,recv_buffer[0],wresult,&style,TSTRUE);
               break;
            }
            case RECV_TOPIC:{//host your_nick canal topic
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"Topic is '%s'",recv_buffer[3]);
               style_text_t style = {0,RGB(0,128,0)};
               write_text_name(tabcontrol_chatview_handle,recv_buffer[2],wresult,&style,TSFALSE);
               break;
            }
            case RECV_TOPIC_CHANGED:{//nick user host canal topic
               if(recv_buffer_size<5){
                  break;
               }
               swprintf(wresult,L"Topic changed by %s to '%s'",recv_buffer[0],recv_buffer[4]);
               style_text_t style = {0,RGB(0,255,0)};
               write_text_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSFALSE);
               break;
            }
         }
      }
      if(connected!=0){
         if(config.reconnect_retries==0){
            SendMessage(hWnd,WM_DISCONNECTING,0,0);
            switch(MessageBox(hWnd,L"Do you really want to reconnect?",L"Reconnect",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
               case IDYES:{
                  SendMessage(hWnd,WM_CONNECTING,0,0);
                  break;
               }
               case IDNO:{
                  break;
               }
            }
         }else{
            SendMessage(hWnd,WM_RECONNECTING,0,0);
         }
      }
   }
   return NULL;
}

LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   //static SHACTIVATEINFO s_sai;
   switch(uMsg){
      case WM_CREATE_TAB:{
         wchar_t *tab_name=(wchar_t*)lParam;
         tab_create(hWnd,tabcontrol_chatview_handle,tab_name,(TAB_TYPE)wParam);
         break;
      }
      case WM_DESTROY_TAB:{
         wchar_t *tab_name=(wchar_t*)lParam;
         tab_delete_name(tabcontrol_chatview_handle,tab_name);
         break;
      }
      case WM_LOAD_CURSOR:{
         loadcursor_icon = SetCursor(LoadCursor(NULL, IDC_WAIT));
         break;
      }
      case WM_UNLOAD_CURSOR:{
         SetCursor(loadcursor_icon);
         break;
      }
      case WM_CONNECTING:{
         if(guiclient_connecting(hWnd)!=0){
            write_text_all(tabcontrol_chatview_handle,L"FAILED TO CONNECT",NULL,TSFALSE);
         }
         break;
      }
      case WM_DISCONNECTING:{
         guiclient_disconnecting(hWnd);
         break;
      }
      case WM_RECONNECTING:{
         if(guiclient_reconnecting(hWnd)!=0){
            write_text_all(tabcontrol_chatview_handle,L"FAILED TO RECONNECT",NULL,TSFALSE);
         }
         break;
      }

      case WM_NOTIFY:{
         LPNMHDR notification = (LPNMHDR)lParam;
         switch(notification->idFrom){
            case EDIT_CHATVIEW_TEXT:{
               switch(notification->code){
                  case EN_LINK:{
                     ENLINK *link = (ENLINK*)lParam;
                     wchar_t wlink[IRC_SIZE_MEDIUM];
                     TEXTRANGE textrange;
                     memcpy(&textrange.chrg,&link->chrg,sizeof(CHARRANGE));
                     textrange.lpstrText = wlink;
                     SendMessage(notification->hwndFrom,EM_GETTEXTRANGE,0,(LPARAM)&textrange);

                     SHELLEXECUTEINFO ShExecInfo = {0};
                     ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                     ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
                     ShExecInfo.hwnd = NULL;
                     ShExecInfo.lpVerb = L"open";
                     ShExecInfo.lpFile = wlink;
                     ShExecInfo.lpParameters = NULL;
                     ShExecInfo.lpDirectory = NULL;
                     ShExecInfo.nShow = SW_SHOW;
                     ShExecInfo.hInstApp = NULL;
                     ShellExecuteEx(&ShExecInfo);
                     break;
                  }
               }
               break;
            }
            case TABCONTROL_CHATVIEW:{
               switch(notification->code){
                  case TCN_SELCHANGING:{
                     tab_refresh(tabcontrol_chatview_handle,HIDE);
                     break;
                  }
                  case TCN_SELCHANGE:{
                     tab_refresh(tabcontrol_chatview_handle,SHOW);
                     break;
                  }
                  case NM_RCLICK:{
                     tab_delete_current(tabcontrol_chatview_handle);
                     break;
                  }
               }
               break;
            }
         }
         break;
      }
      case WM_COMMAND:{
         int wmEvent = HIWORD(wParam);
         HWND control_handler = (HWND)lParam;
         switch (LOWORD(wParam)){
            case EDIT_CHATVIEW_TEXT:{
               switch(wmEvent){
                  case EN_KILLFOCUS:{
                     SendMessage(control_handler,WM_COPY,0,0);
                     break;
                  }
               }
               break;
            }
            case LIST_CHATVIEW_NICK:{
               if(wmEvent==LBN_DBLCLK){
                  wchar_t wnick[IRC_SIZE_SMALL];
                  char nick[IRC_SIZE_SMALL];
                  int element = ListBox_GetCurSel(control_handler);
                  ListBox_GetText(control_handler,element,wnick);
                  if(wcslen(wnick)<1){
                     break;
                  }
                  WideCharToMultiByte(config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
                  char *nick_ptr = irc_get_nick(&irc,nick);
                  if(nick_ptr==NULL){
                     break;
                  }
                  MultiByteToWideChar(config.encoding,0,nick_ptr,-1,wnick,IRC_SIZE_SMALL);
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wnick);
                  tab_select_name(tabcontrol_chatview_handle,wnick);
               }
               break;
            }
            case BUTTON_CLOSETAB:{
               wchar_t wtab_close[IRC_SIZE_SMALL];
               char tab_close[IRC_SIZE_SMALL];
               char *send[2] = {tab_close,config.part};
               tab_get_name_current(tabcontrol_chatview_handle,wtab_close,IRC_SIZE_SMALL);
               WideCharToMultiByte(config.encoding,0,wtab_close,-1,tab_close,IRC_SIZE_SMALL,NULL,NULL);
               if(memcmp(tab_close,".status",7)!=0){
                  if(irc_validate_channel(&irc,tab_close)==0){
                     irc_send_message(&irc,SEND_PART,send,2);
                  }
                  tab_delete_current(tabcontrol_chatview_handle);
               }else{
                  if(connected==0){
                     break;
                  }
                  switch(MessageBox(hWnd,L"About to disconnect. Do you wish to reconnect?",L"Disconnect",MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON3|MB_APPLMODAL|MB_SETFOREGROUND)){
                     case IDYES:{
                        SendMessage(hWnd, WM_RECONNECTING, 0, 0);
                        break;
                     }
                     case IDNO:{
                        SendMessage(hWnd, WM_DISCONNECTING, 0, 0);
                        break;
                     }
                     case IDCANCEL:{
                        break;
                     }
                  }
               }
               break;
            }
            case BUTTON_CHATSEND:{
               static wchar_t wchat_text[IRC_SIZE_MEDIUM];
               static char chat_text[IRC_SIZE_MEDIUM];
               static wchar_t wchat_destination[IRC_SIZE_SMALL];
               static char chat_destination[IRC_SIZE_SMALL];
               static wchar_t wchat_nick[IRC_SIZE_SMALL];
               static wchar_t wchat_return[IRC_SIZE_MEDIUM];
               if(connected==0){
                  break;
               }
               char *send[2]={chat_destination,chat_text};
               GetWindowText(edit_chatinput_handle, wchat_text, IRC_SIZE_MEDIUM);
               if(wcslen(wchat_text)!=0){
                  WideCharToMultiByte(config.encoding,0,wchat_text,-1,chat_text,IRC_SIZE_MEDIUM,NULL,NULL);
                  tab_get_name_current(tabcontrol_chatview_handle,wchat_destination,IRC_SIZE_SMALL);
                  WideCharToMultiByte(config.encoding,0,wchat_destination,-1,chat_destination,IRC_SIZE_SMALL,NULL,NULL);
                  if(memcmp(chat_text,"/",1)==0){
                     //parsing_alias(&irc,chat_destination,chat_text);
                  }else if(memcmp(chat_destination,".status",7)==0){
                     irc_send_message(&irc,SEND_RAW,send+1,1);
                  }else{
                     irc_send_message(&irc,SEND_PRIVMSG,send,2);
                  }
                  Edit_SetText(edit_chatinput_handle,L"");
                  MultiByteToWideChar(config.encoding,0,irc.nick,-1,wchat_nick,IRC_SIZE_SMALL);
                  swprintf(wchat_return,L"%s: %s",wchat_nick,wchat_text);
                  write_text_current(tabcontrol_chatview_handle,wchat_return,NULL,TSTRUE);
               }
               break;
            }
            case IDM_OPTIONS_PREFERENCES:{
               wchar_t *parameters[2]={L"client",profile};
               DialogBoxParam(app_instance, (LPCTSTR)IDD_PREFERENCES, hWnd, PreferencesProc, (LPARAM)parameters);
               break;
            }
            case IDM_OPTIONS_OPENPRIVATE:{
               wchar_t wresult_text[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Open Private", L"", wresult_text, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wresult_text)==0){
                  break;
               }
               SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wresult_text);
               tab_select_name(tabcontrol_chatview_handle,wresult_text);
               break;
            }
            case IDM_OPTIONS_JOINCHANNEL:{
               wchar_t wresult_text[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Join Channel", L"#", wresult_text, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wresult_text)==0){
                  break;
               }
               char result_text[IRC_SIZE_SMALL];
               char *send[1] = {result_text};
               WideCharToMultiByte(config.encoding,0,wresult_text,-1,result_text,IRC_SIZE_SMALL,NULL,NULL);
               irc_send_message(&irc,SEND_JOIN,send,1);
               break;
            }
            /*case IDM_OPENURL:{
               wchar_t wurl[IRC_SIZE_MEDIUM]=L"http://";
               tab_t *current_tab;
               if(tab_get_parameters_current(tabcontrol_chatview_handle,&current_tab)==0){
                  if(wcslen(current_tab->lasturl)!=0){
                     wcscpy(wurl,current_tab->lasturl);
                  }
               }
               wchar_t wresult_text[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Open URL", wurl, wresult_text, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wresult_text)==0){
                  break;
               }
               wchar_t wreturn_text[IRC_SIZE_SMALL];
               if(wcsncmp(L"http://",wresult_text,7)==0){
                  wsprintf(wreturn_text,L"%s",wresult_text);
               }else{
                  wsprintf(wreturn_text,L"http://%s",wresult_text);
               }
               SHELLEXECUTEINFO ShExecInfo = {0};
               ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
               ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
               ShExecInfo.hwnd = NULL;
               ShExecInfo.lpVerb = L"open";
               ShExecInfo.lpFile = wreturn_text;
               ShExecInfo.lpParameters = NULL;
               ShExecInfo.lpDirectory = NULL;
               ShExecInfo.nShow = SW_SHOW;
               ShExecInfo.hInstApp = NULL;
               ShellExecuteEx(&ShExecInfo);
               break;
            }*/
            case IDM_OPTIONS_ABOUT:{
               DialogBoxParam(app_instance, (LPCTSTR)IDD_ABOUTBOX, hWnd, AboutProc, NULL);
               break;
            }
            case IDM_OPTIONS_SETTOPIC:{
               wchar_t wtopic[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Set Topic", L"", wtopic, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wtopic)==0){
                  break;
               }
               wchar_t wchannel[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               tab_get_name_current(tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               if(irc_validate_channel(&irc,channel)!=0){
                  break;
               }
               char topic[IRC_SIZE_SMALL];
               WideCharToMultiByte(config.encoding,0,wtopic,-1,topic,IRC_SIZE_SMALL,NULL,NULL);
               char *send[2]={channel,topic};
               irc_send_message(&irc,SEND_SET_TOPIC,send,2);
               break;
            }
            case IDM_OPTIONS_GETTOPIC:{
               wchar_t wchannel[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               tab_get_name_current(tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               if(irc_validate_channel(&irc,channel)!=0){
                  break;
               }
               char *send[1]={channel};
               irc_send_message(&irc,SEND_GET_TOPIC,send,1);
               break;
            }
            case IDM_CONNECT:
            case IDM_OPTIONS_CONNECT:{
               SendMessage(hWnd, WM_CONNECTING, 0, 0);
               break;
            }
            case IDM_DISCONNECT:
            case IDM_OPTIONS_DISCONNECT:{
               SendMessage(hWnd, WM_DISCONNECTING, 0, 0);
               break;
            }
            case IDM_OPTIONS_EXIT:{
               SendMessage(hWnd, WM_CLOSE, 0, 0);
               break;
            }
         }
         break;
      }
      /*case WM_DRAWITEM:{
         LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
         switch(wParam){
            case TABCONTROL_CHATVIEW:{
               break;
            }
         }
         wchar_t tab_text[IRC_SIZE_SMALL];
         wcscpy(tab_text,L"");

         TCITEM tci;
         tci.mask = TCIF_TEXT;
         tci.pszText = tab_text;
         tci.cchTextMax = IRC_SIZE_SMALL;

         TabCtrl_GetItem(tabcontrol_chatview_handle, lpdis->itemID, &tci);

         FillRect(lpdis->hDC, &lpdis->rcItem, CreateSolidBrush(0x00000000));
         SetBkColor(lpdis->hDC, 0x00000000);

         ExtTextOut();
         TextOut(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,tci.pszText,wcslen(tci.pszText));
         break;
      }*/
      case WM_SIZE:{
         LOG_PIXELS_X = GetScreenCapsX();
         LOG_PIXELS_Y = GetScreenCapsY();

         refresh_client_sizes(LOWORD(lParam),HIWORD(lParam));

         MoveWindow(edit_chatinput_handle,EDITCHAT_LEFT,EDITCHAT_TOP,EDITCHAT_WIDTH,EDITCHAT_HEIGHT,FALSE);
         MoveWindow(button_chatsend_handle,BUTTONCHAT_LEFT,BUTTONCHAT_TOP,BUTTONCHAT_WIDTH,BUTTONCHAT_HEIGHT,FALSE);
         MoveWindow(tabcontrol_chatview_handle,TABCONTROLCHAT_LEFT,TABCONTROLCHAT_TOP,TABCONTROLCHAT_WIDTH,TABCONTROLCHAT_HEIGHT,FALSE);
         MoveWindow(button_closetab_handle,CLOSETAB_LEFT,CLOSETAB_TOP,CLOSETAB_WIDTH,CLOSETAB_HEIGHT,FALSE);
         tab_resize_all(tabcontrol_chatview_handle);
         break;
      }
      case WM_ACTIVATE:{
         //SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         //SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         break;
      }
      case WM_CREATE:{
         /*memset(&s_sai, 0, sizeof(SHACTIVATEINFO));
         s_sai.cbSize = sizeof(SHACTIVATEINFO);

         LOG_PIXELS_X = GetScreenCapsX();
         LOG_PIXELS_Y = GetScreenCapsY();

         LPCREATESTRUCT create = (LPCREATESTRUCT)lParam;
         wcsncpy(profile,(wchar_t*)create->lpCreateParams,IRC_SIZE_SMALL);
         if(guiclient_init(hWnd)!=0){
            PostQuitMessage(0);
         }
         if(menu_bar_handle!=NULL){
            RECT rcMainWindow;
            RECT rcMenuBar;
            GetWindowRect(hWnd, &rcMainWindow);
            GetWindowRect(menu_bar_handle, &rcMenuBar);
            rcMainWindow.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
            MoveWindow(hWnd, rcMainWindow.left, rcMainWindow.top, rcMainWindow.right-rcMainWindow.left, rcMainWindow.bottom-rcMainWindow.top, FALSE);
         }*/
         break;
      }
      case WM_QUIT:{
         //called on PostQuitMessage(0);
         break;
      }
      case WM_HIBERNATE:{
      }
      case WM_CLOSE:{
      }
      case WM_DESTROY:{
         guiclient_destroy(hWnd);
         PostQuitMessage(0);
         break;
      }
   }
   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int guiclient_init(HWND hWnd){
   if(WSAinit_tcp()!=0){
      return -1;
   }
   connected = 0;
   receiver_active = 1;
   receiver_thread_event = CreateEvent(NULL,FALSE,FALSE,NULL);
   if(receiver_thread_event==NULL){
      MessageBox(hWnd,L"Critical error: CreateEvent() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   receiver_thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)receiverThreadProc,(void*)hWnd,0,NULL);
   if(receiver_thread==NULL){
      MessageBox(hWnd,L"Critical error: CreateThread() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      CloseHandle(receiver_thread_event);
      return -1;
   }
   wfile_to_fullpath(L"alert.wav",sound_alert);
   if(irc_config_init(&irc,&config,profile)!=0){
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      CloseHandle(receiver_thread);
      CloseHandle(receiver_thread_event);
      return -1;
   }
   timer_led=NULL;
   init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   init_chat_screen(hWnd);
   tab_create(hWnd,tabcontrol_chatview_handle,L".status",STATUS);
   if(config.connect_on_startup){
      PostMessage(hWnd, WM_CONNECTING, 0, 0);
   }
   return 0;
}

void guiclient_destroy(HWND hWnd){
   receiver_active = 0;
   if(connected!=0){
      connected = 0;
      irc_disconnect(&irc,config.quit);
   }
   destroy_chat_screen();
   destroy_menu_bar();
   SetEvent(receiver_thread_event);
   WaitForSingleObject(receiver_thread,INFINITE);
   CloseHandle(receiver_thread_event);
   CloseHandle(receiver_thread);
   if(timer_led!=NULL){
      timeKillEvent(timer_led);
      deactivate_led(timer_led,0,NULL,NULL,NULL);
   }
   irc_config_destroy(&irc,&config);
   WSAdestroy_tcp();
}

int guiclient_connecting(HWND hWnd){
   if(connected!=0){
      return -1;
   }
   init_loading_screen(hWnd);
   if(irc_config_reload(&irc,&config,profile)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   if(irc_connect(&irc)<0){
      destroy_loading_screen(hWnd);
      return -1;
   }
   connected = 1;
   tab_connect(tabcontrol_chatview_handle,L"CONNECTED",NULL,TSFALSE);
   destroy_menu_bar();
   init_menu_bar(hWnd,IDR_MAIN_MENU_ONLINE);
   SetEvent(receiver_thread_event);
   destroy_loading_screen(hWnd);
   return 0;
}

int guiclient_reconnecting(HWND hWnd){
   if(connected==0){
      return -1;
   }
   init_loading_screen(hWnd);
   connected = 0;
   irc_disconnect(&irc,config.quit);
   tab_disconnect(tabcontrol_chatview_handle,L"DISCONNECTED",NULL,TSFALSE);
   if(irc_config_reload(&irc,&config,profile)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   int trys = config.reconnect_retries;
   while(irc_connect(&irc)<0){
      Sleep(IRC_RECONNECT_TIMEOUT);
      trys--;
      if(trys<=0){
         destroy_menu_bar();
         init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
         destroy_loading_screen(hWnd);
         return -1;
      }
   }
   connected = 1;
   tab_connect(tabcontrol_chatview_handle,L"CONNECTED",NULL,TSFALSE);
   SetEvent(receiver_thread_event);
   destroy_loading_screen(hWnd);
   return 0;
}

void guiclient_disconnecting(HWND hWnd){
   if(connected!=0){
      connected = 0;
      irc_disconnect(&irc,config.quit);
      tab_disconnect(tabcontrol_chatview_handle,L"DISCONNECTED",NULL,TSFALSE);
      destroy_menu_bar();
      init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   }
}
