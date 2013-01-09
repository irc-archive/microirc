/*
* irc.h
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Jos� Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <richedit.h>

#include "resource.h"

#include "../util/util.h"
#include "../buffer/buffer.h"
#include "../network/network.h"
#include "../list/list.h"
#include "../iniparser/iniparser.h"
#include "../ircprotocol/ircprotocol.h"

//#define CLIENT_ONLY

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

#define EDITCHATVIEWTEXT_LIMIT 65536
#define EDITCHATINPUT_LIMIT 256

#define SCROLL_PREFERENCES_MIN_POSITIONS 1
#define SCROLL_PREFERENCES_MAX_POSITIONS 10
#define SCROLL_PREFERENCES_HEIGHT 365

#define HIDPI 96

typedef struct config_t{
    HINSTANCE h_instance;
    wchar_t window_class[IRC_SIZE_SMALL];
    wchar_t window_title[IRC_SIZE_SMALL];
    wchar_t module_path[IRC_SIZE_SMALL];
    
    int text_color;
    int background_color;
    unsigned int LOG_PIXELS_X;
    unsigned int LOG_PIXELS_Y;
}config_t;

typedef struct ircconfig_t{
    int connect_on_startup;
    int reconnect_retries;
    char part[IRC_SIZE_SMALL];
    char kick[IRC_SIZE_SMALL];
    char quit[IRC_SIZE_SMALL];
    int encoding;
    int bubble;
    int sounds;
    int led_number;
    int led_interval;
}ircconfig_t;//move to irconfig

typedef struct guiclient_t{

}guiclient_t;

typedef struct guimanager_t{
    HWND connect_handles[IRC_PROFILE_LIMIT];
    int connect_size;
}guimanager_t;

//global
extern config_t g_config;

//client
LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void *receiverThreadProc(void *window_handle);
int guiclient_init(HWND hWnd);
void guiclient_destroy(HWND hWnd);
int guiclient_connecting(HWND hWnd);
int guiclient_reconnecting(HWND hWnd);
void guiclient_disconnecting(HWND hWnd);

//manager
LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int guimanager_init(HWND hWnd);
void guimanager_destroy();


//function
int update_title(wchar_t *window_title, wchar_t *cmd_line);
int get_screen_caps_x();
int get_screen_caps_y();
inline int HIDPIMulDiv(int x, int y, int z);
inline int SCALEX(int argX);
inline int SCALEY(int argY);
inline int UNSCALEX(int argX);
inline int UNSCALEY(int argY);

LRESULT CALLBACK WindowProcClient(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
