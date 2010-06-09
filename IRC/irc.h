/*
* irc.h
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#define IRC_SIZE_LITTLE 256
#define IRC_SIZE_MEDIUM IRC_BUFFER_SIZE_MEDIUM*2

#define IRC_MAX_NICKS_PER_MESSAGE 100

#define IRC_RECONNECT_TIMEOUT_START 3000
#define IRC_RECONNECT_TIMEOUT_MULTIPLIER 2

#define WM_CREATE_TAB WM_USER+10
#define WM_DESTROY_TAB WM_USER+11
#define WM_LOAD_CURSOR WM_USER+12
#define WM_UNLOAD_CURSOR WM_USER+13
#define WM_DISCONNECTING WM_USER+14
#define WM_CONNECTING WM_USER+15
#define WM_RECONNECTING WM_USER+16

#define ICON TEXT("IRC.ico")
#define TEXT_TITLE TEXT("IRC v1.0")
#define TEXT_WINDOW_CLASS TEXT("irc application")
#define TEXT_CANNOT_CONNECT TEXT("Cannot connect to the server.")

#define BUTTON_CONNECT 200
#define BUTTON_SEND 201
#define TEXT_SEND 202
#define TAB_CONTROL 203
#define BUTTON_CANCEL 207

#define BUTTON_CLOSE 205
#define LIST_BOX 206

#define DELETE_TEXT_MARGIN 2048

int init(HWND);
void destroy();
int connecting(HWND);
void disconnecting(HWND);
int reconnecting(HWND);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
char *tokens_required(char*, char, int, char**, int*);

char *strstri(char *t, char *s){
   int i, j;
   for(i=0; t[i] != '\0'; i++){
      for(j=0; s[j] != '\0'; j++){
         if(toupper(s[j])==toupper(t[i+j])){
            continue; 
         }else{
            break;
         }
      }
      if (s[j] == '\0'){
         break;
      }
   }
   if (s[j] == '\0'){
      return (i+t);
   }else{
      return '\0';
   }
}
