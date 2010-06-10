/*
* irc.cpp
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Josó Diogo Reis <diogoandre12@gmail.com>
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

HCURSOR hOldCursor;

#include "resource.h"
#include "irc.h"
#include "buffer.h"
#include "network.h"
#include "list.h"
#include "iniparser.h"
#include "ircprotocol.h"
#include "tab_manager.h"
#include "gui_functions.h"

HANDLE thread;

HANDLE open_handle;
volatile int open_state;

irc_t irc;
volatile int connected;
volatile int encoding;

wchar_t alertsound[IRC_SIZE_LITTLE];
wchar_t configfile[IRC_SIZE_LITTLE];

wchar_t wdestination[IRC_SIZE_LITTLE];
char tdestination[IRC_SIZE_LITTLE];
wchar_t wnick[IRC_SIZE_LITTLE];

wchar_t wtextprocess[IRC_SIZE_MEDIUM];
char ttextprocess[IRC_SIZE_MEDIUM];
wchar_t wwritebuffer[IRC_SIZE_MEDIUM];

//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
   MSG msg;
   hInstance_Main = hInstance;
   SHInitExtraControls();
   HWND hWnd_Main = FindWindow(L"irc application", L"IRC v1.0");   
   if(hWnd_Main){
      SetForegroundWindow((HWND)((ULONG) hWnd_Main | 0x00000001));
      return 0;
   } 
   WNDCLASS wc;
   wc.style = CS_HREDRAW|CS_VREDRAW;
   wc.lpfnWndProc = WindowProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
   wc.hCursor = 0;
   wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName = 0;
   wc.lpszClassName = L"irc application";
   if(!RegisterClass(&wc)){
      return FALSE;
   }
   hWnd_Main = CreateWindowEx(0,L"irc application", L"IRC v1.0", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, NULL);
   if(!hWnd_Main){
      return 0;
   }
   ShowWindow(hWnd_Main, nCmdShow);
   UpdateWindow(hWnd_Main);
   /*InitCommonControls();
   if(SHInitExtraControls()==FALSE){
      return 0;
   }
   if(CreateWindowEx(0,WC_SIPPREF, L"", WS_CHILD, 0, 0, 0, 0, hWnd_Main, (HMENU)0, hInstance_Main, NULL)==NULL){
      return 0;
   }*/
   //HACCEL hAccelTable;
   //hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEWONE));
   while (GetMessage(&msg, NULL, 0, 0)){
      //if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      //}
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
            MessageBox(NULL,L"Connecting failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
         }
         break;
      }
      case WM_DISCONNECTING:{
         disconnecting(hWnd);
         break;
      }
      case WM_RECONNECTING:{
         if(reconnecting(hWnd)!=0){
            MessageBox(NULL,L"Reconnecting failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
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
         switch (LOWORD(element_id)){
            case BUTTON_CONNECT:{
               if(connecting(hWnd)!=0){
                  MessageBox(NULL,L"Connecting failed.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               }
               break;
            }
            case LIST_BOX:{
               if(wmEvent==LBN_DBLCLK){
                  HWND listbox = (HWND)param_id;
                  int element = ListBox_GetCurSel(listbox);
                  ListBox_GetText(listbox,element,wtextprocess);
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wtextprocess);
                  tab_select_name(hWnd_TabControlChat,wtextprocess);
               }
               break;
            }
            case BUTTON_CLOSE:{
               char *send[1];
               send[0]=ttextprocess;
               tab_get_name_actual(hWnd_TabControlChat,wtextprocess,IRC_SIZE_MEDIUM);
               WideCharToMultiByte(encoding,0,wtextprocess,-1,ttextprocess,IRC_SIZE_MEDIUM,NULL,NULL);
               if(memcmp(ttextprocess,".status",7)!=0){
                  if(memcmp(ttextprocess,"#",1)==0){
                     irc_send_message(&irc,SEND_PART,send,1);
                  }
                  tab_delete_actual(hWnd_TabControlChat);
               }else{
                  switch (MessageBox(hWnd,L"Do you realy want to quit?",L"QUIT",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
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
                  WideCharToMultiByte(encoding,0,wtextprocess,-1,ttextprocess,IRC_SIZE_MEDIUM,NULL,NULL);
                  tab_get_name_actual(hWnd_TabControlChat,wdestination,IRC_SIZE_LITTLE);
                  WideCharToMultiByte(encoding,0,wdestination,-1,tdestination,IRC_SIZE_LITTLE,NULL,NULL);
                  send[0]=tdestination;
                  MultiByteToWideChar(encoding,0,irc.nick,-1,wnick,IRC_SIZE_LITTLE);
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
            /*case IDM_HELP_ABOUT:
            DialogBox(g_hInstanceMain, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
            break;
            case IDM_OK:
            SendMessage (hWnd, WM_CLOSE, 0, 0);				
            break;*/
         }
         break;
      }
      case WM_CREATE:{
         init(hWnd);
         break;
      }
      case WM_DESTROY:{
         destroy();
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
                  MultiByteToWideChar(encoding,0,tchar_buffer[4],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s",timestamp,wchar_buffer[0]);
                  tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
                  break;
               }
               tchar_buffer[4]+=7;
            }
            case RECV_PRIVMSG:{//nick user host destination mensagem
               if(tchar_size<5){
                  break;
               }
               if(strstri(tchar_buffer[4],irc.nick)!=NULL && irc.sounds!=0){
                  PlaySound(alertsound,NULL,SND_ASYNC|SND_FILENAME);
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               if(recv_result==RECV_CTCP){
                  swprintf(wchar_buffer[4],L"\r\n%s %s %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }else{
                  swprintf(wchar_buffer[4],L"\r\n%s %s: %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }
               if(tchar_buffer[3][0]=='#'){
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)wchar_buffer[1]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               }else{
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wchar_buffer[0]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[0],wchar_buffer[4],TEXT,APPEND);
               }
               break;
            }
            case RECV_NOTICE:{
               if(tchar_size<5){
                  break;
               }
               if(strstri(tchar_buffer[4],irc.nick)!=NULL && irc.sounds!=0){
                  PlaySound(alertsound,NULL,SND_ASYNC|SND_FILENAME);
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               swprintf(wchar_buffer[4],L"\r\n%s %s: %s",timestamp,wchar_buffer[0],wchar_buffer[2]);
               if(tchar_buffer[3][0]=='#'){
                  SendMessage(hWnd,WM_CREATE_TAB,STATUS,(LPARAM)wchar_buffer[1]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               }else{
                  tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
               }
               break;
            }
            case RECV_NICK:{//nick user host newnick
               if(tchar_size<4){
                  break;
               }
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  sprintf(irc.nick,"%s",tchar_buffer[3]);
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               tab_nickchange(hWnd_TabControlChat,timestamp,wchar_buffer[0],wchar_buffer[1]);
               break;
            }
            case RECV_JOIN:{//nick user host canal
               if(tchar_size<4){
                  break;
               }
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  SendMessage(hWnd,WM_CREATE_TAB,CHAT,(LPARAM)wchar_buffer[1]);
               }else{
                  MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
                  MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s joined",timestamp,wchar_buffer[0]);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[0],NICK,APPEND);
               }
               break;
            }
            case RECV_PART:{//nick user host canal mensagem
               if(tchar_size<4){
                  break;
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               if(strcmp(tchar_buffer[0],irc.nick)==0){
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],NULL,NICK,REMOVE);
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               if(tchar_size==4){
                  swprintf(wchar_buffer[4],L"\r\n%s %s parted",timestamp,wchar_buffer[0]);
               }else{
                  MultiByteToWideChar(encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s parted (%s)",timestamp,wchar_buffer[0],wchar_buffer[2]);
               }
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[0],NICK,REMOVE);
               break;
            }
            case RECV_QUIT:{//nick user host mensagem
               if(tchar_size<4){
                  break;
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               if(tchar_size==3){
                  tab_quit(hWnd_TabControlChat,timestamp,wchar_buffer[0],L"");
               }else{
                  MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  tab_quit(hWnd_TabControlChat,timestamp,wchar_buffer[0],wchar_buffer[1]);
               }
               break;
            }
            case RECV_KICK:{//nick user host canal nick_vitima mensagem
               if(tchar_size<6){
                  break;
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[3],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
               if(strcmp(tchar_buffer[4],irc.nick)==0){
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[1],NULL,NICK,REMOVE);
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[0],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               MultiByteToWideChar(encoding,0,tchar_buffer[4],-1,wchar_buffer[2],IRC_SIZE_MEDIUM);
               if(tchar_size==5){
                  swprintf(wchar_buffer[4],L"\r\n%s %s kicked by %s",timestamp,wchar_buffer[2],wchar_buffer[0]);
               }else{
                  MultiByteToWideChar(encoding,0,tchar_buffer[5],-1,wchar_buffer[3],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4],L"\r\n%s %s kicked by %s (%s)",timestamp,wchar_buffer[2],wchar_buffer[0],wchar_buffer[3]);
               }
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[4],TEXT,APPEND);
               tab_write_name(hWnd_TabControlChat,wchar_buffer[1],wchar_buffer[2],NICK,REMOVE);
               break;
            }
            case RECV_NICK_LIST:{//host canal nicks
               if(tchar_size<3){
                  break;
               }
               MultiByteToWideChar(encoding,0,tchar_buffer[1],-1,wchar_buffer[0],IRC_SIZE_MEDIUM);
               char *nicks = tchar_buffer[2];
               char *d_tokens[IRC_MAX_NICKS_PER_MESSAGE];
               int s_tokens=0;
               int i;
               tokens_required(nicks,CHAR_SPACE,IRC_MAX_NICKS_PER_MESSAGE,d_tokens,&s_tokens);
               for(i=0;i<s_tokens;i++){
                  MultiByteToWideChar(encoding,0,d_tokens[i],-1,wchar_buffer[1],IRC_SIZE_MEDIUM);
                  tab_write_name(hWnd_TabControlChat,wchar_buffer[0],wchar_buffer[1],NICK,APPEND);
               }
               break;
            }
            case RECV_NICK_TAKEN:{//host actualnick failednick message
               swprintf(wchar_buffer[4],L"\r\n%s nick already in use",timestamp);
               tab_write_actual(hWnd_TabControlChat,wchar_buffer[4],TEXT,APPEND);
               break;
            }
            case RECV_OTHER:{//host message
               if(tchar_size<2){
                  break;
               }
               swprintf(wchar_buffer[4],L"\r\n%s",timestamp);
               int i;
               for(i=0;i<tchar_size;i++){
                  MultiByteToWideChar(encoding,0,tchar_buffer[i],-1,wchar_buffer[i],IRC_SIZE_MEDIUM);
                  swprintf(wchar_buffer[4]+wcslen(wchar_buffer[4]),L" %s",wchar_buffer[i]);
               }
               tab_write_name(hWnd_TabControlChat,L".status",wchar_buffer[4],TEXT,APPEND);
               break;
            }
         }
      }
      if(connected==1){
         if(irc.reconnect==0){
            switch (MessageBox(NULL,L"Do you realy want to reconnect?",L"DISCONNECTED",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
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
      return -1;
   }
   thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)thread_procedure,(void*)hWnd,0,NULL);
   if(thread==NULL){
      CloseHandle(open_handle);
      return -1;
   }
   init_menu_bar(hWnd);
   init_login_menu(hWnd);
   if(GetModuleFileName(NULL,alertsound,IRC_SIZE_LITTLE)!=0){
      wcscpy(wcsrchr(alertsound,92)+1,L"alert.wav");
   }
   if(GetModuleFileName(NULL,configfile,IRC_SIZE_LITTLE)!=0){
      wcscpy(wcsrchr(configfile,92)+1,L"options.ini");
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
   PostQuitMessage(0);
}

int connecting(HWND hWnd){
   if(connected!=0){
      return -1;
   }
   char configbuffer[IRC_SIZE_MEDIUM];
   WideCharToMultiByte(encoding,0,configfile,-1,configbuffer,IRC_SIZE_MEDIUM,NULL,NULL);
   if(irc_init_from_ini(&irc,configbuffer)!=0){
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
   if(irc.encoding == ENCODING_UTF8){
      encoding = CP_UTF8;
   }else if(irc.encoding == ENCODING_ISO8859){
      encoding = CP_ACP;
   }
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
   init_loading_screen(hWnd);
   int trys = irc.reconnect;
   int sleep = IRC_RECONNECT_TIMEOUT_START;
   while(irc_connect(&irc)<0){
      Sleep(sleep);
      sleep *= IRC_RECONNECT_TIMEOUT_MULTIPLIER;
      trys--;
      if(trys==0){
         destroy_loading_screen(hWnd);
         connected = 0;
         irc_destroy(&irc);
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
      irc_destroy(&irc);
      while(tab_delete_actual(hWnd_TabControlChat)!=-1);
      destroy_chat_screen(hWnd);
      init_login_menu(hWnd);
   }
}
