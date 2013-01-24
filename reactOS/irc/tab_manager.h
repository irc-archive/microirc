/*
* tab_manager.h
*
* This file contains an implementation of a tab manager to use on irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

WNDPROC old_ChatViewTextProc;
LRESULT CALLBACK ChatViewTextProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WNDPROC old_ChatViewNickProc;
LRESULT CALLBACK ChatViewNickProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#include "tab_manager_proc.h"

LRESULT CALLBACK ChatViewTextProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_LBUTTONUP:{
         SetFocus(client.edit_chatinput_handle);
         break;
      }
      case EM_SCROLLCARET:{
         SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, 0);
         break;
      }
      case WM_KEYDOWN:{
         /* Up and down arrow keys, just move the cursor inside richedit, we want to move one unit of scroll */
         if(wParam == VK_UP){
            tab_t* tab;
            tab_get_parameters_current(client.tabcontrol_chatview_handle,&tab);
            SendMessage(tab->text, WM_VSCROLL, SB_LINEUP, 0);
         }else if(wParam == VK_DOWN){
            tab_t* tab;
            tab_get_parameters_current(client.tabcontrol_chatview_handle,&tab);
            SendMessage(tab->text, WM_VSCROLL, SB_LINEDOWN, 0);
         }
      }
   }
   return CallWindowProc(old_ChatViewTextProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ChatViewNickProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_COMMAND:{
         //int wmEvent = HIWORD(wParam);
         wchar_t wnick[IRC_SIZE_SMALL];
         wchar_t wchannel[IRC_SIZE_SMALL];
         char nick[IRC_SIZE_SMALL];
         char channel[IRC_SIZE_SMALL];
         switch (LOWORD(wParam)){
            case IDM_CHATBOX_KICK:{
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(client.config.encoding,0,wnick,-1,nick,IRC_SIZE_SMALL,NULL,NULL);
               char *nick_ptr = irc_get_nick(&client.irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(client.tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               char *send[3] = {channel,nick_ptr,client.config.kick};
               irc_send_message(&client.irc,SEND_KICK,send,3);
               break;
            }
            case IDM_CHATBOX_KICKBAN:{
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(client.config.encoding,0,wnick,-1,nick,IRC_SIZE_SMALL,NULL,NULL);
               char *nick_ptr = irc_get_nick(&client.irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(client.tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               char *send[6] = {channel,nick_ptr,client.config.kick,channel,"+b",nick_ptr};
               irc_send_message(&client.irc,SEND_KICK,send,3);
               irc_send_message(&client.irc,SEND_CHANNEL_MODE,&send[3],3);
               break;
            }
            case IDM_CHATBOX_BAN:{
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(client.config.encoding,0,wnick,-1,nick,IRC_SIZE_SMALL,NULL,NULL);
               char *nick_ptr = irc_get_nick(&client.irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               tab_get_name_current(client.tabcontrol_chatview_handle,wchannel,IRC_SIZE_SMALL);
               WideCharToMultiByte(client.config.encoding,0,wchannel,-1,channel,IRC_SIZE_SMALL,NULL,NULL);
               char *send[3] = {channel,"+b",nick_ptr};
               irc_send_message(&client.irc,SEND_CHANNEL_MODE,send,3);
               break;
            }
            /*case IDM_CHATBOX_WHOIS:{
               MessageBox(NULL,L"not done",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }*/
            case IDM_CHATBOX_COPYNICK:{
               int element = ListBox_GetCurSel(hWnd);
               ListBox_GetText(hWnd,element,wnick);
               if(wcslen(wnick)<1){
                  break;
               }
               WideCharToMultiByte(client.config.encoding,0,wnick,-1,nick,IRC_SIZE_SMALL,NULL,NULL);
               char *nick_ptr = irc_get_nick(&client.irc,nick);
               if(nick_ptr==NULL){
                  break;
               }
               MultiByteToWideChar(client.config.encoding,0,nick_ptr,-1,wnick,IRC_SIZE_SMALL);
               wchar_t wtext[IRC_SIZE_MEDIUM];
               Edit_GetText(client.edit_chatinput_handle,wtext,IRC_SIZE_MEDIUM);
               wcscat(wtext,wnick);
               wcscat(wtext,L" ");
               Edit_SetText(client.edit_chatinput_handle,wtext);
               element = Edit_GetTextLength(client.edit_chatinput_handle);
               SendMessage(client.edit_chatinput_handle, EM_SETSEL, element, element);
               SetFocus(client.edit_chatinput_handle);
               break;
            }
         }
         break;
      }
      case WM_RBUTTONDOWN:{
        HMENU menu = LoadMenu(config.h_instance, MAKEINTRESOURCE(IDR_CHATBOX_MENU));
        menu = GetSubMenu(menu, 0);
        TrackPopupMenuEx(menu,TPM_LEFTALIGN,resize.TABNICK_LEFT+LOWORD(lParam),resize.TABNICK_TOP+HIWORD(lParam),hWnd,NULL);
         break;
      }
   }
   return CallWindowProc(old_ChatViewNickProc, hWnd, uMsg, wParam, lParam);
}
