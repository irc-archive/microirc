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
         return (INT_PTR)TRUE;
      }
      case WM_COMMAND:{
         if (LOWORD(wParam) == IDOK){
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
   return (INT_PTR)FALSE;
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
         break;
      }
      /*case WM_SETTINGCHANGE:{
         SendMessage(GetParent(hDlg),WM_SETTINGCHANGE,wParam,lParam);
         break;
      }*/
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
         if (!SHCreateMenuBar(&mbi)){
            return FALSE;
         }
         SetScrollRange(hDlg,SB_VERT,SCROLL_PREFERENCES_MIN_POSITIONS,SCROLL_PREFERENCES_MAX_POSITIONS,TRUE);
         UpdateWindow(hDlg);

         iniparser_t iniparser;
         if(iniparser_init(&iniparser)!=0){
            return FALSE;
         }
         if(iniparser_load(&iniparser,file_config)!=0){
            iniparser_destroy(&iniparser);
            return FALSE;
         }
         settext_fromstr(hDlg,IDC_EDIT1,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"));
         settext_fromstr(hDlg,IDC_EDIT2,iniparser_getstring(&iniparser, "server", "port", "6667"));
         settext_fromstr(hDlg,IDC_EDIT3,iniparser_getstring(&iniparser, "client", "user", "user"));
         settext_fromstr(hDlg,IDC_EDIT4,iniparser_getstring(&iniparser, "client", "name", "Real Name"));
         settext_fromstr(hDlg,IDC_EDIT5,iniparser_getstring(&iniparser, "client", "nick", "change_me"));
         settext_fromstr(hDlg,IDC_EDIT6,iniparser_getstring(&iniparser, "client", "perform", ""));
         settext_fromstr(hDlg,IDC_EDIT7,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"));
         settext_fromint(hDlg,IDC_EDIT8,iniparser_getint(&iniparser, "autojoin", "delay", 5000));
         settext_fromint(hDlg,IDC_EDIT9,config.reconnect);
         settext_fromstr(hDlg,IDC_EDIT10,config.part);
         settext_fromstr(hDlg,IDC_EDIT11,config.kick);
         settext_fromstr(hDlg,IDC_EDIT12,config.quit);

         HWND combo = GetDlgItem(hDlg,IDC_COMBO1);
         ComboBox_AddString(combo,L"Local");
         ComboBox_AddString(combo,L"UTF-8");
         if(config.encoding == CP_UTF8){
            ComboBox_SetCurSel(combo,1);
         }else{
            ComboBox_SetCurSel(combo,0);
         }

         settext_fromint(hDlg,IDC_EDIT13,config.bubble);

         HWND check = GetDlgItem(hDlg,IDC_CHECK1);
         if(config.sounds != 0){
            Button_SetCheck(check,BST_CHECKED);
         }else{
            Button_SetCheck(check,BST_UNCHECKED);
         }

         settext_fromint(hDlg,IDC_EDIT14,config.lednumber);
         settext_fromint(hDlg,IDC_EDIT15,config.ledinterval);
         iniparser_destroy(&iniparser);
         return TRUE;
      }
      case WM_COMMAND:{
         if (LOWORD(wParam) == IDOK){
            iniparser_t iniparser;
            if(iniparser_init(&iniparser)!=0){
               return FALSE;
            }
            if(iniparser_load(&iniparser,file_config)!=0){
               iniparser_destroy(&iniparser);
               return FALSE;
            }
            char tempstr[IRCPROTOCOL_SIZE_MEDIUM];
            int tempint;

            gettext_tostr(hDlg, IDC_EDIT1, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Host is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "server", "host", tempstr);

            gettext_tostr(hDlg, IDC_EDIT2, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Port is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "server", "port", tempstr);

            gettext_tostr(hDlg, IDC_EDIT3, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"User is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "user", tempstr);

            gettext_tostr(hDlg, IDC_EDIT4, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Name is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "name", tempstr);

            gettext_tostr(hDlg, IDC_EDIT5, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Nick is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "nick", tempstr);

            gettext_tostr(hDlg, IDC_EDIT6, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            iniparser_setstring(&iniparser, "client", "perform", tempstr);

            gettext_tostr(hDlg, IDC_EDIT7, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            iniparser_setstring(&iniparser, "autojoin", "channels", tempstr);

            tempint = gettext_toint(hDlg,IDC_EDIT8);
            if(tempint < 0){
               MessageBox(hDlg,L"Auto-join Delay is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setint(&iniparser, "autojoin", "delay", tempint);

            tempint = gettext_toint(hDlg,IDC_EDIT9);
            if(tempint < 0){
               MessageBox(hDlg,L"Reconnect Retries is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setint(&iniparser, "autoreconnect", "retries", tempint);
            config.reconnect = tempint;

            gettext_tostr(hDlg, IDC_EDIT10, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            iniparser_setstring(&iniparser, "messages", "part", tempstr);
            strncpy0(config.part,tempstr,IRC_SIZE_SMALL);

            gettext_tostr(hDlg, IDC_EDIT11, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            iniparser_setstring(&iniparser, "messages", "kick", tempstr);
            strncpy0(config.kick,tempstr,IRC_SIZE_SMALL);

            gettext_tostr(hDlg, IDC_EDIT12, tempstr, IRCPROTOCOL_SIZE_MEDIUM);
            iniparser_setstring(&iniparser, "messages", "quit", tempstr);
            strncpy0(config.quit,tempstr,IRC_SIZE_SMALL);

            HWND combo = GetDlgItem(hDlg,IDC_COMBO1);
            tempint = ComboBox_GetCurSel(combo);
            iniparser_setint(&iniparser, "miscellaneous", "encoding", tempint);
            if(tempint != 0){
               config.encoding = CP_UTF8;
            }else{
               config.encoding = CP_ACP;
            }

            tempint = gettext_toint(hDlg,IDC_EDIT13);
            iniparser_setint(&iniparser, "miscellaneous", "bubble", tempint);
            config.bubble = tempint;

            HWND check = GetDlgItem(hDlg,IDC_CHECK1);
            if(Button_GetCheck(check)==BST_CHECKED){
               tempint = 1;
            }else{
               tempint = 0;
            }
            iniparser_setint(&iniparser, "miscellaneous", "sounds", tempint);
            config.sounds = tempint;

            tempint = gettext_toint(hDlg,IDC_EDIT14);
            iniparser_setint(&iniparser, "miscellaneous", "lednumber", tempint);
            config.lednumber = tempint;

            tempint = gettext_toint(hDlg,IDC_EDIT15);
            if(tempint < 0){
               MessageBox(hDlg,L"Led Interval is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setint(&iniparser, "miscellaneous", "ledinterval", tempint);
            config.ledinterval = tempint;

            if(iniparser_store(&iniparser,file_config)!=0){
               iniparser_destroy(&iniparser);
               return FALSE;
            }
            iniparser_destroy(&iniparser);
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
         }else if (LOWORD(wParam) == IDCANCEL){
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

INT_PTR CALLBACK InputBoxProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_INITDIALOG:{
         wchar_t **titletext = (wchar_t**)lParam;
         SetWindowText(hDlg,(LPCWSTR)titletext[0]);
         HWND edit = GetDlgItem(hDlg,IDC_EDIT1);
         Edit_SetText(edit,titletext[1]);
         int element = Edit_GetTextLength(edit);
         SendMessage(edit, EM_SETSEL, element, element);
         SetFocus(edit);
         return FALSE;
      }
      //VK_ENTER, case WM_GETDLGCODE:MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
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
         EndDialog(hDlg, uMsg);
         return TRUE;
      }
   }
   return FALSE;
}
