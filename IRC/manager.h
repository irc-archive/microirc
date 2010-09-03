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

LRESULT CALLBACK WindowProcManager(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   static SHACTIVATEINFO s_sai;
   switch(uMsg){
      case WM_COMMAND:{
         int wmEvent = HIWORD(wParam);
         HWND control_handler = (HWND)lParam;
         wchar_t wprofile_name[IRC_SIZE_SMALL];
         switch (LOWORD(wParam)){
            case IDM_NEW:{
               if(open_input_box(hWnd, L"New Profile", L"", wprofile_name, IRC_SIZE_SMALL)!=0){
                  break;
               }
               if(wcslen(wprofile_name)==0 || validate_name(wprofile_name)!=0){
                  MessageBox(hWnd,L"Invalid Profile Name.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }
               wcscat(wprofile_name,L".ini");

               iniparser_t iniparser;
               if(iniparser_init(&iniparser)!=0){
                  break;
               }
               iniparser_setstring(&iniparser, "server", "host", "chat.freenode.net");
               iniparser_setstring(&iniparser, "server", "port", "6667");
               iniparser_setstring(&iniparser, "client", "user", "user");
               iniparser_setstring(&iniparser, "client", "name", "Real Name");
               iniparser_setstring(&iniparser, "client", "nick", "change_me");
               iniparser_setstring(&iniparser, "client", "perform", "");
               iniparser_setstring(&iniparser, "autojoin", "channels", "#microirc");
               iniparser_setint(&iniparser, "autojoin", "delay", 5000);
               iniparser_setint(&iniparser, "autoreconnect", "retries", 5);
               iniparser_setstring(&iniparser, "messages", "part", "");
               iniparser_setstring(&iniparser, "messages", "kick", "");
               iniparser_setstring(&iniparser, "messages", "quit", "http://code.google.com/p/microirc/");
               iniparser_setint(&iniparser, "miscellaneous", "encoding", 1);
               iniparser_setint(&iniparser, "miscellaneous", "bubble", 0);
               iniparser_setint(&iniparser, "miscellaneous", "sounds", 0);
               iniparser_setint(&iniparser, "miscellaneous", "lednumber", -1);
               iniparser_setint(&iniparser, "miscellaneous", "ledinterval", 500);
               if(winiparser_store(&iniparser,wprofile_name)!=0){
                  iniparser_destroy(&iniparser);
                  break;
               }
               iniparser_destroy(&iniparser);

               guimanager_create(wprofile_name,hWnd);
               wchar_t *parameters[2]={L"manager",wprofile_name};
               DialogBoxParam(app_instance, (LPCTSTR)IDD_PREFERENCES, hWnd, PreferencesProc, (LPARAM)parameters);
               break;
            }
            case IDM_EDIT:{
               int d_index[10];
               int s_index=0;
               int i;
               guimanager_getselected(d_index, &s_index);
               for(i=0;i<s_index;i++){
                  Button_GetText(manager.connect_handles[d_index[i]],wprofile_name,IRC_SIZE_SMALL);
                  wchar_t *parameters[2]={L"manager",wprofile_name};
                  DialogBoxParam(app_instance, (LPCTSTR)IDD_PREFERENCES, hWnd, PreferencesProc, (LPARAM)parameters);
               }
               break;
            }
            case IDM_REMOVE:{
               wchar_t wprofile_fullpath[IRC_SIZE_SMALL];
               int d_index[10];
               int s_index=0;
               int i;
               guimanager_getselected(d_index, &s_index);
               for(i=s_index-1;i>=0;i--){
                  Button_GetText(manager.connect_handles[d_index[i]],wprofile_name,IRC_SIZE_SMALL);
                  wcsncpy(wprofile_fullpath,module_path,IRC_SIZE_SMALL);
                  wcscpy(wcsrchr(wprofile_fullpath,'\\')+1,wprofile_name);
                  guimanager_delete(d_index[i]);
                  DeleteFile(wprofile_fullpath);
               }
               break;
            }
            case IDM_EXIT:{
               SendMessage(hWnd,WM_CLOSE,0,0);
               break;
            }
            case IDM_LAUNCH:{
               int d_index[10];
               int s_index=0;
               int i;
               guimanager_getselected(d_index, &s_index);
               for(i=0;i<s_index;i++){
                  Button_GetText(manager.connect_handles[d_index[i]],wprofile_name,IRC_SIZE_SMALL);
                  CreateProcess(module_path,wprofile_name,NULL,NULL,FALSE,INHERIT_CALLER_PRIORITY,NULL,NULL,NULL,NULL);
               }
               SendMessage(hWnd,WM_CLOSE,0,0);
               break;
            }
         }
         break;
      }
      case WM_SIZE:{
         /*refresh_sizes(LOWORD(lParam),HIWORD(lParam),GetScreenCapsX(),GetScreenCapsY());

         MoveWindow(edit_chatinput_handle,EDITCHAT_LEFT,EDITCHAT_TOP,EDITCHAT_WIDTH,EDITCHAT_HEIGHT,FALSE);
         MoveWindow(button_chatsend_handle,BUTTONCHAT_LEFT,BUTTONCHAT_TOP,BUTTONCHAT_WIDTH,BUTTONCHAT_HEIGHT,FALSE);
         MoveWindow(tabcontrol_chatview_handle,TABCONTROLCHAT_LEFT,TABCONTROLCHAT_TOP,TABCONTROLCHAT_WIDTH,TABCONTROLCHAT_HEIGHT,FALSE);
         MoveWindow(button_closetab_handle,CLOSETAB_LEFT,CLOSETAB_TOP,CLOSETAB_WIDTH,CLOSETAB_HEIGHT,FALSE);
         tab_resize_all(tabcontrol_chatview_handle);*/
         break;
      }
      case WM_ACTIVATE:{
         SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      }
      case WM_SETTINGCHANGE:{
         SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
         break;
      }
      case WM_CREATE:{
         memset(&s_sai, 0, sizeof(SHACTIVATEINFO));
         s_sai.cbSize = sizeof(SHACTIVATEINFO);

         if(guimanager_init(hWnd)!=0){
            PostQuitMessage(0);
         }
         if(menu_bar_handle!=NULL){
            RECT rcMainWindow;
            RECT rcMenuBar;
            GetWindowRect(hWnd, &rcMainWindow);
            GetWindowRect(menu_bar_handle, &rcMenuBar);
            rcMainWindow.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
            MoveWindow(hWnd, rcMainWindow.left, rcMainWindow.top, rcMainWindow.right-rcMainWindow.left, rcMainWindow.bottom-rcMainWindow.top, FALSE);
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
         guimanager_destroy();
         PostQuitMessage(0);
         break;
      }
   }
   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int guimanager_init(HWND hWnd){
   memset(&manager,0,sizeof(struct guimanager_t));
   wchar_t wfilespath[IRC_SIZE_SMALL];
   wcscpy(wfilespath,module_path);
   wcscpy(wcsrchr(wfilespath,'\\')+1,L"*.ini");
   WIN32_FIND_DATA find;
   HANDLE findhwnd = FindFirstFile(wfilespath,&find);
   if(findhwnd!=INVALID_HANDLE_VALUE){
      guimanager_create(find.cFileName,hWnd);
      while(FindNextFile(findhwnd,&find)){
         guimanager_create(find.cFileName,hWnd);
      }
      FindClose(findhwnd);
   }
   init_menu_bar(hWnd,IDR_MAIN_MENU_MANAGER);
   return 0;
}

void guimanager_destroy(){
   int i;
   for(i=0;i<manager.connect_size;i++){
      DestroyWindow(manager.connect_handles[i]);
   }
   memset(&manager,0,sizeof(struct guimanager_t));
}

int guimanager_create(wchar_t *text, HWND hWnd){
   if(manager.connect_size>=10){
      return -1;
   }
   manager.connect_handles[manager.connect_size]=CreateWindowEx(0,L"button",text,BS_LEFTTEXT|BS_AUTOCHECKBOX|WS_CHILD|WS_VISIBLE,0,5+(manager.connect_size*20),150,20,hWnd,(HMENU)NULL,app_instance,NULL);
   manager.connect_size++;
   return 0;
}

void guimanager_getselected(int *d_result, int *s_result){
   *s_result = 0;
   int i;
   for(i=0;i<manager.connect_size;i++){
      if(Button_GetCheck(manager.connect_handles[i])==BST_CHECKED){
         d_result[*s_result] = i;
         (*s_result)++;
      }
   }
}

int guimanager_delete(int index){
   if(index<0 || index>=10){
      return -1;
   }
   DestroyWindow(manager.connect_handles[index]);
   manager.connect_size--;
   int i;
   for(i=index;i<manager.connect_size;i++){
      manager.connect_handles[i]=manager.connect_handles[i+1];
      MoveWindow(manager.connect_handles[i],0,5+(i*20),150,20,FALSE);
   }
   return 0;
}
