/*
* dialogs.h
*
* This file contains the code used on resource dialogs.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Jos� Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
   switch (message){
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
         EndDialog(hDlg, message);
         return TRUE;
      }
   }
   return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Preferences(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
   switch (message){
      case WM_VSCROLL:{
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
               if(info.nPos==1 && pos==2){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,SCROLLPAGE_HEIGHT*-height,NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }else if(info.nPos==2 && pos==1){
                  SetScrollPos(hDlg,SB_VERT,pos,TRUE);
                  ScrollWindowEx(hDlg,0,SCROLLPAGE_HEIGHT*height,NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
            case SB_TOP:{
            }
            case SB_PAGEUP:{
            }
            case SB_LINEUP:{
               if(info.nPos != 1){
                  SetScrollPos(hDlg,SB_VERT,1,TRUE);
                  ScrollWindowEx(hDlg,0,SCROLLPAGE_HEIGHT*height,NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
            case SB_BOTTOM:{
            }
            case SB_PAGEDOWN:{
            }
            case SB_LINEDOWN:{
               if(info.nPos != 2){
                  SetScrollPos(hDlg,SB_VERT,2,TRUE);
                  ScrollWindowEx(hDlg,0,SCROLLPAGE_HEIGHT*-height,NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
               }
               break;
            }
         }
         break;
      }
      case WM_INITDIALOG:{
         SHINITDLGINFO shidi;
         shidi.dwMask = SHIDIM_FLAGS;
         shidi.dwFlags = SHIDIF_DONEBUTTON |SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_SIPDOWN | SHIDIF_EMPTYMENU;
         shidi.hDlg = hDlg;
         SHInitDialog(&shidi);

         SHACTIVATEINFO s_sai;
         SHMENUBARINFO mbi;
         memset(&mbi, 0, sizeof(SHMENUBARINFO));
         mbi.cbSize = sizeof(SHMENUBARINFO);
         mbi.hwndParent = hDlg;
         mbi.nToolBarId = IDR_PREFERENCES_MENU;
         mbi.hInstRes = hInstance_Main;
         if (!SHCreateMenuBar(&mbi)){
            return FALSE;
         }
         memset(&s_sai, 0, sizeof (s_sai));
         s_sai.cbSize = sizeof (s_sai);
         UpdateWindow(hDlg);

         SetScrollRange(hDlg,SB_VERT,1,2,TRUE);

         iniparser_t iniparser;
         if(iniparser_init(&iniparser)!=0){
            return FALSE;
         }
         if(iniparser_load(&iniparser,configfile)!=0){
            iniparser_destroy(&iniparser);
            return FALSE;
         }
         settext_fromstr(hDlg,IDC_EDIT1,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"));
         settext_fromstr(hDlg,IDC_EDIT2,iniparser_getstring(&iniparser, "server", "port", "6667"));
         settext_fromstr(hDlg,IDC_EDIT3,iniparser_getstring(&iniparser, "client", "user", "irc"));
         settext_fromstr(hDlg,IDC_EDIT4,iniparser_getstring(&iniparser, "client", "name", "default"));
         settext_fromstr(hDlg,IDC_EDIT5,iniparser_getstring(&iniparser, "client", "nick", "default_nick"));
         settext_fromstr(hDlg,IDC_EDIT6,iniparser_getstring(&iniparser, "client", "perform", ""));
         settext_fromstr(hDlg,IDC_EDIT7,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"));
         settext_fromint(hDlg,IDC_EDIT8,iniparser_getint(&iniparser, "autojoin", "delay", 5000));
         settext_fromint(hDlg,IDC_EDIT9,config.reconnect);

         HWND combo = GetDlgItem(hDlg,IDC_COMBO1);
         ComboBox_AddString(combo,L"Local");
         ComboBox_AddString(combo,L"UTF-8");
         if(config.encoding == CP_UTF8){
            ComboBox_SetCurSel(combo,1);
         }else{
            ComboBox_SetCurSel(combo,0);
         }

         HWND check = GetDlgItem(hDlg,IDC_CHECK1);
         if(config.sounds){
            Button_SetCheck(check,BST_CHECKED);
         }else{
            Button_SetCheck(check,BST_UNCHECKED);
         }

         settext_fromint(hDlg,IDC_EDIT10,config.lednumber);
         settext_fromint(hDlg,IDC_EDIT11,config.ledinterval);
         return TRUE;
      }
      case WM_COMMAND:{
         if (LOWORD(wParam) == IDOK){
            iniparser_t iniparser;
            if(iniparser_init(&iniparser)!=0){
               return FALSE;
            }
            if(iniparser_load(&iniparser,configfile)!=0){
               iniparser_destroy(&iniparser);
               return FALSE;
            }
            char tempstr[IRC_BUFFER_SIZE_LITTLE];
            int tempint;

            gettext_tostr(hDlg, IDC_EDIT1, tempstr, IRC_BUFFER_SIZE_LITTLE);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Host is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "server", "host", tempstr);

            gettext_tostr(hDlg, IDC_EDIT2, tempstr, IRC_BUFFER_SIZE_LITTLE);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Port is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "server", "port", tempstr);

            gettext_tostr(hDlg, IDC_EDIT3, tempstr, IRC_BUFFER_SIZE_LITTLE);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"User is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "user", tempstr);

            gettext_tostr(hDlg, IDC_EDIT4, tempstr, IRC_BUFFER_SIZE_LITTLE);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Name is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "name", tempstr);

            gettext_tostr(hDlg, IDC_EDIT5, tempstr, IRC_BUFFER_SIZE_LITTLE);
            if(strlen(tempstr)==0){
               MessageBox(hDlg,L"Nick is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setstring(&iniparser, "client", "nick", tempstr);

            gettext_tostr(hDlg, IDC_EDIT6, tempstr, IRC_BUFFER_SIZE_LITTLE);
            iniparser_setstring(&iniparser, "client", "perform", tempstr);

            gettext_tostr(hDlg, IDC_EDIT7, tempstr, IRC_BUFFER_SIZE_LITTLE);
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

            HWND combo = GetDlgItem(hDlg,IDC_COMBO1);
            tempint = ComboBox_GetCurSel(combo);
            iniparser_setint(&iniparser, "options", "encoding", tempint);
            if(tempint){
               config.encoding = CP_UTF8;
            }else{
               config.encoding = CP_ACP;
            }

            HWND check = GetDlgItem(hDlg,IDC_CHECK1);
            if(Button_GetCheck(check)==BST_CHECKED){
               tempint = 1;
            }else{
               tempint = 0;
            }
            iniparser_setint(&iniparser, "options", "sounds", tempint);
            config.sounds = tempint;

            tempint = gettext_toint(hDlg,IDC_EDIT10);
            iniparser_setint(&iniparser, "options", "lednumber", tempint);
            config.lednumber = tempint;

            tempint = gettext_toint(hDlg,IDC_EDIT11);
            if(tempint < 0){
               MessageBox(hDlg,L"Led Interval is invalid.",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               return FALSE;
            }
            iniparser_setint(&iniparser, "options", "ledinterval", tempint);
            config.ledinterval = tempint;

            if(iniparser_store(&iniparser,configfile)!=0){
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
         EndDialog(hDlg, message);
         return TRUE;
      }
   }
   return FALSE;
}

INT_PTR CALLBACK InputBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
   switch (message){
      case WM_INITDIALOG:{
         SetWindowText(hDlg,(LPCWSTR)lParam);
         HWND edit = GetDlgItem(hDlg,IDC_EDIT1);
         SetFocus(edit);
         return FALSE;
      }
      //VK_ENTER, case WM_GETDLGCODE:MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
      case WM_COMMAND:{
         if (LOWORD(wParam) == IDOK){
            HWND edit = GetDlgItem(hDlg,IDC_EDIT1);
            LPWSTR result = (LPWSTR)malloc(IRC_SIZE_LITTLE);
            Edit_GetText(edit,result,IRC_SIZE_LITTLE);
            EndDialog(hDlg, (int)result);
            return TRUE;
         }else if (LOWORD(wParam) == IDCANCEL){
            EndDialog(hDlg, (int)NULL);
            return TRUE;
         }
         break;
      }
      case WM_CLOSE:{
         EndDialog(hDlg, message);
         return TRUE;
      }
   }
   return FALSE;
}
