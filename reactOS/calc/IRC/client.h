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

WNDPROC old_ChatSendProc;
LRESULT CALLBACK ChatSendProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#include "client_proc.h"

LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_CREATE_TAB:{
         wchar_t *tab_name=(wchar_t*)lParam;
         tab_create(hWnd,client.tabcontrol_chatview_handle,tab_name,(TAB_TYPE)wParam);
         break;
      }
      case WM_DESTROY_TAB:{
         wchar_t *tab_name=(wchar_t*)lParam;
         tab_delete_name(client.tabcontrol_chatview_handle,tab_name);
         break;
      }
      case WM_LOAD_CURSOR:{
         client.loadcursor_icon = SetCursor(LoadCursor(NULL, IDC_WAIT));
         break;
      }
      case WM_UNLOAD_CURSOR:{
         SetCursor(client.loadcursor_icon);
         break;
      }
      case WM_CONNECTING:{
         if(guiclient_connecting(hWnd)!=0){
            write_text_all(client.tabcontrol_chatview_handle,L"FAILED TO CONNECT",NULL,TSFALSE);
         }
         break;
      }
      case WM_DISCONNECTING:{
         guiclient_disconnecting(hWnd);
         break;
      }
      case WM_RECONNECTING:{
         if(guiclient_reconnecting(hWnd)!=0){
            write_text_all(client.tabcontrol_chatview_handle,L"FAILED TO RECONNECT",NULL,TSFALSE);
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
                     if(link->msg == WM_LBUTTONUP){
                         wchar_t wlink[IRC_SIZE_MEDIUM];
                         TEXTRANGE textrange;
                         memcpy(&textrange.chrg,&link->chrg,sizeof(textrange.chrg));
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
                     }
                     break;
                  }
               }
               break;
            }
            case TABCONTROL_CHATVIEW:{
               switch(notification->code){
                  case TCN_SELCHANGING:{
                     tab_refresh(client.tabcontrol_chatview_handle,HIDE);
                     break;
                  }
                  case TCN_SELCHANGE:{
                     tab_refresh(client.tabcontrol_chatview_handle,SHOW);
                     break;
                  }
                  //case NM_RCLICK:{
                  //   tab_delete_current(client.tabcontrol_chatview_handle);
                  //   break;
                  //}
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
                     //SendMessage(hWnd, EM_SETSEL, EDITCHATVIEWTEXT_LIMIT, EDITCHATVIEWTEXT_LIMIT);
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
                  WideCharToMultiByte(client.config.encoding,0,wnick,-1,nick,IRC_SIZE_MEDIUM,NULL,NULL);
                  char *nick_ptr = irc_get_nick(&client.irc,nick);
                  if(nick_ptr==NULL){
                     break;
                  }
                  MultiByteToWideChar(client.config.encoding,0,nick_ptr,-1,wnick,IRC_SIZE_SMALL);
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wnick);
                  tab_select_name(client.tabcontrol_chatview_handle,wnick);
               }
               break;
            }
            case BUTTON_CLOSETAB:{
               wchar_t wtab_close[IRC_SIZE_SMALL];
               char tab_close[IRC_SIZE_SMALL];
               char *send[2] = {tab_close,client.config.part};
               tab_get_name_current(client.tabcontrol_chatview_handle,wtab_close,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wtab_close,-1,tab_close,IRC_SIZE_SMALL,NULL,NULL);
               if(memcmp(tab_close,".status",7)!=0){
                  if(client.connected!=0){
                      if(irc_validate_channel(&client.irc,tab_close)==0){
                         irc_send_message(&client.irc,SEND_PART,send,2);
                      }
                  }
                  tab_delete_current(client.tabcontrol_chatview_handle);
               }else{
                  if(client.connected==0){
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
               if(client.connected==0){
                  break;
               }
               char *send[2]={chat_destination,chat_text};
               GetWindowText(client.edit_chatinput_handle, wchat_text, IRC_SIZE_MEDIUM);
               if(wcslen(wchat_text)!=0){
                  WideCharToMultiByte(client.config.encoding,0,wchat_text,-1,chat_text,IRC_SIZE_MEDIUM,NULL,NULL);
                  tab_get_name_current(client.tabcontrol_chatview_handle,wchat_destination,IRC_SIZE_SMALL);
                  WideCharToMultiByte(client.config.encoding,0,wchat_destination,-1,chat_destination,IRC_SIZE_SMALL,NULL,NULL);
                  if(memcmp(chat_text,"/",1)==0){
                     //parsing_alias(&client.irc,chat_destination,chat_text);
                  }else if(memcmp(chat_destination,".status",7)==0){
                     irc_send_message(&client.irc,SEND_RAW,send+1,1);
                  }else{
                     irc_send_message(&client.irc,SEND_PRIVMSG,send,2);
                  }
                  Edit_SetText(client.edit_chatinput_handle,L"");
                  MultiByteToWideChar(client.config.encoding,0,client.irc.nick,-1,wchat_nick,IRC_SIZE_SMALL);
                  swprintf(wchat_return,L"%s: %s",wchat_nick,wchat_text);
                  write_text_current(client.tabcontrol_chatview_handle,wchat_return,NULL,TSTRUE);
               }
               break;
            }
            case IDM_OPTIONS_PREFERENCES:{
               wchar_t *parameters[2]={L"client",client.profile};
               DialogBoxParam(config.h_instance, (LPCTSTR)IDD_PREFERENCES, hWnd, PreferencesProc, (LPARAM)parameters);
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
               tab_select_name(client.tabcontrol_chatview_handle,wresult_text);
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
               WideCharToMultiByte(client.config.encoding,0,wresult_text,-1,result_text,IRC_SIZE_SMALL,NULL,NULL);
               irc_send_message(&client.irc,SEND_JOIN,send,1);
               break;
            }
            case IDM_OPTIONS_ABOUT:{
               DialogBoxParam(config.h_instance, (LPCTSTR)IDD_ABOUTBOX, hWnd, AboutProc, (LPARAM )NULL);
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
               tab_get_name_current(client.tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               if(irc_validate_channel(&client.irc,channel)!=0){
                  break;
               }
               char topic[IRC_SIZE_SMALL];
               WideCharToMultiByte(client.config.encoding,0,wtopic,-1,topic,IRC_SIZE_SMALL,NULL,NULL);
               char *send[2]={channel,topic};
               irc_send_message(&client.irc,SEND_SET_TOPIC,send,2);
               break;
            }
            case IDM_OPTIONS_GETTOPIC:{
               wchar_t wchannel[IRC_SIZE_SMALL];
               char channel[IRC_SIZE_SMALL];
               tab_get_name_current(client.tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               if(irc_validate_channel(&client.irc,channel)!=0){
                  break;
               }
               char *send[1]={channel};
               irc_send_message(&client.irc,SEND_GET_TOPIC,send,1);
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
      case WM_MOUSEWHEEL:{
         /* Redirect scroll events to the main chat window */
         tab_t* tab;
         tab_get_parameters_current(client.tabcontrol_chatview_handle,&tab);
         return ChatViewTextProc(tab->text, uMsg, wParam, lParam);
      }
      case WM_SIZE:{
         config.LOG_PIXELS_X = get_screen_caps_x();
         config.LOG_PIXELS_Y = get_screen_caps_y();

         refresh_client_sizes(LOWORD(lParam),HIWORD(lParam));

         MoveWindow(client.edit_chatinput_handle,resize.EDITCHAT_LEFT,resize.EDITCHAT_TOP,resize.EDITCHAT_WIDTH,resize.EDITCHAT_HEIGHT,FALSE);
         MoveWindow(client.button_chatsend_handle,resize.BUTTONCHAT_LEFT,resize.BUTTONCHAT_TOP,resize.BUTTONCHAT_WIDTH,resize.BUTTONCHAT_HEIGHT,FALSE);
         MoveWindow(client.tabcontrol_chatview_handle,resize.TABCONTROLCHAT_LEFT,resize.TABCONTROLCHAT_TOP,resize.TABCONTROLCHAT_WIDTH,resize.TABCONTROLCHAT_HEIGHT,FALSE);
         MoveWindow(client.button_closetab_handle,resize.CLOSETAB_LEFT,resize.CLOSETAB_TOP,resize.CLOSETAB_WIDTH,resize.CLOSETAB_HEIGHT,FALSE);
         tab_resize_all(client.tabcontrol_chatview_handle);
         break;
      }
      case WM_ACTIVATE:{
         if(wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE){
            SetFocus(client.edit_chatinput_handle);
            return TRUE;
         }
         //SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         //SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         break;
      }
      case WM_CREATE:{
         config.LOG_PIXELS_X = get_screen_caps_x();
         config.LOG_PIXELS_Y = get_screen_caps_y();

         LPCREATESTRUCT create = (LPCREATESTRUCT)lParam;
         wcsncpy(client.profile,(wchar_t*)create->lpCreateParams,IRC_SIZE_SMALL);
         if(guiclient_init(hWnd)!=0){
            PostQuitMessage(0);
         }
         break;
      }
      case WM_QUIT:{
         //called on PostQuitMessage(0);
         break;
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

LRESULT CALLBACK ChatSendProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_CHAR:{
         if(wParam == VK_RETURN){
            SendMessage(GetParent(hWnd),WM_COMMAND,BUTTON_CHATSEND,0);
            return 0;
         }
         break;
      }
      case WM_KEYDOWN:{
      }
      case WM_KEYUP:{
         /* Redirect page up/down and up/down arrow events to the main chat window */
         if(wParam == VK_PRIOR || wParam == VK_NEXT || wParam == VK_UP || wParam == VK_DOWN){
             tab_t* tab;
             tab_get_parameters_current(client.tabcontrol_chatview_handle,&tab);
             return ChatViewTextProc(tab->text, uMsg, wParam, lParam);
         }
         break;
      }
   }
   return CallWindowProc(old_ChatSendProc, hWnd, uMsg, wParam, lParam);
}