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
#define IRC_PROFILE_LIMIT 8

#define IRC_FILE_PATH_ENCODING CP_ACP
#define IRC_CONFIG_FILE_ENCODING CP_UTF8

#define WM_CREATE_TAB WM_USER+10
#define WM_DESTROY_TAB WM_USER+11
#define WM_LOAD_CURSOR WM_USER+12
#define WM_UNLOAD_CURSOR WM_USER+13
#define WM_DISCONNECTING WM_USER+14
#define WM_CONNECTING WM_USER+15
#define WM_RECONNECTING WM_USER+16

#define BUTTON_CLOSETAB 200
#define BUTTON_CHATSEND 201
#define TABCONTROL_CHATVIEW 202
#define EDIT_CHATVIEW_TEXT 204
#define LIST_CHATVIEW_NICK 205
#define BUBBLE_NOTIFICATION 206

#define EDITCHATVIEWTEXT_DELETE 2048
#define EDITCHATINPUT_LIMIT 256

#define SCROLL_PREFERENCES_MIN_POSITIONS 1
#define SCROLL_PREFERENCES_MAX_POSITIONS 8
#define SCROLL_PREFERENCES_HEIGHT 337

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

typedef struct guimanager_t{
   HWND connect_handles[IRC_PROFILE_LIMIT];
   int connect_size;
}guimanager_t;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void *receiverThreadProc(void *window_handle);
int guiclient_init(HWND hWnd);
void guiclient_destroy(HWND hWnd);
int guiclient_connecting(HWND hWnd);
int guiclient_reconnecting(HWND hWnd);
void guiclient_disconnecting(HWND hWnd);
int title(wchar_t *window_title, wchar_t *cmd_line);
int guimanager_init(HWND hWnd);
void guimanager_destroy();
int guimanager_create(wchar_t *text, HWND hWnd);
void guimanager_getselected(int *d_result, int *s_result);
int guimanager_delete(int index);
void guimanager_resize_all();



const int HIDPI = 96;

int GetScreenCapsX(){
   HDC hDC = GetDC(NULL);
   if(hDC == NULL){
      return -1;
   }
   int i = GetDeviceCaps(hDC, LOGPIXELSX);
   ReleaseDC(NULL, hDC);
   return i;
}

int GetScreenCapsY(){
   HDC hDC = GetDC(NULL);
   if(hDC == NULL){
      return -1;
   }
   int i = GetDeviceCaps(hDC, LOGPIXELSY);
   ReleaseDC(NULL, hDC);
   return i;
}

inline int HIDPISIGN(int x){
   return (((x)<0)?-1:1);
}

inline int HIDPIMulDiv(int x, int y, int z){
   return ((((abs(x)*(y))+((z)>>1))/(z))*HIDPISIGN(x));
}

/* nLogPixels should be result of GetScreenCaps */
inline int SCALEX(int argX, int nLogPixelsX){
   return HIDPIMulDiv(argX, nLogPixelsX, HIDPI);
}

inline int SCALEY(int argY, int nLogPixelsY){
   return HIDPIMulDiv(argY, nLogPixelsY, HIDPI);
}

inline int UNSCALEX(int argX, int nLogPixelsX){
   return HIDPIMulDiv(argX, HIDPI, nLogPixelsX);
}

inline int UNSCALEY(int argY, int nLogPixelsY){
   return HIDPIMulDiv(argY, HIDPI, nLogPixelsY);
}
