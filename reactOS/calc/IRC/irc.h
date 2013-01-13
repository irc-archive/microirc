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
    HMENU menu_bar_handle;
    
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
}ircconfig_t;

typedef struct guiclient_t{
	wchar_t profile[IRC_SIZE_SMALL];
	wchar_t sound_alert[IRC_SIZE_SMALL];
	HWND tabcontrol_chatview_handle;
	HWND button_closetab_handle;
	HWND button_chatsend_handle;
	HWND edit_chatinput_handle;
	HCURSOR loadcursor_icon;
	MMRESULT timer_led;
	HANDLE receiver_thread;
	HANDLE receiver_thread_event;
	int receiver_active;
	irc_t irc;
	ircconfig_t config;
	int connected;
}guiclient_t;

typedef struct guimanager_t{
    HWND connect_handles[IRC_PROFILE_LIMIT];
    int connect_size;
	HWND static_label1_handle;
	HWND static_label2_handle;
}guimanager_t;

typedef struct resize_t{
	//client
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

	//manager
	unsigned int STATIC_WIDTH;
	unsigned int STATIC_HEIGHT;
	unsigned int RADIO_WIDTH;
	unsigned int RADIO_HEIGHT;

	unsigned int STATIC_TOP;
	unsigned int STATIC_LEFT;
	unsigned int STATIC_LEFT_SECOND;
	unsigned int RADIO_TOP_DISTANCE;
	unsigned int RADIO_TOP;
	unsigned int RADIO_LEFT;
}resize_t;

//global
extern config_t g_config;
extern resize_t resize;
extern guiclient_t client;
extern guimanager_t manager;

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
