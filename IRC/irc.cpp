/*
* irc.cpp
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Jos� Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#pragma comment(lib, "aygshell.lib")
#pragma comment(lib, "commctrl.lib")
#pragma comment(lib, "richink.lib")
#pragma comment(lib, "ws2.lib")
#pragma comment(lib, "mmtimer.lib")

#include <windows.h>
#include <windowsx.h>
#include <Windef.h>
#include <time.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <aygshell.h>
#include <commctrl.h>
#include <richink.h>
#include <ceconfig.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <nled.h>

#include "resource.h"
#include "irc.h"
#include "../util/util.h"
#include "../buffer/buffer.h"
#include "../network/network.h"
#include "../list/list.h"
#include "../iniparser/iniparser.h"
#include "../ircprotocol/ircprotocol.h"

unsigned int BORDER;
unsigned int CLOSETAB_WIDTH;
unsigned int CLOSETAB_HEIGHT;
unsigned int BUTTONCHAT_WIDTH;
unsigned int BUTTONCHAT_HEIGHT;
unsigned int TABCONTROLCHAT_WIDTH;
unsigned int TABCONTROLCHAT_HEIGHT;
unsigned int EDITCHAT_WIDTH;
unsigned int EDITCHAT_HEIGHT;
unsigned int TABALL_HEIGHT;
unsigned int TABTALK_STATUS_WIDTH;
unsigned int TABNICK_CHAT_WIDTH;
unsigned int TABTALK_CHAT_WIDTH;
unsigned int MANAGER_RADIO_WIDTH;
unsigned int MANAGER_RADIO_HEIGHT;

unsigned int CLOSETAB_TOP;
unsigned int CLOSETAB_LEFT;
unsigned int BUTTONCHAT_TOP;
unsigned int BUTTONCHAT_LEFT;
unsigned int TABCONTROLCHAT_TOP;
unsigned int TABCONTROLCHAT_LEFT;
unsigned int EDITCHAT_TOP;
unsigned int EDITCHAT_LEFT;
unsigned int TABTALK_TOP;
unsigned int TABTALK_LEFT;
unsigned int TABNICK_TOP;
unsigned int TABNICK_LEFT;
unsigned int MANAGER_RADIO_TOP_DISTANCE;
unsigned int MANAGER_RADIO_TOP;
unsigned int MANAGER_RADIO_LEFT;

HINSTANCE app_instance;
HWND menu_bar_handle;
wchar_t window_title[IRC_SIZE_SMALL];
wchar_t module_path[IRC_SIZE_SMALL];

wchar_t profile[IRC_SIZE_SMALL];
wchar_t sound_alert[IRC_SIZE_SMALL];
HWND tabcontrol_chatview_handle;
HWND button_closetab_handle;
HWND button_chatsend_handle;
HWND edit_chatinput_handle;
HWND sippref_handle;
HCURSOR loadcursor_icon;
MMRESULT timer_led;
HANDLE receiver_thread;
HANDLE receiver_thread_event;
int receiver_active;
irc_t irc;
ircconfig_t config;
int connected;

guimanager_t manager;

#include "functions.h"
#include "ircconfig.h"
#include "tab_manager.h"
#include "dialogs_functions.h"
#include "gui_functions.h"
#include "client.h"
#include "manager.h"

//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
   app_instance = hInstance;
   wchar_t window_class[IRC_SIZE_SMALL];
   LoadString(hInstance, IDS_WNDCLASS_IRC, window_class, IRC_SIZE_SMALL);
   LoadString(hInstance, IDS_APP_TITLE, window_title, IRC_SIZE_SMALL);
   if(GetModuleFileName(NULL,module_path,IRC_SIZE_SMALL)==0){
      return 0;
   }
   if(title(window_title,lpCmdLine)!=0){
   //if(title(window_title,L"Client.ini")!=0){
      return 0;
   }
   if(FindWindow(window_class, window_title)!=NULL){
      return 0;
   }
   INITCOMMONCONTROLSEX icex;
   memset(&icex, 0, sizeof(INITCOMMONCONTROLSEX));
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_BAR_CLASSES|ICC_TAB_CLASSES;
   InitCommonControlsEx(&icex);
   InitRichInkDLL();
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
      wc.lpfnWndProc = WindowProcManager;//WindowProcManager;
   }else{
      wc.lpfnWndProc = WindowProcClient;
   }
   if(RegisterClass(&wc)==0){
      return 0;
   }
   HWND hWnd_Main = CreateWindowEx(0, window_class, window_title, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, lpCmdLine);
   //HWND hWnd_Main = CreateWindowEx(0, window_class, window_title, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, L"personalized.ini");
   if(hWnd_Main==NULL){
      return 0;
   }
   ShowWindow(hWnd_Main, nCmdShow);
   UpdateWindow(hWnd_Main);
   SendMessage(hWnd_Main, WM_CREATE_AFTER, 0, 0);
   MSG msg;
   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRC));
   while(GetMessage(&msg, NULL, 0, 0)){
      if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }
   return (int)msg.wParam;
}
