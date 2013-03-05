/*
* manager.h
*
* This file contains functions to use by manager.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include "manager_proc.h"

LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_COMMAND:{
         //int wmEvent = HIWORD(wParam);
         //HWND control_handler = (HWND)lParam;
         wchar_t wprofile_name[IRC_SIZE_SMALL];
         switch (LOWORD(wParam)){
            case IDM_NEW:{
               if(open_input_box(hWnd, MAKEINTSTR(IDS_MSG4), L"", wprofile_name, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wprofile_name)==0 || validate_name(wprofile_name)!=0){
                  MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG12),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }
               wcscat(wprofile_name,IRC_CONST_INI);

               int result = checkbox_create(wprofile_name,hWnd);
               if(result==-1){
                  MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG13),NULL,MB_ICONEXCLAMATION|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }else if(result==-2){
                  MessageBox(hWnd,MAKEINTSTR(IDS_ERROR_MSG14),NULL,MB_ICONEXCLAMATION|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }

               iniparser_t iniparser;
               if(iniparser_init(&iniparser)!=0){
                  break;
               }
               iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, IRC_CONF_HOST_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, IRC_CONF_PORT_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, IRC_CONF_PASS_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, IRC_CONF_USER_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, IRC_CONF_NAME_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, IRC_CONF_NICK_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, IRC_CONF_PERFORM_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, IRC_CONF_CHANNELS_VAL);
               iniparser_setint(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, IRC_CONF_DELAY_VAL);
               iniparser_setint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, IRC_CONF_STARTUP_VAL);
               iniparser_setint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, IRC_CONF_RETRIES_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, IRC_CONF_PART_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, IRC_CONF_KICK_VAL);
               iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, IRC_CONF_QUIT_VAL);
               iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, IRC_CONF_ENCODING_VAL);
               iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, IRC_CONF_BUBBLE_VAL);
               iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, IRC_CONF_SOUNDS_VAL);
               iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, IRC_CONF_LEDNUMBER_VAL);
               iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, IRC_CONF_LEDINTERVAL_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_BACKGROUND_COLOR, IRC_CONF_BACKGROUND_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_FONT_COLOR, IRC_CONF_FONT_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_JOIN_COLOR, IRC_CONF_JOIN_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_NICKCHANGE_COLOR, IRC_CONF_NICKCHANGE_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_TOPICCHANGE_COLOR, IRC_CONF_TOPICCHANGE_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_MODE_COLOR, IRC_CONF_MODE_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_PART_COLOR, IRC_CONF_PART_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_KICK_COLOR, IRC_CONF_KICK_COLOR_VAL);
                iniparser_setint(&iniparser, IRC_CONF_COLORS, IRC_CONF_QUIT_COLOR, IRC_CONF_QUIT_COLOR_VAL);

               if(winiparser_store(&iniparser,wprofile_name)!=0){
                  iniparser_destroy(&iniparser);
                  break;
               }
               iniparser_destroy(&iniparser);

               wchar_t *parameters[2]={IRC_CONST_MANAGER,wprofile_name};
               //if(!DialogBoxParam(config.h_instance, MAKEINTRESOURCE(IDD_PREFERENCES), hWnd, PreferencesProc, (LPARAM)parameters)){
               if(!OpenPreferencesDialog(hWnd, (LPARAM)parameters)){
                   MessageBox(NULL,L"WORKS",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
                  checkbox_delete(manager.connect_size-1);
               }
               break;
            }
            case IDM_EDIT:{
               int i;
               int s_index=0;
               int d_index[IRC_PROFILE_LIMIT];
               checkbox_getselected(d_index, &s_index);
               for(i=0;i<s_index;i++){
                  Button_GetText(manager.connect_handles[d_index[i]],wprofile_name,IRC_SIZE_SMALL);
                  wchar_t *parameters[2]={IRC_CONST_MANAGER,wprofile_name};
                  //DialogBoxParam(config.h_instance, MAKEINTRESOURCE(IDD_PREFERENCES), hWnd, PreferencesProc, (LPARAM)parameters);
                  OpenPreferencesDialog(hWnd, (LPARAM)parameters);
               }
               break;
            }
            case IDM_REMOVE:{
               switch(MessageBox(hWnd,MAKEINTSTR(IDS_QUEST_MSG5),MAKEINTSTR(IDS_QUEST_MSG6),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
                  case IDYES:{
                     int i;
                     int s_index=0;
                     int d_index[IRC_PROFILE_LIMIT];
                     checkbox_getselected(d_index, &s_index);
                     for(i=s_index-1;i>=0;i--){
                        checkbox_delete(d_index[i]);
                     }
                     break;
                  }
                  case IDNO:{
                     break;
                  }
               }
               break;
            }
            case IDM_LAUNCH:{
               int i;
               int s_index=0;
               int d_index[IRC_PROFILE_LIMIT];
               checkbox_getselected(d_index, &s_index);
               for(i=0;i<s_index;i++){
                  Button_GetText(manager.connect_handles[d_index[i]],wprofile_name,IRC_SIZE_SMALL);
                  //CreateProcess(module_path,wprofile_name,NULL,NULL,FALSE,INHERIT_CALLER_PRIORITY,NULL,NULL,NULL,NULL);
PROCESS_INFORMATION processInformation;
STARTUPINFO startupInfo;
memset(&processInformation, 0, sizeof(processInformation));
memset(&startupInfo, 0, sizeof(startupInfo));
startupInfo.cb = sizeof(startupInfo);

wchar_t aa[1000];
swprintf(aa, L"%s %s", config.module_path, wprofile_name);//, sizeof(wprofile_name),wcslen(wprofile_name));
//config.module_path,wprofile_name
CreateProcess(NULL,aa   ,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&startupInfo, &processInformation);

//WTF FIX ME

               }
               if(s_index>0){
                  SendMessage(hWnd,WM_CLOSE,0,0);
               }
               break;
            }
            case IDM_EXIT:{
               SendMessage(hWnd,WM_CLOSE,0,0);
               break;
            }
         }
         break;
      }
      case WM_SIZE:{
         config.LOG_PIXELS_X = get_screen_caps_x();
         config.LOG_PIXELS_Y = get_screen_caps_y();

         refresh_manager_sizes();

         MoveWindow(manager.static_label1_handle,resize.STATIC_LEFT,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,FALSE);
         MoveWindow(manager.static_label2_handle,resize.STATIC_LEFT_SECOND,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,FALSE);
         checkbox_resize_all();
         break;
      }
      case WM_ACTIVATE:{
         //SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         //SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         break;
      }
      case WM_CREATE:{
         config.LOG_PIXELS_X = get_screen_caps_x();
         config.LOG_PIXELS_Y = get_screen_caps_y();

         if(guimanager_init(hWnd)!=0){
            PostQuitMessage(0);
         }
         break;
      }
      case WM_QUIT:{
         //called on PostQuitMessage(0);
         break;
      }
      case WM_CLOSE:{
      }
      case WM_DESTROY:{
         guimanager_destroy();
         PostQuitMessage(0);
         break;
      }
   }
   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
