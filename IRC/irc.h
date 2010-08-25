/*
* irc.h
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#define IRC_SIZE_SMALL 256
#define IRC_SIZE_MEDIUM 1024

#define IRC_RECONNECT_TIMEOUT 7500

#define IRC_CONFIG_FILE_ENCODING CP_UTF8

#define WM_CREATE_TAB WM_USER+10
#define WM_DESTROY_TAB WM_USER+11
#define WM_LOAD_CURSOR WM_USER+12
#define WM_UNLOAD_CURSOR WM_USER+13
#define WM_DISCONNECTING WM_USER+14
#define WM_CONNECTING WM_USER+15
#define WM_RECONNECTING WM_USER+16

#define BUTTON_CHATSEND 201
#define TABCONTROL_CHATVIEW 203
#define BUTTON_CLOSETAB 204
#define EDIT_CHATVIEW_TEXT 205
#define LIST_CHATVIEW_NICK 206
#define BUBBLE_NOTIFICATION 208

#define EDITCHATVIEWTEXT_DELETE 2048
#define EDITCHATINPUT_LIMIT 256

#define SCROLL_PREFERENCES_MIN_POSITIONS 1
#define SCROLL_PREFERENCES_MAX_POSITIONS 8
#define SCROLL_PREFERENCES_HEIGHT 320

#define BORDER 0.005 //this value can be changed and all the controls got automaticaly resized
#define STATICCONNECTING_WIDTH 1
#define STATICCONNECTING_HEIGHT 1
#define CLOSETAB_WIDTH 0.08
#define CLOSETAB_HEIGHT 0.08
#define TABCONTROLCHAT_WIDTH (1-(3*BORDER+CLOSETAB_WIDTH))
#define TABCONTROLCHAT_HEIGHT 0.08
#define TABALL_HEIGHT (1-(4*BORDER+TABCONTROLCHAT_HEIGHT+EDITCHAT_HEIGHT))
#define TABTALK_STATUS_WIDTH (1-(2*BORDER))
#define TABNICK_CHAT_WIDTH 0.25
#define TABTALK_CHAT_WIDTH (1-(3*BORDER+TABNICK_CHAT_WIDTH))
#define BUTTONCHAT_WIDTH 0.17
#define BUTTONCHAT_HEIGHT 0.08
#define EDITCHAT_WIDTH (1-(3*BORDER+BUTTONCHAT_WIDTH))
#define EDITCHAT_HEIGHT 0.08

#define STATICCONNECTING_TOP 0
#define STATICCONNECTING_LEFT 0
#define TABCONTROLCHAT_TOP BORDER
#define TABCONTROLCHAT_LEFT BORDER
#define CLOSETAB_TOP BORDER
#define CLOSETAB_LEFT (TABCONTROLCHAT_LEFT+TABCONTROLCHAT_WIDTH+BORDER)
#define TABTALK_TOP (TABCONTROLCHAT_TOP+TABCONTROLCHAT_HEIGHT+BORDER)
#define TABTALK_LEFT BORDER
#define TABNICK_TOP (TABCONTROLCHAT_TOP+TABCONTROLCHAT_HEIGHT+BORDER)
#define TABNICK_LEFT (TABTALK_LEFT+TABTALK_CHAT_WIDTH+BORDER)
#define EDITCHAT_TOP (TABTALK_TOP+TABALL_HEIGHT+BORDER)
#define EDITCHAT_LEFT BORDER
#define BUTTONCHAT_TOP (TABTALK_TOP+TABALL_HEIGHT+BORDER)
#define BUTTONCHAT_LEFT (EDITCHAT_LEFT+EDITCHAT_WIDTH+BORDER)

typedef struct ircconfig_t{
   int reconnect;
   char part[IRC_SIZE_SMALL];
   char kick[IRC_SIZE_SMALL];
   char quit[IRC_SIZE_SMALL];
   int encoding;
   int bubble;
   int sounds;
   int lednumber;
   int ledinterval;
}ircconfig_t;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void *receiverThreadProc(void *window_handle);
int guiclient_init(HWND hWnd);
void guiclient_destroy(HWND hWnd);
int guiclient_connecting(HWND hWnd);
int guiclient_reconnecting(HWND hWnd);
void guiclient_disconnecting(HWND hWnd);
