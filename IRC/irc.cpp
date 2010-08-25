/*
* irc.cpp
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#include <commctrl.h>
#pragma comment(lib, "commctrl.lib")
#include <ceconfig.h>
#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windef.h>
#include <time.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <nled.h>

#include "resource.h"
#include "irc.h"
#include "../util/util.h"
#include "../buffer/buffer.h"
#include "../network/network.h"
#include "../list/list.h"
#include "../iniparser/iniparser.h"
#include "../ircprotocol/ircprotocol.h"

int window_height;
int window_width;
HINSTANCE app_instance;
wchar_t window_title[IRC_SIZE_SMALL];

HWND menu_bar_handle;
HWND static_connecting_handle;

HWND tabcontrol_chatview_handle;
HWND button_closetab_handle;
HWND button_chatsend_handle;
HWND edit_chatinput_handle;
HWND loadcursor_handle;
HCURSOR loadcursor_icon;

MMRESULT timer_led;

HANDLE receiver_thread;
HANDLE receiver_thread_event;
int receiver_active;

irc_t irc;
ircconfig_t config;
int connected;
wchar_t sound_alert[IRC_SIZE_SMALL];
char file_config[IRC_SIZE_SMALL];

wchar_t wchat_text[IRC_SIZE_MEDIUM];
char chat_text[IRC_SIZE_MEDIUM];
wchar_t wchat_destination[IRC_SIZE_SMALL];
char chat_destination[IRC_SIZE_SMALL];
wchar_t wchat_nick[IRC_SIZE_SMALL];
wchar_t wchat_return[IRC_SIZE_MEDIUM];

#include "ircconfig.h"
#include "functions.h"
#include "tab_manager.h"
#include "dialogs_functions.h"
#include "gui_functions.h"

//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
   TCHAR window_class[IRC_SIZE_SMALL];
   LoadString(hInstance, IDS_APP_TITLE, window_title, IRC_SIZE_SMALL);
   LoadString(hInstance, IDS_WNDCLASS_IRC, window_class, IRC_SIZE_SMALL);
   app_instance = hInstance;
   HWND hWnd_Main = FindWindow(window_class, window_title);   
   if(hWnd_Main!=NULL){
      SetForegroundWindow((HWND)((ULONG) hWnd_Main | 0x00000001));
      return 0;
   }
   INITCOMMONCONTROLSEX icex;
   memset(&icex, 0, sizeof(INITCOMMONCONTROLSEX));
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_BAR_CLASSES|ICC_TAB_CLASSES;
   InitCommonControlsEx(&icex);
   SHInitExtraControls();
   WNDCLASS wc;
   memset(&wc, 0, sizeof(WNDCLASS));
   wc.style = CS_HREDRAW|CS_VREDRAW;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRC));
   wc.hCursor = 0;
   wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName = 0;
   wc.lpszClassName = window_class;
   if(wcslen(lpCmdLine)==0){
      wc.lpfnWndProc = WindowProcClient;//WindowProcManager;
   }else{
      wc.lpfnWndProc = WindowProcClient;
   }
   if(!RegisterClass(&wc)){
      return 0;
   }
   hWnd_Main = CreateWindowEx(0, window_class, window_title, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, lpCmdLine);
   if(!hWnd_Main){
      return 0;
   }
   ShowWindow(hWnd_Main, nCmdShow);
   UpdateWindow(hWnd_Main);
   MSG msg;
   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRC));
   while (GetMessage(&msg, NULL, 0, 0)){
      if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
            tab_message(tabcontrol_chatview_handle,L"\r\nFAILED TO CONNECT");
         }
         break;
      }
      case WM_DISCONNECTING:{
         guiclient_disconnecting(hWnd);
         break;
      }
      case WM_RECONNECTING:{
         if(guiclient_reconnecting(hWnd)!=0){
            tab_message(tabcontrol_chatview_handle,L"\r\nFAILED TO RECONNECT");
         }
         break;
      }

      case WM_NOTIFY:{
         switch(LOWORD(wParam)){
            case TABCONTROL_CHATVIEW:{
               switch(((LPNMHDR)lParam)->code){
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
               if(connected==0){
                  break;
               }
               SYSTEMTIME timestamp;
               char *send[2]={chat_destination,chat_text};
               GetWindowText(edit_chatinput_handle, wchat_text, IRC_SIZE_MEDIUM);
               if(wcslen(wchat_text)!=0){
                  WideCharToMultiByte(config.encoding,0,wchat_text,-1,chat_text,IRC_SIZE_MEDIUM,NULL,NULL);
                  tab_get_name_current(tabcontrol_chatview_handle,wchat_destination,IRC_SIZE_SMALL);
                  WideCharToMultiByte(config.encoding,0,wchat_destination,-1,chat_destination,IRC_SIZE_SMALL,NULL,NULL);
                  if(memcmp(chat_destination,".status",7)==0){
                     irc_send_message(&irc,SEND_RAW,send+1,1);
                  }else{
                     irc_send_message(&irc,SEND_PRIVMSG,send,2);
                  }
                  Edit_SetText(edit_chatinput_handle,L"");
                  MultiByteToWideChar(config.encoding,0,irc.nick,-1,wchat_nick,IRC_SIZE_SMALL);
                  GetSystemTime(&timestamp);
                  swprintf(wchat_return,L"\r\n[%02d:%02d:%02d] %s: %s",(unsigned short)timestamp.wHour,(unsigned short)timestamp.wMinute,(unsigned short)timestamp.wSecond,wchat_nick,wchat_text);
                  tab_write_current(tabcontrol_chatview_handle,wchat_return,TEXT,APPEND);
               }
               break;
            }
            case IDM_OPTIONS_PREFERENCES:{
               DialogBox(app_instance, (LPCTSTR)IDD_PREFERENCES, hWnd, PreferencesProc);
               break;
            }
            case IDM_OPTIONS_OPENPRIVATE:{
               wchar_t wresult_text[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Open Private", L"", wresult_text, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(!*wresult_text){
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
               if(!*wresult_text){
                  break;
               }
               char result_text[IRC_SIZE_SMALL];
               char *send[1] = {result_text};
               WideCharToMultiByte(config.encoding,0,wresult_text,-1,result_text,IRC_SIZE_SMALL,NULL,NULL);
               irc_send_message(&irc,SEND_JOIN,send,1);
               break;
            }
            case IDM_OPENURL:{
               wchar_t wresult_text[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Open URL", L"http://", wresult_text, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(!*wresult_text){
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
            }
            case IDM_OPTIONS_ABOUT:{
               DialogBoxParam(app_instance, (LPCTSTR)IDD_ABOUTBOX, hWnd, AboutProc, NULL);
               break;
            }
            case IDM_OPTIONS_SETTOPIC:{
               wchar_t wtopic[IRC_SIZE_SMALL];
               if(open_input_box(hWnd, L"Set Topic", L"", wtopic, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(!*wtopic){
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
      case WM_SIZE:{
         RECT window_sizes;
         GetWindowRect(hWnd, &window_sizes);
         window_width = window_sizes.right;
         window_height = window_sizes.bottom;
         window_height -= window_sizes.top*2;
         MoveWindow(static_connecting_handle,STATICCONNECTING_LEFT*window_width,STATICCONNECTING_TOP*window_height,STATICCONNECTING_WIDTH*window_width,STATICCONNECTING_HEIGHT*window_height,TRUE);
         MoveWindow(edit_chatinput_handle,EDITCHAT_LEFT*window_width,EDITCHAT_TOP*window_height,EDITCHAT_WIDTH*window_width,EDITCHAT_HEIGHT*window_height,TRUE);
         MoveWindow(button_chatsend_handle,BUTTONCHAT_LEFT*window_width,BUTTONCHAT_TOP*window_height,BUTTONCHAT_WIDTH*window_width,BUTTONCHAT_HEIGHT*window_height,TRUE);
         MoveWindow(tabcontrol_chatview_handle,TABCONTROLCHAT_LEFT*window_width,TABCONTROLCHAT_TOP*window_height,TABCONTROLCHAT_WIDTH*window_width,TABCONTROLCHAT_HEIGHT*window_height,TRUE);
         MoveWindow(button_closetab_handle,CLOSETAB_LEFT*window_width,CLOSETAB_TOP*window_height,CLOSETAB_WIDTH*window_width,CLOSETAB_HEIGHT*window_height,TRUE);
         tab_resize_all(tabcontrol_chatview_handle);
         break;
      }
      case WM_ACTIVATE:{
         //SHACTIVATEINFO s_sai;
         //SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         //SHACTIVATEINFO s_sai;
         //SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         switch(wParam){
            case SPI_SETSIPINFO:{
               SIPINFO si;
               memset(&si,0,sizeof(si));
               si.cbSize=sizeof(si);
               if(SHSipInfo(SPI_GETSIPINFO,0,&si,0)){
                  RECT rect = si.rcVisibleDesktop;
                  if(rect.bottom>window_height){
                     MoveWindow(hWnd,rect.left,rect.top,rect.right,rect.bottom-rect.top,TRUE);
                  }else{
                     MoveWindow(hWnd,rect.left,rect.top,rect.right,rect.bottom,TRUE);
                  }
               }
               break;
            }
         }
         break;
      }
      case WM_CREATE:{
         RECT window_sizes;
         GetWindowRect(hWnd, &window_sizes);
         window_width = window_sizes.right;
         window_height = window_sizes.bottom;
         window_height -= window_sizes.top*2;
         if(guiclient_init(hWnd)!=0){
            PostQuitMessage(0);
         }
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

LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_COMMAND:{
         int wmEvent = HIWORD(wParam);
         HWND control_handler = (HWND)lParam;
         switch (LOWORD(wParam)){
            case NULL:{//BOTOES
               break;
            }
         }
         break;
      }
      case WM_SIZE:{
         RECT window_sizes;
         GetWindowRect(hWnd, &window_sizes);
         window_width = window_sizes.right;
         window_height = window_sizes.bottom;
         window_height -= window_sizes.top*2;
         //FAZER
         break;
      }
      case WM_ACTIVATE:{
         //SHACTIVATEINFO s_sai;
         //SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         //SHACTIVATEINFO s_sai;
         //SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         switch(wParam){
            case SPI_SETSIPINFO:{
               SIPINFO si;
               memset(&si,0,sizeof(si));
               si.cbSize=sizeof(si);
               if(SHSipInfo(SPI_GETSIPINFO,0,&si,0)){
                  RECT rect = si.rcVisibleDesktop;
                  if(rect.bottom>window_height){
                     MoveWindow(hWnd,rect.left,rect.top,rect.right,rect.bottom-rect.top,TRUE);
                  }else{
                     MoveWindow(hWnd,rect.left,rect.top,rect.right,rect.bottom,TRUE);
                  }
               }
               break;
            }
         }
         break;
      }
      case WM_CREATE:{
         RECT window_sizes;
         GetWindowRect(hWnd, &window_sizes);
         window_width = window_sizes.right;
         window_height = window_sizes.bottom;
         window_height -= window_sizes.top*2;
         if(guiclient_init(hWnd)!=0){
            PostQuitMessage(0);
         }
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

void *receiverThreadProc(void *window_handle){
   HWND hWnd = (HWND)window_handle;
   SYSTEMTIME timestamp;
   wchar_t wtimestamp[16];
   int i;
   int recv_buffer_size;
   char *recv_buffer_ptr[IRCPROTOCOL_RECV_MAX_TOKENS];
   wchar_t recv_buffer[IRCPROTOCOL_RECV_MAX_TOKENS][IRC_SIZE_MEDIUM];
   wchar_t wresult[IRC_SIZE_MEDIUM];
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
            GetSystemTime(&timestamp);
            swprintf(wtimestamp,L"[%02d:%02d:%02d]",(unsigned short)timestamp.wHour,(unsigned short)timestamp.wMinute,(unsigned short)timestamp.wSecond);
            for(i=0;i<recv_buffer_size;i++){
               MultiByteToWideChar(config.encoding,0,recv_buffer_ptr[i],-1,recv_buffer[i],IRC_SIZE_MEDIUM);
            }
         }
         switch(recv_result){
            case RECV_CHANNEL_MODE:{//[host OR nick user host] channel modes [arguments OR null]
               if(recv_buffer_size<5){
                  if(recv_buffer_size==3){
                     swprintf(wresult,L"\r\n%s %s sets modes: %s",wtimestamp,recv_buffer[0],recv_buffer[2]);
                  }else{
                     swprintf(wresult,L"\r\n%s %s sets modes: %s %s",wtimestamp,recv_buffer[0],recv_buffer[2],recv_buffer[3]);
                  }
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[1],wresult,TEXT,APPEND);
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
                     swprintf(wresult,L"\r\n%s %s sets modes: %s",wtimestamp,recv_buffer[0],recv_buffer[4]);
                  }else{
                     swprintf(wresult,L"\r\n%s %s sets modes: %s %s",wtimestamp,recv_buffer[0],recv_buffer[4],recv_buffer[5]);
                  }
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
               }
               break;
            }
            case RECV_CTCP:{//nick user host destination message
               if(recv_buffer_size<5){
                  break;
               }
               if(memcmp(recv_buffer_ptr[4],"ACTION ",7)!=0){
                  swprintf(wresult,L"\r\n%s %s",wtimestamp,recv_buffer[4]);
                  tab_write_name(tabcontrol_chatview_handle,L".status",wresult,TEXT,APPEND);
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
                  if(config.lednumber>=0)
                     activate_led();
                  if(GetForegroundWindow()!=hWnd){
                     SHNOTIFICATIONDATA sn;
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
                     SHNotificationAdd(&sn);
                  }
               }
               if(recv_result==RECV_CTCP){
                  swprintf(wresult,L"\r\n%s %s %s",wtimestamp,recv_buffer[0],recv_buffer[4]);
               }else if(recv_result==RECV_PRIVMSG){
                  swprintf(wresult,L"\r\n%s %s: %s",wtimestamp,recv_buffer[0],recv_buffer[4]);
               }else if(recv_result==RECV_NOTICE){
                  swprintf(wresult,L"\r\n%s %s: %s",wtimestamp,recv_buffer[0],recv_buffer[4]);
               }
               if(irc_validate_channel(&irc,recv_buffer_ptr[3])==0){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)recv_buffer[3]);
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
               }else{
                  if(recv_result==RECV_NOTICE){
                     tab_write_name(tabcontrol_chatview_handle,L".status",wresult,TEXT,APPEND);
                  }else{
                     SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)recv_buffer[0]);
                     tab_write_name(tabcontrol_chatview_handle,recv_buffer[0],wresult,TEXT,APPEND);
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
                  swprintf(wresult,L"\r\n%s %s joined",wtimestamp,recv_buffer[0]);
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],NICK,APPEND);
               }
               break;
            }
            case RECV_KICK:{//nick user host channel victim [message OR null]
               if(recv_buffer_size<6){
                  break;
               }
               if(strcmp(recv_buffer_ptr[4],irc.nick)==0){
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],NULL,NICK,REMOVE);
               }
               if(recv_buffer_size==5){
                  swprintf(wresult,L"\r\n%s %s kicked by %s",wtimestamp,recv_buffer[4],recv_buffer[0]);
               }else{
                  swprintf(wresult,L"\r\n%s %s kicked by %s (%s)",wtimestamp,recv_buffer[4],recv_buffer[0],recv_buffer[5]);
               }
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[4],NICK,REMOVE);
               break;
            }
            case RECV_NICK:{//nick user host newnick
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"\r\n%s nickchange %s -> %s",wtimestamp,recv_buffer[0],recv_buffer[3]);
               tab_nickchange(tabcontrol_chatview_handle,recv_buffer[0],recv_buffer[3],wresult);
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
               swprintf(wresult,L"\r\n%s %s sets modes: %s %s",wtimestamp,recv_buffer[0],recv_buffer[2],recv_buffer[1]);
               tab_write_name(tabcontrol_chatview_handle,L".status",wresult,TEXT,APPEND);
               break;
            }
            case RECV_NICK_TAKEN:{//host actualnick failednick [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"\r\n%s nick already in use",wtimestamp);
               tab_write_current(tabcontrol_chatview_handle,wresult,TEXT,APPEND);
               break;
            }
            case RECV_PART:{//nick user host channel [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(strcmp(recv_buffer_ptr[0],irc.nick)==0){
                  tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],NULL,NICK,REMOVE);
               }
               if(recv_buffer_size==4){
                  swprintf(wresult,L"\r\n%s %s parted",wtimestamp,recv_buffer[0]);
               }else{
                  swprintf(wresult,L"\r\n%s %s parted (%s)",wtimestamp,recv_buffer[0],recv_buffer[4]);
               }
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],recv_buffer[0],NICK,REMOVE);
               break;
            }
            case RECV_OTHER:{//host message
               if(recv_buffer_size<2){
                  break;
               }
               swprintf(wresult,L"\r\n%s",wtimestamp);
               int i;
               for(i=0;i<recv_buffer_size;i++){
                  swprintf(wresult+wcslen(wresult),L" %s",recv_buffer[i]);
               }
               tab_write_name(tabcontrol_chatview_handle,L".status",wresult,TEXT,APPEND);
               break;
            }
            case RECV_QUIT:{//nick user host [message OR null]
               if(recv_buffer_size<4){
                  break;
               }
               if(recv_buffer_size==3){
                  swprintf(wresult,L"\r\n%s %s quit",wtimestamp,recv_buffer[0]);
               }else{
                  swprintf(wresult,L"\r\n%s %s quit (%s)",wtimestamp,recv_buffer[0],recv_buffer[3]);
               }
               tab_quit(tabcontrol_chatview_handle,recv_buffer[0],wresult);
               break;
            }
            case RECV_TOPIC:{//host your_nick canal topic
               if(recv_buffer_size<4){
                  break;
               }
               swprintf(wresult,L"\r\nTopic is '%s'",recv_buffer[3]);
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[2],wresult,TEXT,APPEND);
               break;
            }
            case RECV_TOPIC_CHANGED:{//nick user host canal topic
               if(recv_buffer_size<5){
                  break;
               }
               swprintf(wresult,L"\r\nTopic changed by %s to '%s'",recv_buffer[0],recv_buffer[4]);
               tab_write_name(tabcontrol_chatview_handle,recv_buffer[3],wresult,TEXT,APPEND);
               break;
            }
         }
      }
      if(connected!=0){
         if(config.reconnect==0){
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
      goto fullerror1;
   }
   if(GetModuleFileName(NULL,sound_alert,IRC_SIZE_SMALL)==0){
      MessageBox(hWnd,L"Critical error: GetModuleFileName() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      goto fullerror;
   }
   wcscpy(wcsrchr(sound_alert,'\\')+1,L"alert.wav");
   wchar_t buffer[IRC_SIZE_SMALL];
   if(GetModuleFileName(NULL,buffer,IRC_SIZE_SMALL)==0){
      MessageBox(hWnd,L"Critical error: GetModuleFileName() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      goto fullerror;
   }
   wcscpy(wcsrchr(buffer,'\\')+1,L"options.ini");
   WideCharToMultiByte(CP_ACP,0,buffer,-1,file_config,IRC_SIZE_SMALL,NULL,NULL);
   if(irc_config_init(&irc,&config,file_config)!=0){
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      goto fullerror;
   }
   timer_led=NULL;
   init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   init_chat_screen(hWnd);
   tab_create(hWnd,tabcontrol_chatview_handle,L".status",STATUS);
   return 0;
   fullerror:
      CloseHandle(receiver_thread);
   fullerror1:
      CloseHandle(receiver_thread_event);
      return -1;
}

void guiclient_destroy(HWND hWnd){
   receiver_active = 0;
   if(connected!=0){
      connected = 0;
      irc_disconnect(&irc,config.quit);
   }
   destroy_chat_screen(hWnd);
   destroy_menu_bar(hWnd);
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
   if(irc_config_reload(&irc,&config,file_config)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   if(irc_connect(&irc)<0){
      destroy_loading_screen(hWnd);
      return -1;
   }
   connected = 1;
   tab_connect(tabcontrol_chatview_handle,L"\r\nCONNECTED");
   destroy_menu_bar(hWnd);
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
   tab_disconnect(tabcontrol_chatview_handle,L"\r\nDISCONNECTED");
   if(irc_config_reload(&irc,&config,file_config)!=0){
      destroy_loading_screen(hWnd);
      MessageBox(hWnd,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   int trys = config.reconnect;
   while(irc_connect(&irc)<0){
      Sleep(IRC_RECONNECT_TIMEOUT);
      trys--;
      if(trys<=0){
         destroy_menu_bar(hWnd);
         init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
         destroy_loading_screen(hWnd);
         return -1;
      }
   }
   connected = 1;
   tab_connect(tabcontrol_chatview_handle,L"\r\nCONNECTED");
   SetEvent(receiver_thread_event);
   destroy_loading_screen(hWnd);
   return 0;
}

void guiclient_disconnecting(HWND hWnd){
   if(connected!=0){
      connected = 0;
      irc_disconnect(&irc,config.quit);
      tab_disconnect(tabcontrol_chatview_handle,L"\r\nDISCONNECTED");
      destroy_menu_bar(hWnd);
      init_menu_bar(hWnd,IDR_MAIN_MENU_OFFLINE);
   }
}
