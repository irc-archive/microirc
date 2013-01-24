/*
* client_proc.h
*
* This file contains functions to use by client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void init_chat_screen(HWND hWnd){
   client.edit_chatinput_handle = CreateWindowEx(0,L"EDIT",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_NOHIDESEL,resize.EDITCHAT_LEFT,resize.EDITCHAT_TOP,resize.EDITCHAT_WIDTH,resize.EDITCHAT_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
   client.button_chatsend_handle = CreateWindowEx(0,L"BUTTON",TEXT("Send"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|BS_CENTER|BS_VCENTER,resize.BUTTONCHAT_LEFT,resize.BUTTONCHAT_TOP,resize.BUTTONCHAT_WIDTH,resize.BUTTONCHAT_HEIGHT,hWnd,(HMENU)BUTTON_CHATSEND,config.h_instance,NULL);
   client.tabcontrol_chatview_handle = CreateWindowEx(0,L"SYSTABCONTROL32",NULL,WS_CHILD|WS_VISIBLE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,resize.TABCONTROLCHAT_LEFT,resize.TABCONTROLCHAT_TOP,resize.TABCONTROLCHAT_WIDTH,resize.TABCONTROLCHAT_HEIGHT,hWnd,(HMENU)TABCONTROL_CHATVIEW,config.h_instance,NULL);
   client.button_closetab_handle = CreateWindowEx(0,L"BUTTON",TEXT("x"),WS_CHILD|WS_VISIBLE|BS_CENTER|BS_VCENTER,resize.CLOSETAB_LEFT,resize.CLOSETAB_TOP,resize.CLOSETAB_WIDTH,resize.CLOSETAB_HEIGHT,hWnd,(HMENU)BUTTON_CLOSETAB,config.h_instance,NULL);

   old_ChatSendProc = (WNDPROC)GetWindowLong(client.edit_chatinput_handle,GWL_WNDPROC);
   SetWindowLong(client.edit_chatinput_handle,GWL_WNDPROC,(LONG)ChatSendProc);

   Edit_LineLength(client.edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   Edit_LimitText(client.edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   TabCtrl_SetPadding(client.tabcontrol_chatview_handle,SCALEX(3),SCALEY(1));
}

void destroy_chat_screen(){
   DestroyWindow(client.edit_chatinput_handle);
   DestroyWindow(client.button_chatsend_handle);
   while(tab_delete_current(client.tabcontrol_chatview_handle)!=-1);
   DestroyWindow(client.tabcontrol_chatview_handle);
   DestroyWindow(client.button_closetab_handle);
}

int guiclient_init(HWND hWnd){
   if(WSAinit_tcp()!=0){
      return -1;
   }
   client.connected = 0;
   client.receiver_active = 1;
   client.receiver_thread_event = CreateEvent(NULL,FALSE,FALSE,NULL);
   if(client.receiver_thread_event==NULL){
      MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG10),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   client.receiver_thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)receiverThreadProc,(void*)hWnd,0,NULL);
   if(client.receiver_thread==NULL){
      MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG15),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      CloseHandle(client.receiver_thread_event);
      return -1;
   }
   wfile_to_fullpath(IRC_CONST_SOUND,client.sound_alert);
   if(irc_config_init(&client.irc,&client.config,client.profile)!=0){
      MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG11),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      CloseHandle(client.receiver_thread);
      CloseHandle(client.receiver_thread_event);
      return -1;
   }
   //timer_led=NULL;
   init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   init_chat_screen(hWnd);
   tab_create(hWnd,client.tabcontrol_chatview_handle,IRC_CONST_STATUS,STATUS);
   if(client.config.connect_on_startup){
      PostMessage(hWnd, WM_CONNECTING, 0, 0);
   }
   return 0;
}

void guiclient_destroy(HWND hWnd){
   client.receiver_active = 0;
   if(client.connected!=0){
      client.connected = 0;
      irc_disconnect(&client.irc,client.config.quit);
   }
   destroy_chat_screen();
   destroy_menu_bar();
   SetEvent(client.receiver_thread_event);
   WaitForSingleObject(client.receiver_thread,INFINITE);
   CloseHandle(client.receiver_thread_event);
   CloseHandle(client.receiver_thread);
   /*if(timer_led!=NULL){
      timeKillEvent(timer_led);
      deactivate_led(timer_led,0,NULL,NULL,NULL);
   }*/
   irc_config_destroy(&client.irc,&client.config);
   WSAdestroy_tcp();
}

int guiclient_connecting(HWND hWnd){
   if(client.connected!=0){
      return -1;
   }
   init_loading_screen(hWnd);
   if(irc_config_reload(&client.irc,&client.config,client.profile)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG11),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   if(irc_connect(&client.irc)<0){
      destroy_loading_screen(hWnd);
      return -1;
   }
   client.connected = 1;
   tab_connect(client.tabcontrol_chatview_handle,MAKEINTSTR(IDS_INFO_MSG3),NULL,TSFALSE);
   destroy_menu_bar();
   init_menu_bar(hWnd,IDR_MAIN_MENU_ONLINE);
   SetEvent(client.receiver_thread_event);
   destroy_loading_screen(hWnd);
   return 0;
}

int guiclient_reconnecting(HWND hWnd){
   if(client.connected==0){
      return -1;
   }
   init_loading_screen(hWnd);
   client.connected = 0;
   irc_disconnect(&client.irc,client.config.quit);
   tab_disconnect(client.tabcontrol_chatview_handle,MAKEINTSTR(IDS_INFO_MSG4),NULL,TSFALSE);
   if(irc_config_reload(&client.irc,&client.config,client.profile)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG11),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   int trys = client.config.reconnect_retries;
   while(irc_connect(&client.irc)<0){
      Sleep(IRC_RECONNECT_TIMEOUT);
      trys--;
      if(trys<=0){
         destroy_menu_bar();
         init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
         destroy_loading_screen(hWnd);
         return -1;
      }
   }
   client.connected = 1;
   tab_connect(client.tabcontrol_chatview_handle,MAKEINTSTR(IDS_INFO_MSG3),NULL,TSFALSE);
   SetEvent(client.receiver_thread_event);
   destroy_loading_screen(hWnd);
   return 0;
}

void guiclient_disconnecting(HWND hWnd){
   if(client.connected!=0){
      client.connected = 0;
      irc_disconnect(&client.irc,client.config.quit);
      tab_disconnect(client.tabcontrol_chatview_handle,MAKEINTSTR(IDS_INFO_MSG4),NULL,TSFALSE);
      destroy_menu_bar();
      init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   }
}

void *receiverThreadProc(void *window_handle){
   HWND hWnd = (HWND)window_handle;
   int i;
   int recv_buffer_size;
   char *recv_buffer_ptr[IRCPROTOCOL_RECV_MAX_TOKENS];
   wchar_t recv_buffer[IRCPROTOCOL_RECV_MAX_TOKENS][IRC_SIZE_MEDIUM];
   wchar_t wresult[IRC_SIZE_MEDIUM];
   //style_text_t formatted;
   int recv_result;
   while(client.receiver_active==1){
      if(WaitForSingleObject(client.receiver_thread_event,INFINITE)!=WAIT_OBJECT_0){
         recv_result = 0;
      }else{
         recv_result = 1;
      }
      while(recv_result > 0){
         recv_result = irc_recv_message(&client.irc,recv_buffer_ptr,&recv_buffer_size);
         if(recv_result!=-1){
            for(i=0;i<recv_buffer_size;i++){
               MultiByteToWideChar(client.config.encoding,0,recv_buffer_ptr[i],-1,recv_buffer[i],IRC_SIZE_MEDIUM);
            }
         }
         switch(recv_result){
            case RECV_CHANNEL_MODE:{//[host OR nick user host] channel modes [arguments OR null]
               if(recv_buffer_size<5){
                  if(recv_buffer_size==3){
                     swprintf(wresult,L"%s %s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG1),recv_buffer[2]);
                  }else{
                     swprintf(wresult,L"%s %s %s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG1),recv_buffer[2],recv_buffer[3]);
                  }
                  style_text_t style = {0,RGB(0,0,255)};
                  write_text_name(client.tabcontrol_chatview_handle,recv_buffer[1],wresult,&style,TSTRUE);
               }else if(recv_buffer_size<7){
                  char *modes = recv_buffer_ptr[4];
                  while(*modes!='\0'){
                     if(strchr(client.irc.prefix_mode,*modes)!=NULL){
                        char *send[1]={recv_buffer_ptr[3]};
                        irc_send_message(&client.irc,SEND_NAMES,send,1);
                        break;
                     }
                     modes++;
                  }
                  if(recv_buffer_size==5){
                     swprintf(wresult,L"%s %s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG1),recv_buffer[4]);
                  }else{
                     swprintf(wresult,L"%s %s %s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG1),recv_buffer[4],recv_buffer[5]);
                  }
                  style_text_t style = {0,RGB(0,0,255)};
                  write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               }
               break;
            }
            case RECV_CTCP:{//nick user host destination message
               if(recv_buffer_size<5){
                  break;
               }
               if(strncmp(recv_buffer_ptr[4],"ACTION ",7)!=0){
                  swprintf(wresult,L"%s",recv_buffer[4]);
                  //style_text_t style = {0,RGB(0,0,0)};
                  write_text_name(client.tabcontrol_chatview_handle,IRC_CONST_STATUS,wresult,NULL,TSTRUE);
                  break;
               }
               recv_buffer_ptr[4]+=7;
               MultiByteToWideChar(client.config.encoding,0,recv_buffer_ptr[4],-1,recv_buffer[4],IRC_SIZE_MEDIUM);
            }
            case RECV_PRIVMSG:{//nick user host destination message
            }
            case RECV_NOTICE:{//nick user host destination message
               if(recv_buffer_size<5){
                  break;
               }
               if(strstri(recv_buffer_ptr[4],client.irc.nick)!=NULL){
                  if(client.config.sounds!=0){
                     PlaySound(client.sound_alert,NULL,SND_ASYNC|SND_FILENAME);
                  }
                  if(client.config.led_number>=0){
                     //activate_led();
                  }
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
               if(irc_validate_channel(&client.irc,recv_buffer_ptr[3])==0){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)recv_buffer[3]);
                  write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,pstyle,TSTRUE);
               }else{
                  if(recv_result==RECV_NOTICE){
                     write_text_name(client.tabcontrol_chatview_handle,IRC_CONST_STATUS,wresult,pstyle,TSTRUE);
                  }else{
                     SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)recv_buffer[0]);
                     write_text_name(client.tabcontrol_chatview_handle,recv_buffer[0],wresult,pstyle,TSTRUE);
                  }
               }
               break;
            }
            case RECV_JOIN:{//nick user host channel
               if(recv_buffer_size<4){
                  break;
               }
               if(strcmp(recv_buffer_ptr[0],client.irc.nick)==0){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)recv_buffer[3]);
               }else{
                  swprintf(wresult,L"%s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG2));
                  style_text_t style = {0,RGB(0,128,128)};
                  write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
                  write_nick_name(client.tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],APPEND);
               }
               break;
            }
            case RECV_KICK:{//nick user host channel victim [message OR null]
               if(recv_buffer_size<6){
                  break;
               }
               if(strcmp(recv_buffer_ptr[4],client.irc.nick)==0){
                  write_nick_name(client.tabcontrol_chatview_handle,recv_buffer[3],NULL,REMOVE);
               }
               if(recv_buffer_size==5){
                  swprintf(wresult,L"%s %s %s",recv_buffer[4],MAKEINTSTR(IDS_PART_MSG3),recv_buffer[0]);
               }else{
                  swprintf(wresult,L"%s %s %s (%s)",recv_buffer[4],MAKEINTSTR(IDS_PART_MSG3),recv_buffer[0],recv_buffer[5]);
               }
               style_text_t style = {0,RGB(128,128,128)};
               write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               write_nick_name(client.tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[4],REMOVE);
               break;
            }
            case RECV_NICK:{//nick user host newnick
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"%s %s -> %s",MAKEINTSTR(IDS_PART_MSG4),recv_buffer[0],recv_buffer[3]);
               style_text_t style = {0,RGB(128,0,128)};
               tab_change_nick(client.tabcontrol_chatview_handle,recv_buffer[0],recv_buffer[3],wresult,&style,TSTRUE);
               break;
            }
            case RECV_NICK_LIST:{//host channel nicklist
               if(recv_buffer_size<3){
                  break;
               }
               char *nicks = recv_buffer_ptr[2];
               char *d_tokens[IRCPROTOCOL_MAX_NICKS_PER_MESSAGE];
               int s_tokens=0;
               irc_tokenize_nicklist(&client.irc,nicks,d_tokens,&s_tokens);
               tab_refresh_nicklist(client.tabcontrol_chatview_handle,recv_buffer[1],d_tokens,s_tokens);
               break;
            }
            case RECV_NICK_MODE:{//nick nick modes
               if(recv_buffer_size<3){
                  break;
               }
               swprintf(wresult,L"%s %s %s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG1),recv_buffer[2],recv_buffer[1]);
               style_text_t style = {0,RGB(0,0,255)};
               write_text_name(client.tabcontrol_chatview_handle,IRC_CONST_STATUS,wresult,&style,TSTRUE);
               break;
            }
            case RECV_NICK_TAKEN:{//host actualnick failednick [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,MAKEINTSTR(IDS_PART_MSG5));
               style_text_t style = {0,RGB(255,0,255)};
               write_text_current(client.tabcontrol_chatview_handle,wresult,&style,TSTRUE);
               break;
            }
            case RECV_PART:{//nick user host channel [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(strcmp(recv_buffer_ptr[0],client.irc.nick)==0){
                  write_nick_name(client.tabcontrol_chatview_handle,recv_buffer[3],NULL,REMOVE);
               }
               if(recv_buffer_size==4){
                  swprintf(wresult,L"%s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG6));
               }else{
                  swprintf(wresult,L"%s %s (%s)",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG6),recv_buffer[4]);
               }
               style_text_t style = {0,RGB(128,0,0)};
               write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSTRUE);
               write_nick_name(client.tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],REMOVE);
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
               write_text_name(client.tabcontrol_chatview_handle,IRC_CONST_STATUS,wresult,NULL,TSTRUE);
               break;
            }
            case RECV_QUIT:{//nick user host [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(recv_buffer_size==3){
                  swprintf(wresult,L"%s %s",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG7));
               }else{
                  swprintf(wresult,L"%s %s (%s)",recv_buffer[0],MAKEINTSTR(IDS_PART_MSG7),recv_buffer[3]);
               }
               style_text_t style = {0,RGB(255,0,0)};
               tab_quit(client.tabcontrol_chatview_handle,recv_buffer[0],wresult,&style,TSTRUE);
               break;
            }
            case RECV_TOPIC:{//host your_nick canal topic
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"%s '%s'",MAKEINTSTR(IDS_PART_MSG8),recv_buffer[3]);
               style_text_t style = {0,RGB(0,128,0)};
               write_text_name(client.tabcontrol_chatview_handle,recv_buffer[2],wresult,&style,TSFALSE);
               break;
            }
            case RECV_TOPIC_CHANGED:{//nick user host canal topic
               if(recv_buffer_size<5){
                  break;
               }
               swprintf(wresult,L"%s %s %s '%s'",MAKEINTSTR(IDS_PART_MSG9),recv_buffer[0],MAKEINTSTR(IDS_PART_MSG9),recv_buffer[4]);
               style_text_t style = {0,RGB(0,255,0)};
               write_text_name(client.tabcontrol_chatview_handle,recv_buffer[3],wresult,&style,TSFALSE);
               break;
            }
         }
      }
      if(client.connected!=0){
         if(client.config.reconnect_retries==0){
            SendMessage(hWnd,WM_DISCONNECTING,0,0);
            switch(MessageBox(hWnd,MAKEINTSTR(IDS_QUEST_MSG1),MAKEINTSTR(IDS_QUEST_MSG2),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
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
