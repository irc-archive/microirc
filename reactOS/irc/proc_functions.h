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

INT_PTR CALLBACK PreferencesProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
//if(TRUE)return FALSE;

    static wchar_t type[IRC_SIZE_SMALL];
    static wchar_t profile[IRC_SIZE_SMALL];
    static char host[IRCPROTOCOL_SIZE_SMALL];
    static char port[IRCPROTOCOL_SIZE_SMALL];
    static char pass[IRCPROTOCOL_SIZE_SMALL];
    static char user[IRCPROTOCOL_SIZE_SMALL];
    static char name[IRCPROTOCOL_SIZE_SMALL];
    static char nick[IRCPROTOCOL_SIZE_SMALL];
    static char perform[IRCPROTOCOL_SIZE_MEDIUM];
    static char channels[IRCPROTOCOL_SIZE_MEDIUM];
    static int delay;
    static int connect_on_startup;
    static int reconnect_retries;
    static char part[IRC_SIZE_SMALL];
    static char kick[IRC_SIZE_SMALL];
    static char quit[IRC_SIZE_SMALL];
    static int encoding;
    static int bubble;
    static int sounds;
    static int led_number;
    static int led_interval;

   switch(uMsg){
      case WM_DESTROY:{
        MessageBox(NULL,L"gravou",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
            iniparser_t iniparser;
            if(iniparser_init(&iniparser)!=0){
               return TRUE;
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
               return TRUE;
            }
            iniparser_destroy(&iniparser);
            
            return FALSE;
      }
      case WM_NOTIFY:{
        LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
        NMHDR nmhdr = lppsn->hdr;
        //if(nmhdr.code == PSN_KILLACTIVE){
        //    MessageBox(NULL,L"killactive",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
        //}
        if(nmhdr.code == PSN_APPLY){
            MessageBox(NULL,L"apply",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
        }
        break;
      }
      case WM_INITDIALOG:{
        MessageBox(NULL,L"init",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
         //SetScrollRange(hDlg,SB_VERT,SCROLL_PREFERENCES_MIN_POSITIONS,SCROLL_PREFERENCES_MAX_POSITIONS,TRUE);
         //UpdateWindow(hDlg);
         
         PROPSHEETPAGE* params = (PROPSHEETPAGE*)lParam;
         wchar_t **parameters = (wchar_t**)params->lParam;//"MANAGER" or "CLIENT", profile
         
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

    PROPSHEETPAGE psp[4];

    void *page0[3] = {&iniparser, L"0", parameters[0], parameters[1]};
    psp[0].dwSize      = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags     = PSP_USEICONID | PSP_USETITLE; //PSP_USECALLBACK
    psp[0].hInstance   = config.h_instance;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDT_TAB1);
    psp[0].pszIcon     = NULL; //MAKEINTRESOURCE(IDI_FONT);
    psp[0].pfnDlgProc  = PreferencesProc;
    psp[0].pszTitle    = L"0";
    psp[0].lParam      = (LPARAM)page0;
    psp[0].pfnCallback = NULL;

    wchar_t *page1[3] = {&iniparser, L"1", parameters[0], parameters[1]};
    psp[1].dwSize      = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags     = PSP_USEICONID | PSP_USETITLE; //PSP_USECALLBACK
    psp[1].hInstance   = config.h_instance;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDT_TAB2);
    psp[1].pszIcon     = NULL; //MAKEINTRESOURCE(IDI_BORDER);
    psp[1].pfnDlgProc  = PreferencesProc;
    psp[1].pszTitle    = L"1";
    psp[1].lParam      = (LPARAM)page1;
    psp[1].pfnCallback = NULL;

    wchar_t *page2[3] = {&iniparser, L"2", parameters[0], parameters[1]};
    psp[2].dwSize      = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags     = PSP_USEICONID | PSP_USETITLE; //PSP_USECALLBACK
    psp[2].hInstance   = config.h_instance;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDT_TAB3);
    psp[2].pszIcon     = NULL; //MAKEINTRESOURCE(IDI_BORDER);
    psp[2].pfnDlgProc  = PreferencesProc;
    psp[2].pszTitle    = L"2";
    psp[2].lParam      = (LPARAM)page2;
    psp[2].pfnCallback = NULL;

    wchar_t *page3[3] = {&iniparser, L"3", parameters[0], parameters[1]};
    psp[3].dwSize      = sizeof(PROPSHEETPAGE);
    psp[3].dwFlags     = PSP_USEICONID | PSP_USETITLE; //PSP_USECALLBACK
    psp[3].hInstance   = config.h_instance;
    psp[3].pszTemplate = MAKEINTRESOURCE(IDT_TAB4);
    psp[3].pszIcon     = NULL; //MAKEINTRESOURCE(IDI_BORDER);
    psp[3].pfnDlgProc  = PreferencesProc;
    psp[3].pszTitle    = L"3";
    psp[3].lParam      = (LPARAM)page3;
    psp[3].pfnCallback = NULL; 

    PROPSHEETHEADER psh;

    psh.dwSize      = sizeof(PROPSHEETHEADER);
    psh.dwFlags     = PSH_USEICONID | PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP | PSH_PROPTITLE; // | PSH_USECALLBACK;
    psh.hwndParent  = hWndParent;
    psh.hInstance   = config.h_instance;
    psh.pszIcon     = NULL;//MAKEINTRESOURCE(IDI_CELL_PROPERTIES);
    psh.pszCaption  = L"Cell Properties";
    psh.nPages      = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage  = 0;
    psh.ppsp        = (LPCPROPSHEETPAGE) &psp;
    //psh.pfnCallback = TESTEPROC;

    int result = PropertySheet(&psh);

    if(winiparser_store(&iniparser,profile)!=0){
        iniparser_destroy(&iniparser);
        return FALSE;
    }
    iniparser_destroy(&iniparser);
    
    if(result > 0){
        //MessageBox(NULL,L"GUD",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
        return TRUE;
     }else{
        //MessageBox(NULL,L"BAD",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
        return FALSE;
     }
    
/*MSG msg;
BOOL bret;
while ((bret = GetMessage (&msg, NULL, 0, 0)) != 0){
  if(bret == -1){
    //Your error handler
    return false;
  }
  if(!PropSheet_IsDialogMessage (GetHWND(), &msg)){
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }
  if (PropSheet_GetCurrentPageHwnd(GetHWND()) == NULL){  
    // User pressed OK or Cancel
    break;
  }
}*/
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
