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
   switch(uMsg){
      case WM_COMMAND:{
         //int wmEvent = HIWORD(wParam);
         //HWND control_handler = (HWND)lParam;
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

               int result = checkbox_create(wprofile_name,hWnd);
               if(result==-1){
                  MessageBox(hWnd,L"Cannot create more profiles.",NULL,MB_ICONEXCLAMATION|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }else if(result==-2){
                  MessageBox(hWnd,L"A profile with that name already exists.",NULL,MB_ICONEXCLAMATION|MB_APPLMODAL|MB_SETFOREGROUND);
                  break;
               }

               iniparser_t iniparser;
               if(iniparser_init(&iniparser)!=0){
                  break;
               }
               iniparser_setstring(&iniparser, "server", "host", "chat.freenode.net");
               iniparser_setstring(&iniparser, "server", "port", "6667");
               iniparser_setstring(&iniparser, "server", "pass", "");
               iniparser_setstring(&iniparser, "client", "user", "user");
               iniparser_setstring(&iniparser, "client", "name", "Real Name");
               iniparser_setstring(&iniparser, "client", "nick", "change_me");
               iniparser_setstring(&iniparser, "client", "perform", "");
               iniparser_setstring(&iniparser, "autojoin", "channels", "#microirc");
               iniparser_setint(&iniparser, "autojoin", "delay", 5000);
               iniparser_setint(&iniparser, "connection", "connect_on_startup", 1);
               iniparser_setint(&iniparser, "connection", "reconnect_retries", 5);
               iniparser_setstring(&iniparser, "messages", "part", "");
               iniparser_setstring(&iniparser, "messages", "kick", "");
               iniparser_setstring(&iniparser, "messages", "quit", "http://code.google.com/p/microirc/");
               iniparser_setint(&iniparser, "miscellaneous", "encoding", 1);
               iniparser_setint(&iniparser, "miscellaneous", "bubble", 0);
               iniparser_setint(&iniparser, "miscellaneous", "sounds", 0);
               iniparser_setint(&iniparser, "miscellaneous", "led_number", -1);
               iniparser_setint(&iniparser, "miscellaneous", "led_interval", 500);
               if(winiparser_store(&iniparser,wprofile_name)!=0){
                  iniparser_destroy(&iniparser);
                  break;
               }
               iniparser_destroy(&iniparser);

               wchar_t *parameters[2]={L"manager",wprofile_name};
               if(!DialogBoxParam(config.h_instance, MAKEINTRESOURCE(IDD_PREFERENCES), hWnd, PreferencesProc, (LPARAM)parameters)){
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
                  wchar_t *parameters[2]={L"manager",wprofile_name};
                  DialogBoxParam(config.h_instance, MAKEINTRESOURCE(IDD_PREFERENCES), hWnd, PreferencesProc, (LPARAM)parameters);
               }
               break;
            }
            case IDM_REMOVE:{
               switch(MessageBox(hWnd,L"Are you sure you want to remove the selected profile(s)? This cannot be undone.",L"Confirmation",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL|MB_SETFOREGROUND)){
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
         break;// FIX ME
         if(config.menu_bar_handle!=NULL){
            RECT rcMainWindow;
            RECT rcMenuBar;
            GetWindowRect(hWnd, &rcMainWindow);
            GetWindowRect((HWND)config.menu_bar_handle, &rcMenuBar);
            rcMainWindow.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
            MoveWindow(hWnd, rcMainWindow.left, rcMainWindow.top, rcMainWindow.right-rcMainWindow.left, rcMainWindow.bottom-rcMainWindow.top, FALSE);
         }
         break;
      }
      case WM_QUIT:{
         //called on PostQuitMessage(0);
         break;
      }
      //case WM_HIBERNATE:{
      //}
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
	memset(&manager,0,sizeof(manager));
	wchar_t wfilespath[IRC_SIZE_SMALL];
	wfile_to_fullpath(L"*.ini",wfilespath);
	WIN32_FIND_DATA find;
	memset(&find,0,sizeof(find));
	HANDLE findhwnd = FindFirstFile(wfilespath,&find);
	if(findhwnd!=INVALID_HANDLE_VALUE){
		checkbox_create(find.cFileName,hWnd);
		while(FindNextFile(findhwnd,&find)){
			checkbox_create(find.cFileName,hWnd);
		}
		FindClose(findhwnd);
	}
	init_menu_bar(hWnd,IDR_MAIN_MENU_MANAGER);
	init_profile_screen(hWnd);
	return 0;
}

void guimanager_destroy(){
   int i;
   for(i=0;i<manager.connect_size;i++){
      DestroyWindow(manager.connect_handles[i]);
   }
   destroy_profile_screen();
   memset(&manager,0,sizeof(struct guimanager_t));
}
