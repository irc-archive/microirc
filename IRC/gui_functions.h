/*
* gui_functions.h
*
* This file contains some gui functions for irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void init_menu_bar(HWND hWnd){
   SHACTIVATEINFO s_sai;
   SHMENUBARINFO mbi;
   memset(&mbi, 0, sizeof(SHMENUBARINFO));
   mbi.cbSize = sizeof(SHMENUBARINFO);
   mbi.hwndParent = hWnd;
   mbi.nToolBarId = IDR_MAIN_MENU;
   mbi.hInstRes = hInstance_Main;
   if (!SHCreateMenuBar(&mbi)){
      hWnd_MenuBar = NULL;
   }else{
      hWnd_MenuBar = mbi.hwndMB;
   }
   memset(&s_sai, 0, sizeof (s_sai));
   s_sai.cbSize = sizeof (s_sai);
   UpdateWindow(hWnd);
}

void destroy_menu_bar(HWND hWnd){
   CommandBar_Destroy(hWnd_MenuBar);
   UpdateWindow(hWnd);
}

void init_login_menu(HWND hWnd){
   hWnd_ButtonConnect = CreateWindowEx(0,L"button",TEXT("Connect"),WS_CHILD|WS_VISIBLE|WS_BORDER,BUTTONCONNECT_LEFT*width,BUTTONCONNECT_TOP*height,BUTTONCONNECT_WIDTH*width,BUTTONCONNECT_HEIGHT*height,hWnd,(HMENU)BUTTON_CONNECT,hInstance_Main,NULL);
   UpdateWindow(hWnd);
}

void destroy_login_menu(HWND hWnd){
   DestroyWindow(hWnd_ButtonConnect);
   UpdateWindow(hWnd);
}

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
   hWnd_StaticConnecting = CreateWindowEx(0,L"static",TEXT("CONNECTING...."),WS_CHILD|WS_VISIBLE|SS_CENTER,STATICCONNECTING_LEFT*width,STATICCONNECTING_TOP*height,STATICCONNECTING_WIDTH*width,STATICCONNECTING_HEIGHT*height,hWnd,(HMENU)BUTTON_CONNECT,hInstance_Main,NULL);
   UpdateWindow(hWnd);
}

void destroy_loading_screen(HWND hWnd){
   DestroyWindow(hWnd_StaticConnecting);
   SendMessage(hWnd,WM_UNLOAD_CURSOR,0,0);
   UpdateWindow(hWnd);
}

WNDPROC old_SendTextProc;
HWND hWnd_SendText;
LRESULT CALLBACK SendTextProc(HWND hWnd, UINT event_id, WPARAM element_id, LPARAM param_id){
   switch (event_id){
      case WM_CHAR:{
         if(element_id == VK_RETURN){
            SendMessage(hWnd_SendText,WM_COMMAND,BUTTON_SEND,0);
            return 0;
         }
         break;
      }
   }
   return CallWindowProc(old_SendTextProc, hWnd, event_id, element_id, param_id);
}

void init_chat_screen(HWND hWnd){
   INITCOMMONCONTROLSEX icex;
   hWnd_EditChat = CreateWindowEx(0,L"edit", NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_NOHIDESEL,EDITCHAT_LEFT*width,EDITCHAT_TOP*height,EDITCHAT_WIDTH*width,EDITCHAT_HEIGHT*height,hWnd,NULL,hInstance_Main,NULL);
   
   hWnd_SendText = hWnd;
   old_SendTextProc = (WNDPROC)GetWindowLong(hWnd_EditChat,GWL_WNDPROC);
   SetWindowLong(hWnd_EditChat,GWL_WNDPROC,(LONG)SendTextProc);
   
   Edit_LineLength(hWnd_EditChat,200);
   Edit_LimitText(hWnd_EditChat,200);
   hWnd_ButtonChat = CreateWindowEx(0,L"button",TEXT("Send"),WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_CENTER|BS_VCENTER,BUTTONCHAT_LEFT*width,BUTTONCHAT_TOP*height,BUTTONCHAT_WIDTH*width,BUTTONCHAT_HEIGHT*height,hWnd,(HMENU)BUTTON_SEND,hInstance_Main,NULL);
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_TAB_CLASSES;
   InitCommonControlsEx(&icex);
   hWnd_TabControlChat = CreateWindowEx(0,WC_TABCONTROL,NULL,WS_CHILD|WS_VISIBLE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,TABCONTROLCHAT_LEFT*width,TABCONTROLCHAT_TOP*height,TABCONTROLCHAT_WIDTH*width,TABCONTROLCHAT_HEIGHT*height,hWnd,(HMENU)TAB_CONTROL,hInstance_Main,NULL);
   hWnd_CloseTab = CreateWindowEx(0,L"button",TEXT("x"),WS_VISIBLE|WS_CHILD|BS_CENTER|BS_VCENTER,CLOSETAB_LEFT*width,CLOSETAB_TOP*height,CLOSETAB_WIDTH*width,CLOSETAB_HEIGHT*height,hWnd,(HMENU)BUTTON_CLOSE,hInstance_Main,NULL);
   hWnd_TapAndHold = CreateWindowEx(0,WC_SIPPREF,L"",WS_CHILD,0,0,0,0,hWnd,NULL,hInstance_Main,NULL);
   UpdateWindow(hWnd);
}

void destroy_chat_screen(HWND hWnd){
   DestroyWindow(hWnd_TapAndHold);
   DestroyWindow(hWnd_EditChat);
   DestroyWindow(hWnd_ButtonChat);
   while(tab_delete_actual(hWnd_TabControlChat)!=-1);
   DestroyWindow(hWnd_TabControlChat);
   DestroyWindow(hWnd_CloseTab);
   UpdateWindow(hWnd);
}

int open_input_box(HWND parent_window, wchar_t *title, wchar_t *result, unsigned result_len){
   //result will be null string if DialogBoxParam fails or user provides no text
   wchar_t *temp = (wchar_t*)DialogBoxParam(hInstance_Main, (LPCTSTR)IDD_INPUTBOX, parent_window, InputBox, (LPARAM)title);
   if(temp!=NULL){
      //copy result to the provided buffer and deallocate
      wcsncpy(result, temp, result_len-1);
      free(temp);
      return 0;
   }else{
      *result = L'\0';
      return -1;
   }
}
