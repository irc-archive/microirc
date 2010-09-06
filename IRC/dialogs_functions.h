/*
* dialogs_functions.h
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
         SHINITDLGINFO shidi;
         shidi.dwMask = SHIDIM_FLAGS;
         shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU | SHIDIF_WANTSCROLLBAR;
         shidi.hDlg = hDlg;
         SHInitDialog(&shidi);
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
         memset(&window_size,0,sizeof(RECT));
         window_size.bottom=SCROLL_PREFERENCES_HEIGHT;
         MapDialogRect(hDlg,&window_size);
         int dialog_height = window_size.bottom;
         GetWindowRect(hDlg,&window_size);
         dialog_height -= window_size.bottom-window_size.top*4;//2 for menus(up/down), 2 for input panel

         SCROLLINFO info;
         memset(&info,0,sizeof(SCROLLINFO));
         info.cbSize = sizeof(SCROLLINFO);
         info.fMask = SIF_POS|SIF_TRACKPOS;
         GetScrollInfo(hDlg,SB_VERT,&info);
         switch(LOWORD(wParam)){
            case SB_THUMBTRACK:{
            }
            case SB_THUMBPOSITION:{
               int pos = HIWORD(wParam);
               if(pos>info.nPos && pos<=SCROLL_PREFERENCES_MAX_POSITIONS){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,-(((dialog_height)/(SCROLL_PREFERENCES_MAX_POSITIONS-1))*(pos-info.nPos)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }else if(pos<info.nPos && pos>=SCROLL_PREFERENCES_MIN_POSITIONS){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,(((dialog_height)/(SCROLL_PREFERENCES_MAX_POSITIONS-1))*(info.nPos-pos)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
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
                  ScrollWindowEx(hDlg,0,((dialog_height)/(SCROLL_PREFERENCES_MAX_POSITIONS-1)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
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
                  ScrollWindowEx(hDlg,0,-((dialog_height)/(SCROLL_PREFERENCES_MAX_POSITIONS-1)),NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
         }
         return TRUE;
      }
      case WM_INITDIALOG:{
         SHINITDLGINFO shidi;
         memset(&shidi, 0, sizeof(SHINITDLGINFO));
         shidi.dwMask = SHIDIM_FLAGS;
         shidi.dwFlags = SHIDIF_DONEBUTTON |SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_SIPDOWN | SHIDIF_EMPTYMENU;
         shidi.hDlg = hDlg;
         SHInitDialog(&shidi);

         SHMENUBARINFO mbi;
         memset(&mbi, 0, sizeof(SHMENUBARINFO));
         mbi.cbSize = sizeof(SHMENUBARINFO);
         mbi.hwndParent = hDlg;
         mbi.nToolBarId = IDR_PREFERENCES_MENU;
         mbi.hInstRes = app_instance;
         if(!SHCreateMenuBar(&mbi)){
            break;
         }
         SetScrollRange(hDlg,SB_VERT,SCROLL_PREFERENCES_MIN_POSITIONS,SCROLL_PREFERENCES_MAX_POSITIONS,TRUE);
         UpdateWindow(hDlg);

         wchar_t **parameters = (wchar_t**)lParam;//"manager" or "client", profile
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
         settext_fromstr(hDlg,IDC_EDIT2,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"));
         settext_fromstr(hDlg,IDC_EDIT3,iniparser_getstring(&iniparser, "server", "port", "6667"));
         settext_fromstr(hDlg,IDC_EDIT4,iniparser_getstring(&iniparser, "client", "user", "user"));
         settext_fromstr(hDlg,IDC_EDIT5,iniparser_getstring(&iniparser, "client", "name", "Real Name"));
         settext_fromstr(hDlg,IDC_EDIT6,iniparser_getstring(&iniparser, "client", "nick", "change_me"));
         settext_fromstr(hDlg,IDC_EDIT7,iniparser_getstring(&iniparser, "client", "perform", ""));
         settext_fromstr(hDlg,IDC_EDIT8,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"));
         settext_fromint(hDlg,IDC_EDIT9,iniparser_getint(&iniparser, "autojoin", "delay", 5000));
         if(wcscmp(parameters[0],L"manager")==0){
            settext_fromint(hDlg,IDC_EDIT10,iniparser_getint(&iniparser, "autoreconnect", "retries", 5));
            settext_fromstr(hDlg,IDC_EDIT11,iniparser_getstring(&iniparser, "messages", "part", ""));
            settext_fromstr(hDlg,IDC_EDIT12,iniparser_getstring(&iniparser, "messages", "kick", ""));
            settext_fromstr(hDlg,IDC_EDIT13,iniparser_getstring(&iniparser, "messages", "quit", "http://code.google.com/p/microirc/"));
            setcombo_fromint(hDlg,IDC_COMBO1,iniparser_getint(&iniparser, "miscellaneous", "encoding", 1));
            settext_fromint(hDlg,IDC_EDIT14,iniparser_getint(&iniparser, "miscellaneous", "bubble", 0));
            setcheck_fromint(hDlg,IDC_CHECK1,iniparser_getint(&iniparser, "miscellaneous", "sounds", 0));
            settext_fromint(hDlg,IDC_EDIT15,iniparser_getint(&iniparser, "miscellaneous", "lednumber", -1));
            settext_fromint(hDlg,IDC_EDIT16,iniparser_getint(&iniparser, "miscellaneous", "ledinterval", 500));
         }else if(wcscmp(parameters[0],L"client")==0){
            settext_fromint(hDlg,IDC_EDIT10,config.reconnect);
            settext_fromstr(hDlg,IDC_EDIT11,config.part);
            settext_fromstr(hDlg,IDC_EDIT12,config.kick);
            settext_fromstr(hDlg,IDC_EDIT13,config.quit);
            if(config.encoding==CP_UTF8){
               setcombo_fromint(hDlg,IDC_COMBO1,1);
            }else{
               setcombo_fromint(hDlg,IDC_COMBO1,0);
            }
            settext_fromint(hDlg,IDC_EDIT14,config.bubble);
            setcheck_fromint(hDlg,IDC_CHECK1,config.sounds);
            settext_fromint(hDlg,IDC_EDIT15,config.lednumber);
            settext_fromint(hDlg,IDC_EDIT16,config.ledinterval);
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
            char user[IRCPROTOCOL_SIZE_SMALL];
            char name[IRCPROTOCOL_SIZE_SMALL];
            char nick[IRCPROTOCOL_SIZE_SMALL];
            char perform[IRCPROTOCOL_SIZE_MEDIUM];
            char autojoin_channels[IRCPROTOCOL_SIZE_MEDIUM];
            int autojoin_delay;
            int reconnect;
            char part[IRC_SIZE_SMALL];
            char kick[IRC_SIZE_SMALL];
            char quit[IRC_SIZE_SMALL];
            int encoding;
            int bubble;
            int sounds;
            int lednumber;
            int ledinterval;
            gettext_towstr(hDlg,IDC_EDIT0,type,IRC_SIZE_SMALL);
            gettext_towstr(hDlg,IDC_EDIT1,profile,IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT2, host, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT3, port, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT4, user, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT5, name, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT6, nick, IRCPROTOCOL_SIZE_MEDIUM);
            gettext_tostr(hDlg, IDC_EDIT7, perform, IRCPROTOCOL_SIZE_MEDIUM);
            gettext_tostr(hDlg, IDC_EDIT8, autojoin_channels, IRCPROTOCOL_SIZE_MEDIUM);
            autojoin_delay = gettext_toint(hDlg,IDC_EDIT9);
            reconnect = gettext_toint(hDlg,IDC_EDIT10);
            gettext_tostr(hDlg, IDC_EDIT11, part, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT12, kick, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT13, quit, IRC_SIZE_SMALL);
            encoding = getint_fromcombo(hDlg,IDC_COMBO1);
            bubble = gettext_toint(hDlg,IDC_EDIT14);
            sounds = getint_fromcheck(hDlg,IDC_CHECK1);
            lednumber = gettext_toint(hDlg,IDC_EDIT15);
            ledinterval = gettext_toint(hDlg,IDC_EDIT16);
            if(strlen(host)==0){
               MessageBox(hDlg,L"Host is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(port)==0){
               MessageBox(hDlg,L"Port is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(user)==0){
               MessageBox(hDlg,L"User is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(name)==0){
               MessageBox(hDlg,L"Name is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(strlen(nick)==0){
               MessageBox(hDlg,L"Nick is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(autojoin_delay < 0){
               MessageBox(hDlg,L"Auto-join Delay is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(reconnect < 0){
               MessageBox(hDlg,L"Reconnect Retries is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            if(ledinterval < 0){
               MessageBox(hDlg,L"Led Interval is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               break;
            }
            iniparser_setstring(&iniparser, "server", "host", host);
            iniparser_setstring(&iniparser, "server", "port", port);
            iniparser_setstring(&iniparser, "client", "user", user);
            iniparser_setstring(&iniparser, "client", "name", name);
            iniparser_setstring(&iniparser, "client", "nick", nick);
            iniparser_setstring(&iniparser, "client", "perform", perform);
            iniparser_setstring(&iniparser, "autojoin", "channels", autojoin_channels);
            iniparser_setint(&iniparser, "autojoin", "delay", autojoin_delay);
            iniparser_setint(&iniparser, "autoreconnect", "retries", reconnect);
            iniparser_setstring(&iniparser, "messages", "part", part);
            iniparser_setstring(&iniparser, "messages", "kick", kick);
            iniparser_setstring(&iniparser, "messages", "quit", quit);
            iniparser_setint(&iniparser, "miscellaneous", "encoding", encoding);
            iniparser_setint(&iniparser, "miscellaneous", "bubble", bubble);
            iniparser_setint(&iniparser, "miscellaneous", "sounds", sounds);
            iniparser_setint(&iniparser, "miscellaneous", "lednumber", lednumber);
            iniparser_setint(&iniparser, "miscellaneous", "ledinterval", ledinterval);
            if(wcscmp(type,L"client")==0){
               config.reconnect = reconnect;
               strncpy0(config.part,part,IRC_SIZE_SMALL);
               strncpy0(config.kick,kick,IRC_SIZE_SMALL);
               strncpy0(config.quit,quit,IRC_SIZE_SMALL);
               if(encoding!=0){
                  config.encoding=CP_UTF8;
               }else{
                  config.encoding=CP_ACP;
               }
               config.bubble=bubble;
               config.sounds=sounds;
               config.lednumber=lednumber;
               config.ledinterval=ledinterval;
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
