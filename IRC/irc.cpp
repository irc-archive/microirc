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
#include <time.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <nled.h>

int height;
int width;

HINSTANCE hInstance_Main;

HWND hWnd_MenuBar;
HWND hWnd_ButtonConnect;
HWND hWnd_StaticConnecting;

HWND hWnd_TabControlChat;
HWND hWnd_CloseTab;
HWND hWnd_ButtonChat;
HWND hWnd_EditChat;
HWND hWnd_TapAndHold;

HCURSOR hOldCursor;

#include "resource.h"
#include "irc.h"
#include "buffer.h"
#include "network.h"
#include "list.h"
#include "iniparser.h"
#include "ircprotocol.h"
#include "tab_manager.h"

HANDLE thread;
HANDLE open_handle;
int open_state;

wchar_t alertsound[IRC_SIZE_LITTLE];
char configfile[IRC_SIZE_LITTLE];

wchar_t wdestination[IRC_SIZE_LITTLE];
char tdestination[IRC_SIZE_LITTLE];
wchar_t wnick[IRC_SIZE_LITTLE];

wchar_t wtextprocess[IRC_SIZE_MEDIUM];
char ttextprocess[IRC_SIZE_MEDIUM];
wchar_t wwritebuffer[IRC_SIZE_MEDIUM];

irc_t irc;
ircconfig_t config;
int connected;

#include "functions.h"
#include "dialogs.h"
#include "gui_functions.h"

//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
   hInstance_Main = hInstance;
   MSG msg;
   TCHAR szTitle[IRC_SIZE_LITTLE];
   TCHAR szWindowClass[IRC_SIZE_LITTLE];
   LoadString(hInstance, IDS_APP_TITLE, szTitle, IRC_SIZE_LITTLE); //L"IRC v1.0"
   LoadString(hInstance, IDC_IRC, szWindowClass, IRC_SIZE_LITTLE); //L"irc application"
   HWND hWnd_Main = FindWindow(szWindowClass, szTitle);   
   if(hWnd_Main){
      SetForegroundWindow((HWND)((ULONG) hWnd_Main | 0x00000001));
      return 0;
   }
   InitCommonControls();
   SHInitExtraControls();
   WNDCLASS wc;
   wc.style = CS_HREDRAW|CS_VREDRAW;
   wc.lpfnWndProc = WindowProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRC));
   wc.hCursor = 0;
   wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName = 0;
   wc.lpszClassName = szWindowClass;
   if(!RegisterClass(&wc)){
      return FALSE;
   }
   hWnd_Main = CreateWindowEx(0, szWindowClass, szTitle, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, NULL);
   if(!hWnd_Main){
      return 0;
   }
   ShowWindow(hWnd_Main, nCmdShow);
   UpdateWindow(hWnd_Main);
   HACCEL hAccelTable;
   hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRC));
   while (GetMessage(&msg, NULL, 0, 0)){
      if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT event_id, WPARAM element_id, LPARAM param_id){
   switch (event_id){
      case WM_CREATE_TAB:{
         wchar_t *tab_name=(wchar_t*)param_id;
         tab_create(hWnd,hWnd_TabControlChat,tab_name,(TAB_TYPE)element_id);
         break;
      }
      case WM_DESTROY_TAB:{
         wchar_t *tab_name=(wchar_t*)param_id;
         tab_delete_name(hWnd_TabControlChat,tab_name);
         break;
      }
      case WM_LOAD_CURSOR:{
         hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
         break;
      }
      case WM_UNLOAD_CURSOR:{
         SetCursor(hOldCursor);
         break;
      }
      case WM_CONNECTING:{
         if(connecting(hWnd)!=0){
            MessageBox(hWnd,L"Error connecting to server.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
         }
         break;
      }
      case WM_DISCONNECTING:{
         disconnecting(hWnd);
         break;
      }
      case WM_RECONNECTING:{
         if(reconnecting(hWnd)!=0){
            MessageBox(hWnd,L"Error reconnecting to server.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
         }
         break;
      }
      case WM_NOTIFY:{
         switch(LOWORD(element_id)){
            case TAB_CONTROL:{
               switch(((LPNMHDR)param_id)->code){
                  case TCN_SELCHANGING:{
                     tab_refresh(hWnd_TabControlChat,HIDE);
                     break;
                  }
                  case TCN_SELCHANGE:{
                     tab_refresh(hWnd_TabControlChat,SHOW);
                     break;
                  }
                  case NM_RCLICK:{
                     tab_delete_actual(hWnd_TabControlChat);
                     break;
                  }
               }
               break;
            }
         }
         break;
      }
      case WM_COMMAND:{
         int wmEvent = HIWORD(element_id);
         HWND controlHwnd = (HWND)param_id;
         switch (LOWORD(element_id)){
            case BUTTON_CONNECT:{
               if(connecting(hWnd)!=0){
                  MessageBox(hWnd,L"Error connecting to server.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               }
               break;
            }
            case TALK_BOX:{
               switch(wmEvent){
                  case EN_SETFOCUS:{
                     //SetFocus(hWnd_EditChat);
                     break;
                  }
                  case EN_KILLFOCUS:{
                     SendMessage(controlHwnd,WM_COPY,0,0);
                     break;
                  }
               }
               break;
            }
            case LIST_BOX:{
               if(wmEvent==LBN_DBLCLK){
                  int element = ListBox_GetCurSel(controlHwnd);
                  ListBox_GetText(controlHwnd,element,wtextprocess);
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wtextprocess);
                  tab_select_name(hWnd_TabControlChat,wtextprocess);
               }
               break;
            }
            case BUTTON_CLOSE:{
               char *send[1];
               send[0]=ttextprocess;
               tab_get_name_actual(hWnd_TabControlChat,wtextprocess,IRC_SIZE_MEDIUM);
               WideCharToMultiByte(config.encoding,0,wtextprocess,-1,ttextprocess,IRC_SIZE_MEDIUM,NULL,NULL);
               if(memcmp(ttextprocess,".status",7)!=0){
                  if(memcmp(ttextprocess,"#",1)==0){
                     irc_send_message(&irc,SEND_PART,send,1);
                  }
                  tab_delete_actual(hWnd_TabControlChat);
               }else{
                  switch (MessageBox(hWnd,L"Do you really want to quit?",L"Quit",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
                     case IDYES:{
                        disconnecting(hWnd);
                        break;
                     }
                     case IDNO:{
                        break;
                     }
                  }
               }
               break;
            }
            case BUTTON_SEND:{
               SYSTEMTIME timestamp;
               char *send[2];
               send[1]=ttextprocess;
               GetWindowText(hWnd_EditChat, wtextprocess, IRC_SIZE_MEDIUM);
               if(wcslen(wtextprocess)!=0){
                  WideCharToMultiByte(config.encoding,0,wtextprocess,-1,ttextprocess,IRC_SIZE_MEDIUM,NULL,NULL);
                  tab_get_name_actual(hWnd_TabControlChat,wdestination,IRC_SIZE_LITTLE);
                  WideCharToMultiByte(config.encoding,0,wdestination,-1,tdestination,IRC_SIZE_LITTLE,NULL,NULL);
                  send[0]=tdestination;
                  MultiByteToWideChar(config.encoding,0,irc.nick,-1,wnick,IRC_SIZE_LITTLE);
                  if(memcmp(tdestination,".status",7)==0){
                     send[0]=send[1];
                     irc_send_message(&irc,SEND_RAW,send,1);
                     Edit_SetText(hWnd_EditChat,L"");
                  }else{
                     irc_send_message(&irc,SEND_PRIVMSG,send,2);
                     Edit_SetText(hWnd_EditChat,L"");
                  }
                  GetSystemTime(&timestamp);
                  swprintf(wwritebuffer,L"\r\n[%02d:%02d:%02d] %s: %s",(unsigned short)timestamp.wHour,(unsigned short)timestamp.wMinute,(unsigned short)timestamp.wSecond,wnick,wtextprocess);
                  tab_write_actual(hWnd_TabControlChat,wwritebuffer,TEXT,APPEND);
               }
               break;
            }
            case ID_OPTIONS_PREFERENCES:{
               DialogBox(hInstance_Main, (LPCTSTR)IDD_PREFERENCES, hWnd, Preferences);
               break;
            }
            case ID_OPTIONS_OPENPRIVATE:{
              if(!connected){
                 break;
              }
              wchar_t result[IRC_SIZE_LITTLE];
              open_input_box(hWnd, L"Open Private", result, IRC_SIZE_LITTLE);
              if(!*result){
                 break;
              }
              SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)result);
              tab_select_name(hWnd_TabControlChat,result);
              break;
            }
            case ID_OPTIONS_JOINCHANNEL:{
              if(!connected){
                 break;
              }
              wchar_t result[IRC_SIZE_LITTLE];
              open_input_box(hWnd, L"Join Channel", result, IRC_SIZE_LITTLE);
              if(!*result){
                 break;
              }
              char tresult[IRC_SIZE_LITTLE];
              char *send[1] = {tresult};
              WideCharToMultiByte(CP_UTF8,0,result,-1,tresult,IRC_SIZE_LITTLE,NULL,NULL);
              irc_send_message(&irc,SEND_JOIN,send,1);
              break;
            }
            case IDM_OPENURL:{
               wchar_t result[IRC_SIZE_LITTLE];
               open_input_box(hWnd, L"Open URL", result, IRC_SIZE_LITTLE);
               if(!*result){
                  break;
               }
               if(wcsncmp(L"http://",result,7)==0){
                  wsprintf(wtextprocess,L"%s",result);
               }else{
                  wsprintf(wtextprocess,L"http://%s",result);
               }
               SHELLEXECUTEINFO ShExecInfo;
               ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
               ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
               ShExecInfo.hwnd = NULL;
               ShExecInfo.lpVerb = L"open";
               ShExecInfo.lpFile = wtextprocess;
               ShExecInfo.lpParameters = NULL;
               ShExecInfo.lpDirectory = NULL;
               ShExecInfo.nShow = SW_SHOW;
               ShExecInfo.hInstApp = NULL;
               ShellExecuteEx(&ShExecInfo);
               break;
            }
            case ID_OPTIONS_ABOUT:{
               MessageBox(hWnd, L"µIRC(C) 2009-2010\ncode.google.com/p/microirc\nThis program is licenced under the GPL version 2.\nFor details see COPYING.txt.", L"About", MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            case ID_OPTIONS_SETTOPIC:{
               if(!connected){
                  break;
               }
               wchar_t wtopic[IRC_SIZE_LITTLE];
               if(open_input_box(hWnd, L"Set Topic", wtopic, IRC_SIZE_LITTLE)!=0){
                  break;
               }
               wchar_t wchannel[IRC_SIZE_LITTLE];
               tab_get_name_actual(hWnd_TabControlChat,wchannel,IRC_SIZE_LITTLE);
               if(wcsncmp(wchannel,L"#",1)!=0){
                  break;
               }
               char topic[IRC_SIZE_LITTLE];
               char channel[IRC_SIZE_LITTLE];
               WideCharToMultiByte(CP_UTF8,0,wtopic,-1,topic,IRC_SIZE_LITTLE,NULL,NULL);
               WideCharToMultiByte(CP_UTF8,0,wchannel,-1,channel,IRC_SIZE_LITTLE,NULL,NULL);
               char *send[2]={channel,topic};
               irc_send_message(&irc,SEND_SET_TOPIC,send,2);
               break;
            }
            case ID_OPTIONS_GETTOPIC:{
               if(!connected){
                  break;
               }
               wchar_t wchannel[IRC_SIZE_LITTLE];
               tab_get_name_actual(hWnd_TabControlChat,wchannel,IRC_SIZE_LITTLE);
               if(wcsncmp(wchannel,L"#",1)!=0){
                  break;
               }
               char channel[IRC_SIZE_LITTLE];
               WideCharToMultiByte(CP_UTF8,0,wchannel,-1,channel,IRC_SIZE_LITTLE,NULL,NULL);
               char *send[1]={channel};
               irc_send_message(&irc,SEND_GET_TOPIC,send,1);
               break;
            }
            case ID_OPTIONS_EXIT:{
               SendMessage(hWnd, WM_CLOSE, 0, 0);
               break;
            }
         }
         break;
      }
      case WM_SIZE:{
         width = LOWORD(param_id);
         height = HIWORD(param_id);
         break;
      }
      case WM_ACTIVATE:{
         SHACTIVATEINFO s_sai;
         SHHandleWMActivate(hWnd, element_id, param_id, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         SHACTIVATEINFO s_sai;
         SHHandleWMSettingChange(hWnd, element_id, param_id, &s_sai);
         break;
      }
      case WM_CREATE:{
         if(init(hWnd)!=0){
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
         destroy();
         PostQuitMessage(0);
         break;
      }
   }
   return DefWindowProc(hWnd, event_id, element_id, param_id);
}

void *thread_procedure(void *inused){
   HWND hWnd = (HWND)inused;
   SYSTEMTIME systime_now;
   wchar_t timestamp[11];
   int tchar_size;
   char *tchar_buffer[IRC_RECV_MAX_TOKENS];
   wchar_t wchar_buffer[IRC_RECV_MAX_TOKENS][IRC_SIZE_MEDIUM];
   int recv_result;
   while(open_state==1){
      if(WaitForSingleObject(open_handle,INFINITE)!=WAIT_OBJECT_0){
         recv_result = 0;
      }else{
         recv_result = 1;
      }
      while(recv_result > 0){
         recv_result = irc_recv_message(&irc,tchar_buffer,&tchar_size);
         GetSystemTime(&systime_now);
         swprintf(timestamp,L"[%02d:%02d:%02d]",(unsigned short)systime_now.wHour,(unsigned short)systime_now.wMinute,(unsigned short)systime_now.wSecond);
         switch(recv_result){
            case RECV_CTCP:{//nick user host destination mensagem
               if(tchar_size<5){
                  break;
               }
               if(memcmp(tchar_buffer[4],"ACTION ",7)!=0){
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[4],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s",timestamp,wchar_buffer[0]);
                  tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
                  break;
               }
               tchar_buffer[4]+=7;
            }
            case RECV_PRIVMSG:{//nick user host destination mensagem
            }
            case RECV_NOTICE:{//nick user host destination mensagem
               if(tchar_size<5){
                  break;
               }
               if(strstri(tchar_buffer[4],irc.nick)!=NULL){//StrCmpI
                  if(config.sounds!=0)
                     PlaySound(alertsound,NULL,SND_ASYNC|SND_FILENAME);
                  if(config.lednumber>=0)
                     activate_led();
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               if(recv_result==RECV_CTCP){
                  swprintf(wchar_buffer[4],L"\r\n%s %s %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }else if(recv_result==RECV_PRIVMSG){
                  swprintf(wchar_buffer[4],L"\r\n%s %s: %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }else if(recv_result==RECV_NOTICE){
                  swprintf(wchar_buffer[4],L"\r\n%s %s: %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }
               if(tchar_buffer[3][0]=='#'){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)wchar_buffer[1]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               }else{
                  if(recv_result==RECV_NOTICE){
                     tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
                  }else{
                     SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wchar_buffer[0]);
                     tab_write_name(hWnd_TabControlChat,wchar_buffer[0],wchar_buffer[4],TEXT,APPEND);
                  }
               }
               break;
            }
            case RECV_JOIN:{//nick user host canal
               if(tchar_size<4){
                  break;
               }
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)wchar_buffer[1]);
               }else{
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s joined",timestamp,wchar_buffer[0]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[0],NICK,APPEND);
               }
               break;
            }
            case RECV_KICK:{//nick user host canal nick_vitima mensagem
               if(tchar_size<6){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               if(strcmp(tchar_buffer[4],irc.nick)==0){
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],NULL,NICK,REMOVE);
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               if(tchar_size==5){
                  swprintf(wchar_buffer[4],L"\r\n%s %s kicked by %s",timestamp,wchar_buffer[2],wchar_buffer[0]);
               }else{
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[5],-1,wchar_buffer[3],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s kicked by %s (%s)",timestamp,wchar_buffer[2],wchar_buffer[0],wchar_buffer[3]);
               }
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[2],NICK,REMOVE);
               break;
            }
            case RECV_NICK:{//nick user host newnick
               if(tchar_size<4){
                  break;
               }
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  sprintf(irc.nick,"%s",tchar_buffer[3]);
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               tab_nickchange(hWnd_TabControlChat,timestamp,wchar_buffer[0],wchar_buffer[1]);
               break;
            }
            case RECV_NICK_LIST:{//host canal nicks
               if(tchar_size<3){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[1],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               char *nicks = tchar_buffer[2];
               char *d_tokens[IRC_MAX_NICKS_PER_MESSAGE];
               int s_tokens=0;
               int i;
               tokens_required(nicks,CHAR_SPACE,IRC_MAX_NICKS_PER_MESSAGE,d_tokens,&s_tokens);
               for(i=0;i<s_tokens;i++){
                  MultiByteToWideChar(config.encoding,0,d_tokens[i],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[0],wchar_buffer[1],NICK,APPEND);
               }
               break;
            }
            case RECV_NICK_TAKEN:{//host actualnick failednick message
               swprintf(wchar_buffer[4],L"\r\n%s nick already in use",timestamp);
               tab_write_actual(hWnd_TabControlChat,wchar_buffer[4],TEXT,APPEND);
               break;
            }
            case RECV_PART:{//nick user host canal mensagem
               if(tchar_size<4){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],NULL,NICK,REMOVE);
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               if(tchar_size==4){
                  swprintf(wchar_buffer[4],L"\r\n%s %s parted",timestamp,wchar_buffer[0]);
               }else{
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s parted (%s)",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[0],NICK,REMOVE);
               break;
            }
            case RECV_OTHER:{//host message
               if(tchar_size<2){
                  break;
               }
               swprintf(wchar_buffer[4],L"\r\n%s",timestamp);
               int i;
               for(i=0;i<tchar_size;i++){
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[i],-1,wchar_buffer[i],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4]+wcslen(wchar_buffer[4]),L" %s",wchar_buffer[i]);
               }
               tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
               break;
            }
            case RECV_QUIT:{//nick user host mensagem
               if(tchar_size<4){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               if(tchar_size==3){
                  tab_quit(hWnd_TabControlChat,timestamp,wchar_buffer[0],L"");
               }else{
                  MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  tab_quit(hWnd_TabControlChat,timestamp,wchar_buffer[0],wchar_buffer[1]);
               }
               break;
            }
            case RECV_TOPIC:{//host your_nick canal topic
               if(tchar_size<4){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[2],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               swprintf(wchar_buffer[4],L"\r\nTopic is '%s'",wchar_buffer[1]);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[0],wchar_buffer[4],TEXT,APPEND);
               break;
            }
            case RECV_TOPIC_CHANGED:{//nick user host canal topic
               if(tchar_size<5){
                  break;
               }
               MultiByteToWideChar(config.encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(config.encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               swprintf(wchar_buffer[4],L"\r\nTopic changed by %s to '%s'",wchar_buffer[0],wchar_buffer[2]);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               break;
            }
         }
      }
      if(connected==1){
         if(config.reconnect==0){
            switch (MessageBox(NULL,L"Do you really want to reconnect?",L"Disconnected",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_SETFOREGROUND)){
               case IDYES:{
                  SendMessage(hWnd,WM_DISCONNECTING,0,0);
                  SendMessage(hWnd,WM_CONNECTING,0,0);
                  break;
               }
               case IDNO:{
                  SendMessage(hWnd,WM_DISCONNECTING,0,0);
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

int init(HWND hWnd){
   connected = 0;
   open_state = 1;
   open_handle = CreateEvent(NULL,FALSE,FALSE,NULL);
   if(open_handle==NULL){
       MessageBox(NULL,L"Critical error: CreateEvent() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)thread_procedure,(void*)hWnd,0,NULL);
   if(thread==NULL){
      CloseHandle(open_handle);
      MessageBox(NULL,L"Critical error: CreateThread() failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   init_menu_bar(hWnd);
   init_login_menu(hWnd);
   if(GetModuleFileName(NULL,alertsound,IRC_SIZE_LITTLE)!=0){
      wcscpy(wcsrchr(alertsound,92)+1,L"alert.wav");
   }
   wchar_t configbuffer[IRC_SIZE_MEDIUM];
   if(GetModuleFileName(NULL,configbuffer,IRC_SIZE_MEDIUM)!=0){
      wcscpy(wcsrchr(configbuffer,92)+1,L"options.ini");
   }
   WideCharToMultiByte(CP_UTF8,0,configbuffer,-1,configfile,IRC_SIZE_MEDIUM,NULL,NULL);
   if(irc_and_ircconfig_init(&irc,&config,configfile)!=0){
      MessageBox(NULL,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   return 0;
}

void destroy(){
   open_state = 0;
   //CloseHandle(open_handle);
   //CloseHandle(connected_handle);
   //destroy_login_menu();
   //destroy_menu_bar();
   //join thread...

   //irc_destroy(&irc);
   //ircconfig_destroy(&config);
}

int connecting(HWND hWnd){
   if(connected!=0){
      return -1;
   }
   if(irc_and_ircconfig_init(&irc,&config,configfile)!=0){
      MessageBox(NULL,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   destroy_login_menu(hWnd);
   init_loading_screen(hWnd);
   if(irc_connect(&irc)<0){
      destroy_loading_screen(hWnd);
      init_login_menu(hWnd);
      return -1;
   }
   destroy_loading_screen(hWnd);
   init_chat_screen(hWnd);
   tab_create(hWnd,hWnd_TabControlChat,L".status",STATUS);
   connected = 1;
   SetEvent(open_handle);
   return 0;
}

int reconnecting(HWND hWnd){
   if(connected!=1){
      return -1;
   }
   irc_disconnect(&irc,NULL);
   tab_disconnect(hWnd_TabControlChat);
   if(irc_and_ircconfig_init(&irc,&config,configfile)!=0){
      MessageBox(NULL,L"Config file is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      return -1;
   }
   init_loading_screen(hWnd);
   int trys = config.reconnect;
   int sleep = IRC_RECONNECT_TIMEOUT_START;
   while(irc_connect(&irc)<0){
      Sleep(sleep);
      sleep *= IRC_RECONNECT_TIMEOUT_MULTIPLIER;
      trys--;
      if(trys==0){
         destroy_loading_screen(hWnd);
         connected = 0;
         //irc_destroy(&irc);
         //ircconfig_destroy(&config);
         while(tab_delete_actual(hWnd_TabControlChat)!=-1);
         destroy_chat_screen(hWnd);
         init_login_menu(hWnd);
         return -1;
      }
   }
   tab_connect(hWnd_TabControlChat);
   destroy_loading_screen(hWnd);
   SetEvent(open_handle);
   return 0;
}

void disconnecting(HWND hWnd){
   if(connected==1){
      connected = 0;
      irc_disconnect(&irc,NULL);
      //irc_destroy(&irc);
      //ircconfig_destroy(&config);
      while(tab_delete_actual(hWnd_TabControlChat)!=-1);
      destroy_chat_screen(hWnd);
      init_login_menu(hWnd);
   }
}
