/*
* proc_functions.h
*
* This file contains the code used on resource dialogs.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

INT_PTR CALLBACK AboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_INITDIALOG:{
         /*SHINITDLGINFO shidi;
         shidi.dwMask = SHIDIM_FLAGS;
         shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU | SHIDIF_WANTSCROLLBAR;
         shidi.hDlg = hDlg;
         SHInitDialog(&shidi);*/
         return TRUE;
      }
      case WM_COMMAND:{
         if(LOWORD(wParam) == IDOK){
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
         }
         break;
      }
      case WM_CLOSE:{
         EndDialog(hDlg, uMsg);
         return TRUE;
      }
   }
   return FALSE;
}

INT_PTR CALLBACK PreferencesProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch(uMsg){
      case WM_VSCROLL:{
         RECT window_size;
         memset(&window_size,0,sizeof(window_size));
         window_size.bottom=SCROLL_PREFERENCES_HEIGHT;
         MapDialogRect(hDlg,&window_size);
         int dialog_height = window_size.bottom;

         SCROLLINFO info;
         memset(&info,0,sizeof(info));
         info.cbSize = sizeof(info);
         info.fMask = SIF_POS|SIF_TRACKPOS;
         GetScrollInfo(hDlg,SB_VERT,&info);
         switch(LOWORD(wParam)){
            case SB_THUMBTRACK:{
            }
            case SB_THUMBPOSITION:{
               int pos = HIWORD(wParam);
               if(pos>info.nPos && pos<=SCROLL_PREFERENCES_MAX_POSITIONS){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,-((dialog_height/SCROLL_PREFERENCES_MAX_POSITIONS)*(pos-info.nPos)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }else if(pos<info.nPos && pos>=SCROLL_PREFERENCES_MIN_POSITIONS){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,((dialog_height/SCROLL_PREFERENCES_MAX_POSITIONS)*(info.nPos-pos)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
            case SB_TOP:{
            }
            case SB_PAGEUP:{
            }
            case SB_LINEUP:{
               int pos = info.nPos;
               if(pos > SCROLL_PREFERENCES_MIN_POSITIONS){
                  pos--;
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,(dialog_height/SCROLL_PREFERENCES_MAX_POSITIONS),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
            case SB_BOTTOM:{
            }
            case SB_PAGEDOWN:{
            }
            case SB_LINEDOWN:{
               int pos = info.nPos;
               if(pos < SCROLL_PREFERENCES_MAX_POSITIONS){
                  pos++;
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,-(dialog_height/SCROLL_PREFERENCES_MAX_POSITIONS),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
         }
         return TRUE;
      }
      case WM_INITDIALOG:{
         SetScrollRange(hDlg,SB_VERT,SCROLL_PREFERENCES_MIN_POSITIONS,SCROLL_PREFERENCES_MAX_POSITIONS,TRUE);
         UpdateWindow(hDlg);

         wchar_t **parameters = (wchar_t**)lParam;//"MANAGER" or "CLIENT", profile
         iniparser_t iniparser;
         if(iniparser_init(&iniparser)!=0){
            break;
         }
         if(winiparser_load(&iniparser,parameters[1])!=0){
            iniparser_destroy(&iniparser);
            break;
         }
         settext_fromwstr(hDlg,IDC_EDIT0,parameters[0]);
         settext_fromwstr(hDlg,IDC_EDIT1,parameters[1]);
         settext_fromstr(hDlg,IDC_EDIT2,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, IRC_CONF_HOST_VAL));
         settext_fromstr(hDlg,IDC_EDIT3,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, IRC_CONF_PORT_VAL));
         settext_fromstr(hDlg,IDC_EDIT4,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, IRC_CONF_PASS_VAL));
         settext_fromstr(hDlg,IDC_EDIT5,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, IRC_CONF_USER_VAL));
         settext_fromstr(hDlg,IDC_EDIT6,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, IRC_CONF_NAME_VAL));
         settext_fromstr(hDlg,IDC_EDIT7,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, IRC_CONF_NICK_VAL));
         settext_fromstr(hDlg,IDC_EDIT8,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, IRC_CONF_PERFORM_VAL));
         settext_fromstr(hDlg,IDC_EDIT9,iniparser_getstring(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, IRC_CONF_CHANNELS_VAL));
         settext_fromint(hDlg,IDC_EDIT10,iniparser_getint(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, IRC_CONF_DELAY_VAL));
         if(wcscmp(parameters[0],IRC_CONST_MANAGER)==0){
            setcheck_fromint(hDlg,IDC_CHECK1,iniparser_getint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, IRC_CONF_STARTUP_VAL));
            settext_fromint(hDlg,IDC_EDIT11,iniparser_getint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, IRC_CONF_RETRIES_VAL));
            settext_fromstr(hDlg,IDC_EDIT12,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, IRC_CONF_PART_VAL));
            settext_fromstr(hDlg,IDC_EDIT13,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, IRC_CONF_KICK_VAL));
            settext_fromstr(hDlg,IDC_EDIT14,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, IRC_CONF_QUIT_VAL));
            setcombo_fromint(hDlg,IDC_COMBO1,iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, IRC_CONF_ENCODING_VAL));
            settext_fromint(hDlg,IDC_EDIT15,iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, IRC_CONF_BUBBLE_VAL));
            setcheck_fromint(hDlg,IDC_CHECK2,iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, IRC_CONF_SOUNDS_VAL));
            settext_fromint(hDlg,IDC_EDIT16,iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, IRC_CONF_LEDNUMBER_VAL));
            settext_fromint(hDlg,IDC_EDIT17,iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, IRC_CONF_LEDINTERVAL_VAL));
         }else if(wcscmp(parameters[0],IRC_CONST_CLIENT)==0){
            setcheck_fromint(hDlg,IDC_CHECK1,client.config.connect_on_startup);
            settext_fromint(hDlg,IDC_EDIT11,client.config.reconnect_retries);
            settext_fromstr(hDlg,IDC_EDIT12,client.config.part);
            settext_fromstr(hDlg,IDC_EDIT13,client.config.kick);
            settext_fromstr(hDlg,IDC_EDIT14,client.config.quit);
            if(client.config.encoding==CP_UTF8){
               setcombo_fromint(hDlg,IDC_COMBO1,1);
            }else{
               setcombo_fromint(hDlg,IDC_COMBO1,0);
            }
            settext_fromint(hDlg,IDC_EDIT15,client.config.bubble);
            setcheck_fromint(hDlg,IDC_CHECK2,client.config.sounds);
            settext_fromint(hDlg,IDC_EDIT16,client.config.led_number);
            settext_fromint(hDlg,IDC_EDIT17,client.config.led_interval);
         }
         iniparser_destroy(&iniparser);
         return TRUE;
      }
      case WM_COMMAND:{
         if(LOWORD(wParam) == IDOK){
            iniparser_t iniparser;
            if(iniparser_init(&iniparser)!=0){
               break;
            }
            wchar_t type[IRC_SIZE_SMALL];
            wchar_t profile[IRC_SIZE_SMALL];
            char host[IRCPROTOCOL_SIZE_SMALL];
            char port[IRCPROTOCOL_SIZE_SMALL];
            char pass[IRCPROTOCOL_SIZE_SMALL];
            char user[IRCPROTOCOL_SIZE_SMALL];
            char name[IRCPROTOCOL_SIZE_SMALL];
            char nick[IRCPROTOCOL_SIZE_SMALL];
            char perform[IRCPROTOCOL_SIZE_MEDIUM];
            char channels[IRCPROTOCOL_SIZE_MEDIUM];
            int delay;
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
            gettext_towstr(hDlg,IDC_EDIT0,type,IRC_SIZE_SMALL);
            gettext_towstr(hDlg,IDC_EDIT1,profile,IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT2, host, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT3, port, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT4, pass, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT5, user, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT6, name, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT7, nick, IRCPROTOCOL_SIZE_MEDIUM);
            gettext_tostr(hDlg, IDC_EDIT8, perform, IRCPROTOCOL_SIZE_MEDIUM);
            gettext_tostr(hDlg, IDC_EDIT9, channels, IRCPROTOCOL_SIZE_MEDIUM);
            delay = gettext_toint(hDlg,IDC_EDIT10);
            connect_on_startup = getint_fromcheck(hDlg,IDC_CHECK1);
            reconnect_retries = gettext_toint(hDlg,IDC_EDIT11);
            gettext_tostr(hDlg, IDC_EDIT12, part, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT13, kick, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT14, quit, IRC_SIZE_SMALL);
            encoding = getint_fromcombo(hDlg,IDC_COMBO1);
            bubble = gettext_toint(hDlg,IDC_EDIT15);
            sounds = getint_fromcheck(hDlg,IDC_CHECK2);
            led_number = gettext_toint(hDlg,IDC_EDIT16);
            led_interval = gettext_toint(hDlg,IDC_EDIT17);
            if(strlen(host)==0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG1),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(port)==0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG2),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(user)==0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG3),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(name)==0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG4),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(nick)==0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG5),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(delay < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG6),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(reconnect_retries < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG7),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(bubble < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG8),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(led_interval < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG9),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, host);
            iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, port);
            iniparser_setstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, pass);
            iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, user);
            iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, name);
            iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, nick);
            iniparser_setstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, perform);
            iniparser_setstring(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, channels);
            iniparser_setint(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, delay);
            iniparser_setint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, connect_on_startup);
            iniparser_setint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, reconnect_retries);
            iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, part);
            iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, kick);
            iniparser_setstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, quit);
            iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, encoding);
            iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, bubble);
            iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, sounds);
            iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, led_number);
            iniparser_setint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, led_interval);
            if(wcscmp(type,IRC_CONST_CLIENT)==0){
               client.config.connect_on_startup = connect_on_startup;
               client.config.reconnect_retries = reconnect_retries;
               strncpy0(client.config.part,part,IRC_SIZE_SMALL);
               strncpy0(client.config.kick,kick,IRC_SIZE_SMALL);
               strncpy0(client.config.quit,quit,IRC_SIZE_SMALL);
               if(encoding!=0){
                  client.config.encoding=CP_UTF8;
               }else{
                  client.config.encoding=CP_ACP;
               }
               client.config.bubble=bubble;
               client.config.sounds=sounds;
               client.config.led_number=led_number;
               client.config.led_interval=led_interval;
            }
            if(winiparser_store(&iniparser,profile)!=0){
               iniparser_destroy(&iniparser);
               break;
            }
            iniparser_destroy(&iniparser);
            EndDialog(hDlg, TRUE);
            return TRUE;
         }else if (LOWORD(wParam) == IDCANCEL){
            EndDialog(hDlg, FALSE);
            return TRUE;
         }
         break;
      }
      case WM_CLOSE:{
         EndDialog(hDlg, FALSE);
         return TRUE;
      }
   }
   return FALSE;
}

WNDPROC old_EditInputBoxProc;
LRESULT CALLBACK EditInputBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_CHAR:{
         if(wParam == VK_RETURN){
            SendMessage(GetParent(hWnd),WM_COMMAND,IDOK,0);
            return 0;
         }
         break;
      }
   }
   return CallWindowProc(old_EditInputBoxProc, hWnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK InputBoxProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_INITDIALOG:{
         wchar_t **titletext = (wchar_t**)lParam;
         SetWindowText(hDlg,(LPCWSTR)titletext[0]);
         HWND edit = GetDlgItem(hDlg,IDC_EDIT1);

         old_EditInputBoxProc = (WNDPROC)GetWindowLong(edit,GWL_WNDPROC);
         SetWindowLong(edit,GWL_WNDPROC,(LONG)EditInputBoxProc);

         Edit_SetText(edit,titletext[1]);
         int element = Edit_GetTextLength(edit);
         SendMessage(edit, EM_SETSEL, element, element);
         return TRUE;
      }
      case WM_COMMAND:{
         if (LOWORD(wParam) == IDOK){
            HWND edit = GetDlgItem(hDlg,IDC_EDIT1);
            LPWSTR result = (LPWSTR)malloc(IRC_SIZE_SMALL);
            Edit_GetText(edit,result,IRC_SIZE_SMALL);
            EndDialog(hDlg, (int)result);
            return TRUE;
         }else if (LOWORD(wParam) == IDCANCEL){
            EndDialog(hDlg, (int)NULL);
            return TRUE;
         }
         break;
      }
      case WM_CLOSE:{
         EndDialog(hDlg, (int)NULL);
         return TRUE;
      }
   }
   return FALSE;
}

int open_input_box(HWND parent_window, wchar_t *title, wchar_t *text, wchar_t *result, unsigned int result_len){
   //result will be null string if DialogBoxParam fails or user provides no text
   wchar_t *titletext[2]={title,text};
   wchar_t *temp = (wchar_t*)DialogBoxParam(config.h_instance, MAKEINTRESOURCE(IDD_INPUTBOX), parent_window, InputBoxProc, (LPARAM)titletext);
   if(temp!=NULL){
      //copy result to the provided buffer and deallocate
      wcsncpy(result, temp, result_len-1);
      free(temp);
      return 0;
   }else{
      *result = '\0';
      return -1;
   }
}

void init_menu_bar(HWND hWnd, int barId){
	config.menu_bar_handle = LoadMenu(config.h_instance, MAKEINTRESOURCE(barId));
	if(config.menu_bar_handle != NULL){
		SetMenu(hWnd, config.menu_bar_handle);
	}
}

void destroy_menu_bar(){
	if(config.menu_bar_handle != NULL){
		DestroyMenu(config.menu_bar_handle);
	}
}

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
}

void destroy_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_UNLOAD_CURSOR,0,0);
}
