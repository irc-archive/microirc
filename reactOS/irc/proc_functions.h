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

INT_PTR WINAPI OpenColorsDialog(HWND hWndParent, COLORREF def) {
    CHOOSECOLOR color;
    memset(&color,0,sizeof(color));

    COLORREF cRef[] = { RGB(0, 0, 0), RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 0, 128),
                        RGB(255, 255, 255), RGB(0, 255, 255), RGB(0, 0, 255),
                        RGB(255, 0, 255), RGB(0, 255, 0), RGB(128, 0, 0),
                        RGB(128, 128, 128), RGB(0, 128, 128), RGB(128, 0, 128),
                        RGB(0, 128, 0), RGB(255, 128, 0), RGB(192, 192, 192) };
    color.lpCustColors = cRef;
    color.lStructSize = sizeof(color);
    color.hwndOwner = hWndParent;
    color.Flags = CC_FULLOPEN;

    if(ChooseColor(&color)){
        return color.rgbResult;
    }else{
        return def;
    }
}

LRESULT CALLBACK ColorEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData){
   switch (uMsg){
      case WM_LBUTTONUP:{
         COLORREF res = OpenColorsDialog(hWnd, 0xFFFFFFFF);
         if(res != 0xFFFFFFFF){
            SendMessage(hWnd,EM_SETBKGNDCOLOR,0,res);
         }
         HWND hWndParent = GetParent(hWnd);
         SetFocus(hWndParent);
         break;
      }
   }
   return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK PreferencesProcPage1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static void **pointers = NULL;
    switch(uMsg){
      case WM_NOTIFY:{
        LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
        NMHDR nmhdr = lppsn->hdr;
        if(nmhdr.code == PSN_APPLY){
            iniparser_t *iniparser = pointers[0];
            wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

            char host[IRCPROTOCOL_SIZE_SMALL];
            char port[IRCPROTOCOL_SIZE_SMALL];
            char pass[IRCPROTOCOL_SIZE_SMALL];
            char user[IRCPROTOCOL_SIZE_SMALL];
            char name[IRCPROTOCOL_SIZE_SMALL];
            char nick[IRCPROTOCOL_SIZE_SMALL];
            int connect_on_startup;
            int reconnect_retries;

            gettext_tostr(hDlg, IDC_EDIT2, host, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT3, port, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT4, pass, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT5, user, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT6, name, IRCPROTOCOL_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT7, nick, IRCPROTOCOL_SIZE_MEDIUM);
            connect_on_startup = getint_fromcheck(hDlg,IDC_CHECK1);
            reconnect_retries = gettext_toint(hDlg,IDC_EDIT11);

            if(strlen(host)<=0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG1),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(strlen(port)<=0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG2),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(strlen(user)<=0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG3),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(strlen(name)<=0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG4),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(strlen(nick)<=0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG5),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(reconnect_retries < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG7),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }

            iniparser_setstring(iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, host);
            iniparser_setstring(iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, port);
            iniparser_setstring(iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, pass);
            iniparser_setstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, user);
            iniparser_setstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, name);
            iniparser_setstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, nick);
            iniparser_setint(iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, connect_on_startup);
            iniparser_setint(iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, reconnect_retries);
            if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
               client.config.connect_on_startup = connect_on_startup;
               client.config.reconnect_retries = reconnect_retries;
            }
            SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        break;
      }
      case WM_INITDIALOG:{
         PROPSHEETPAGE* params = (PROPSHEETPAGE*)lParam;

         pointers = (void**)params->lParam;
         iniparser_t *iniparser = pointers[0];
         wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

         settext_fromstr(hDlg,IDC_EDIT2,iniparser_getstring(iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, IRC_CONF_HOST_VAL));
         settext_fromstr(hDlg,IDC_EDIT3,iniparser_getstring(iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, IRC_CONF_PORT_VAL));
         settext_fromstr(hDlg,IDC_EDIT4,iniparser_getstring(iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, IRC_CONF_PASS_VAL));
         settext_fromstr(hDlg,IDC_EDIT5,iniparser_getstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, IRC_CONF_USER_VAL));
         settext_fromstr(hDlg,IDC_EDIT6,iniparser_getstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, IRC_CONF_NAME_VAL));
         settext_fromstr(hDlg,IDC_EDIT7,iniparser_getstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, IRC_CONF_NICK_VAL));
         if(wcscmp(parameters,IRC_CONST_MANAGER)==0){
            setcheck_fromint(hDlg,IDC_CHECK1,iniparser_getint(iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, IRC_CONF_STARTUP_VAL));
            settext_fromint(hDlg,IDC_EDIT11,iniparser_getint(iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, IRC_CONF_RETRIES_VAL));
         }else if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
            setcheck_fromint(hDlg,IDC_CHECK1,client.config.connect_on_startup);
            settext_fromint(hDlg,IDC_EDIT11,client.config.reconnect_retries);
         }
         break;
      }
    }
    return FALSE;
}

INT_PTR CALLBACK PreferencesProcPage2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static void **pointers = NULL;
    switch(uMsg){
      case WM_NOTIFY:{
        LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
        NMHDR nmhdr = lppsn->hdr;
        if(nmhdr.code == PSN_APPLY){
            iniparser_t *iniparser = pointers[0];
            //wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

            char perform[IRCPROTOCOL_SIZE_MEDIUM];
            char channels[IRCPROTOCOL_SIZE_MEDIUM];
            int delay;

            gettext_tostr(hDlg, IDC_EDIT8, perform, IRCPROTOCOL_SIZE_MEDIUM);
            gettext_tostr(hDlg, IDC_EDIT9, channels, IRCPROTOCOL_SIZE_MEDIUM);
            delay = gettext_toint(hDlg,IDC_EDIT10);

            if(delay < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG6),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }

            iniparser_setstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, perform);
            iniparser_setstring(iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, channels);
            iniparser_setint(iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, delay);
            SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        break;
      }
      case WM_INITDIALOG:{
         PROPSHEETPAGE* params = (PROPSHEETPAGE*)lParam;

         pointers = (void**)params->lParam;
         iniparser_t *iniparser = pointers[0];
         //wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

         settext_fromstr(hDlg,IDC_EDIT8,iniparser_getstring(iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, IRC_CONF_PERFORM_VAL));
         settext_fromstr(hDlg,IDC_EDIT9,iniparser_getstring(iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, IRC_CONF_CHANNELS_VAL));
         settext_fromint(hDlg,IDC_EDIT10,iniparser_getint(iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, IRC_CONF_DELAY_VAL));
         break;
      }
    }
    return FALSE;
}

INT_PTR CALLBACK PreferencesProcPage3(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static void **pointers = NULL;
    switch(uMsg){
      case WM_NOTIFY:{
        LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
        NMHDR nmhdr = lppsn->hdr;
        if(nmhdr.code == PSN_APPLY){
            iniparser_t *iniparser = pointers[0];
            wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

            char part[IRC_SIZE_SMALL];
            char kick[IRC_SIZE_SMALL];
            char quit[IRC_SIZE_SMALL];

            gettext_tostr(hDlg, IDC_EDIT12, part, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT13, kick, IRC_SIZE_SMALL);
            gettext_tostr(hDlg, IDC_EDIT14, quit, IRC_SIZE_SMALL);

            iniparser_setstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, part);
            iniparser_setstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, kick);
            iniparser_setstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, quit);

            if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
               strncpy0(client.config.part,part,IRC_SIZE_SMALL);
               strncpy0(client.config.kick,kick,IRC_SIZE_SMALL);
               strncpy0(client.config.quit,quit,IRC_SIZE_SMALL);
            }
            SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        break;
      }
      case WM_INITDIALOG:{
         PROPSHEETPAGE* params = (PROPSHEETPAGE*)lParam;

         pointers = (void**)params->lParam;
         iniparser_t *iniparser = pointers[0];
         wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

         if(wcscmp(parameters,IRC_CONST_MANAGER)==0){
            settext_fromstr(hDlg,IDC_EDIT12,iniparser_getstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, IRC_CONF_PART_VAL));
            settext_fromstr(hDlg,IDC_EDIT13,iniparser_getstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, IRC_CONF_KICK_VAL));
            settext_fromstr(hDlg,IDC_EDIT14,iniparser_getstring(iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, IRC_CONF_QUIT_VAL));
         }else if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
            settext_fromstr(hDlg,IDC_EDIT12,client.config.part);
            settext_fromstr(hDlg,IDC_EDIT13,client.config.kick);
            settext_fromstr(hDlg,IDC_EDIT14,client.config.quit);
         }
         break;
      }
    }
    return FALSE;
}

INT_PTR CALLBACK PreferencesProcPage4(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   static void **pointers = NULL;
   switch(uMsg){
      case WM_NOTIFY:{
        LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
        NMHDR nmhdr = lppsn->hdr;
        if(nmhdr.code == PSN_APPLY){
             iniparser_t *iniparser = pointers[0];
             wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

            int encoding;
            int bubble;
            int sounds;
            int led_number;
            int led_interval;

            int background_color;
            int font_color;
            int join_color;
            int nickchange_color;
            int topicchange_color;
            int mode_color;
            int notice_color;
            int part_color;
            int kick_color;
            int quit_color;

            encoding = getint_fromcombo(hDlg,IDC_COMBO1);
            bubble = gettext_toint(hDlg,IDC_EDIT15);
            sounds = getint_fromcheck(hDlg,IDC_CHECK2);
            led_number = gettext_toint(hDlg,IDC_EDIT16);
            led_interval = gettext_toint(hDlg,IDC_EDIT17);

            background_color = getcolor_fromrich(hDlg,IDC_EDIT30);
            font_color = getcolor_fromrich(hDlg,IDC_EDIT31);
            join_color = getcolor_fromrich(hDlg,IDC_EDIT32);
            nickchange_color = getcolor_fromrich(hDlg,IDC_EDIT33);
            topicchange_color = getcolor_fromrich(hDlg,IDC_EDIT34);
            mode_color = getcolor_fromrich(hDlg,IDC_EDIT35);
            notice_color = getcolor_fromrich(hDlg,IDC_EDIT36);
            part_color = getcolor_fromrich(hDlg,IDC_EDIT37);
            kick_color = getcolor_fromrich(hDlg,IDC_EDIT38);
            quit_color = getcolor_fromrich(hDlg,IDC_EDIT39);

            if(bubble < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG8),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }
            if(led_interval < 0){
               MessageBox(hDlg,MAKEINTSTR(IDS_ERROR_MSG9),NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
               SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
               return TRUE;
            }

            iniparser_setint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, encoding);
            iniparser_setint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, bubble);
            iniparser_setint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, sounds);
            iniparser_setint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, led_number);
            iniparser_setint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, led_interval);

             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_BACKGROUND_COLOR, background_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_FONT_COLOR, font_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_JOIN_COLOR, join_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_NICKCHANGE_COLOR, nickchange_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_TOPICCHANGE_COLOR, topicchange_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_MODE_COLOR, mode_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_NOTICE_COLOR, notice_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_PART_COLOR, part_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_KICK_COLOR, kick_color);
             iniparser_setint(iniparser, IRC_CONF_COLORS, IRC_CONF_QUIT_COLOR, quit_color);

            if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
               if(encoding!=0){
                  client.config.encoding=CP_UTF8;
               }else{
                  client.config.encoding=CP_ACP;
               }
               client.config.bubble=bubble;
               client.config.sounds=sounds;
               client.config.led_number=led_number;
               client.config.led_interval=led_interval;

                client.config.background_color=background_color;
                client.config.font_color=font_color;
                client.config.join_color=join_color;
                client.config.nickchange_color=nickchange_color;
                client.config.topicchange_color=topicchange_color;
                client.config.mode_color=mode_color;
                client.config.notice_color=notice_color;
                client.config.part_color=part_color;
                client.config.kick_color=kick_color;
                client.config.quit_color=quit_color;
            }
            
            SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        break;
      }
      case WM_INITDIALOG:{
         PROPSHEETPAGE* params = (PROPSHEETPAGE*)lParam;

         pointers = (void**)params->lParam;
         iniparser_t *iniparser = pointers[0];
         wchar_t *parameters = pointers[1]; //"MANAGER" or "CLIENT", profile

         if(wcscmp(parameters,IRC_CONST_MANAGER)==0){
            setcombo_fromint(hDlg,IDC_COMBO1,iniparser_getint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, IRC_CONF_ENCODING_VAL));
            settext_fromint(hDlg,IDC_EDIT15,iniparser_getint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, IRC_CONF_BUBBLE_VAL));
            setcheck_fromint(hDlg,IDC_CHECK2,iniparser_getint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, IRC_CONF_SOUNDS_VAL));
            settext_fromint(hDlg,IDC_EDIT16,iniparser_getint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, IRC_CONF_LEDNUMBER_VAL));
            settext_fromint(hDlg,IDC_EDIT17,iniparser_getint(iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, IRC_CONF_LEDINTERVAL_VAL));

             setcolor_fromrich(hDlg,IDC_EDIT30,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_BACKGROUND_COLOR, IRC_CONF_BACKGROUND_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT31,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_FONT_COLOR, IRC_CONF_FONT_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT32,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_JOIN_COLOR, IRC_CONF_JOIN_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT33,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_NICKCHANGE_COLOR, IRC_CONF_NICKCHANGE_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT34,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_TOPICCHANGE_COLOR, IRC_CONF_TOPICCHANGE_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT35,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_MODE_COLOR, IRC_CONF_MODE_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT36,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_NOTICE_COLOR, IRC_CONF_NOTICE_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT37,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_PART_COLOR, IRC_CONF_PART_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT38,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_KICK_COLOR, IRC_CONF_KICK_COLOR_VAL));
             setcolor_fromrich(hDlg,IDC_EDIT39,iniparser_getint(iniparser, IRC_CONF_COLORS, IRC_CONF_QUIT_COLOR, IRC_CONF_QUIT_COLOR_VAL));
         }else if(wcscmp(parameters,IRC_CONST_CLIENT)==0){
            if(client.config.encoding==CP_UTF8){
               setcombo_fromint(hDlg,IDC_COMBO1,1);
            }else{
               setcombo_fromint(hDlg,IDC_COMBO1,0);
            }
            settext_fromint(hDlg,IDC_EDIT15,client.config.bubble);
            setcheck_fromint(hDlg,IDC_CHECK2,client.config.sounds);
            settext_fromint(hDlg,IDC_EDIT16,client.config.led_number);
            settext_fromint(hDlg,IDC_EDIT17,client.config.led_interval);

             setcolor_fromrich(hDlg,IDC_EDIT30,client.config.background_color);
             setcolor_fromrich(hDlg,IDC_EDIT31,client.config.font_color);
             setcolor_fromrich(hDlg,IDC_EDIT32,client.config.join_color);
             setcolor_fromrich(hDlg,IDC_EDIT33,client.config.nickchange_color);
             setcolor_fromrich(hDlg,IDC_EDIT34,client.config.topicchange_color);
             setcolor_fromrich(hDlg,IDC_EDIT35,client.config.mode_color);
             setcolor_fromrich(hDlg,IDC_EDIT36,client.config.notice_color);
             setcolor_fromrich(hDlg,IDC_EDIT37,client.config.part_color);
             setcolor_fromrich(hDlg,IDC_EDIT38,client.config.kick_color);
             setcolor_fromrich(hDlg,IDC_EDIT39,client.config.quit_color);
         }
         
         HWND rich30 = GetDlgItem(hDlg,IDC_EDIT30);
         SetWindowSubclass(rich30, ColorEditProc, IDC_EDIT30, 0);
         HWND rich31 = GetDlgItem(hDlg,IDC_EDIT31);
         SetWindowSubclass(rich31, ColorEditProc, IDC_EDIT31, 0);
         HWND rich32 = GetDlgItem(hDlg,IDC_EDIT32);
         SetWindowSubclass(rich32, ColorEditProc, IDC_EDIT32, 0);
         HWND rich33 = GetDlgItem(hDlg,IDC_EDIT33);
         SetWindowSubclass(rich33, ColorEditProc, IDC_EDIT33, 0);
         HWND rich34 = GetDlgItem(hDlg,IDC_EDIT34);
         SetWindowSubclass(rich34, ColorEditProc, IDC_EDIT34, 0);
         HWND rich35 = GetDlgItem(hDlg,IDC_EDIT35);
         SetWindowSubclass(rich35, ColorEditProc, IDC_EDIT35, 0);
         HWND rich36 = GetDlgItem(hDlg,IDC_EDIT36);
         SetWindowSubclass(rich36, ColorEditProc, IDC_EDIT36, 0);
         HWND rich37 = GetDlgItem(hDlg,IDC_EDIT37);
         SetWindowSubclass(rich37, ColorEditProc, IDC_EDIT37, 0);
         HWND rich38 = GetDlgItem(hDlg,IDC_EDIT38);
         SetWindowSubclass(rich38, ColorEditProc, IDC_EDIT38, 0);
         HWND rich39 = GetDlgItem(hDlg,IDC_EDIT39);
         SetWindowSubclass(rich39, ColorEditProc, IDC_EDIT39, 0);
         break;
      }
   }
   return FALSE;
}

INT_PTR WINAPI OpenPreferencesDialog(HWND hWndParent, LPARAM dwInitParam) {
    wchar_t **parameters = (wchar_t**)dwInitParam;
    iniparser_t iniparser;

    if(iniparser_init(&iniparser)!=0){
        return FALSE;
    }

    if(winiparser_load(&iniparser,parameters[1])!=0){
        iniparser_destroy(&iniparser);
        return FALSE;
    }

    void *pointers[2] = {(void*)&iniparser, parameters[0]};

    PROPSHEETPAGE psp[4];
    memset(&psp,0,sizeof(psp));

    psp[0].dwSize      = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[0].hInstance   = config.h_instance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDT_TAB1);
    psp[0].pszIcon     = NULL;
    psp[0].pfnDlgProc  = PreferencesProcPage1;
    psp[0].pszTitle    = L"Server and ID";
    psp[0].lParam      = (LPARAM)pointers;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize      = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[1].hInstance   = config.h_instance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDT_TAB2);
    psp[1].pszIcon     = NULL;
    psp[1].pfnDlgProc  = PreferencesProcPage2;
    psp[1].pszTitle    = L"Perform";
    psp[1].lParam      = (LPARAM)pointers;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize      = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[2].hInstance   = config.h_instance;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDT_TAB3);
    psp[2].pszIcon     = NULL;
    psp[2].pfnDlgProc  = PreferencesProcPage3;
    psp[2].pszTitle    = L"Messages";
    psp[2].lParam      = (LPARAM)pointers;
    psp[2].pfnCallback = NULL;

    psp[3].dwSize      = sizeof(PROPSHEETPAGE);
    psp[3].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[3].hInstance   = config.h_instance;
    psp[3].pszTemplate = MAKEINTRESOURCE(IDT_TAB4);
    psp[3].pszIcon     = NULL;
    psp[3].pfnDlgProc  = PreferencesProcPage4;
    psp[3].pszTitle    = L"Colors and Misc";
    psp[3].lParam      = (LPARAM)pointers;
    psp[3].pfnCallback = NULL; 

    PROPSHEETHEADER psh;
    memset(&psh,0,sizeof(psh));

    psh.dwSize      = sizeof(PROPSHEETHEADER);
    psh.dwFlags     = PSH_USEICONID | PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP | PSH_PROPTITLE | DS_CENTER;
    psh.hwndParent  = hWndParent;
    psh.hInstance   = config.h_instance;
    psh.pszIcon     = NULL;
    psh.pszCaption  = parameters[1];
    psh.nPages      = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage  = 0;
    psh.ppsp        = (LPCPROPSHEETPAGE) &psp;

    // PSP_USECALLBACK, PSH_USECALLBACK, MAKEINTRESOURCE(IDI_BORDER);
    int result = PropertySheet(&psh);
    if(result > 0){
        if(winiparser_store(&iniparser,parameters[1])!=0){
            iniparser_destroy(&iniparser);
            return FALSE;
        }
        iniparser_destroy(&iniparser);
        return TRUE;
     }else{
        iniparser_destroy(&iniparser);
        return FALSE;
     }
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
