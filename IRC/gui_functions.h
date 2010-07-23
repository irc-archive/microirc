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
   mbi.nToolBarId = IDR_MENU;
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
   hWnd_ButtonConnect = CreateWindowEx(0,L"button",TEXT("Connect"),WS_CHILD|WS_VISIBLE|WS_BORDER,19,125,200, 20, hWnd, (HMENU)BUTTON_CONNECT, hInstance_Main, NULL);
   UpdateWindow(hWnd);
}

void destroy_login_menu(HWND hWnd){
   DestroyWindow(hWnd_ButtonConnect);
   UpdateWindow(hWnd);
}

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
   hWnd_StaticConnecting = CreateWindowEx(0,L"static",TEXT("CONNECTING...."),WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,width,height, hWnd, (HMENU)BUTTON_CONNECT, hInstance_Main, NULL);
   UpdateWindow(hWnd);
}

void destroy_loading_screen(HWND hWnd){
   DestroyWindow(hWnd_StaticConnecting);
   SendMessage(hWnd,WM_UNLOAD_CURSOR,0,0);
   UpdateWindow(hWnd);
}

WNDPROC edit_OldEditHandler;
HWND edit_hWnd_Main;
LRESULT CALLBACK EditProc(HWND hWnd, UINT event_id, WPARAM element_id, LPARAM param_id){
   switch (event_id){
      case WM_CHAR:{
         if(element_id == VK_RETURN){
            SendMessage(edit_hWnd_Main,WM_COMMAND,BUTTON_SEND,0);
            return 0;
         }
         break;
      }
   }
   return CallWindowProc(edit_OldEditHandler, hWnd, event_id, element_id, param_id);
}

void init_chat_screen(HWND hWnd){
   INITCOMMONCONTROLSEX icex;
   hWnd_EditChat = CreateWindowEx(0,L"edit", NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,2,246,197,20,hWnd,(HMENU)TEXT_SEND,hInstance_Main,NULL);
   
   edit_hWnd_Main = hWnd;
   edit_OldEditHandler = (WNDPROC)GetWindowLong(hWnd_EditChat,GWL_WNDPROC);
   SetWindowLong(hWnd_EditChat,GWL_WNDPROC,(LONG)EditProc);
   
   Edit_LineLength(hWnd_EditChat,200);
   Edit_LimitText(hWnd_EditChat,200);
   hWnd_ButtonChat = CreateWindowEx(0,L"button",TEXT("Send"),WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,200,246,38,20,hWnd,(HMENU)BUTTON_SEND,hInstance_Main,NULL);
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_TAB_CLASSES;
   InitCommonControlsEx(&icex);
   hWnd_TabControlChat = CreateWindowEx(0,WC_TABCONTROL,NULL,WS_CHILD|WS_VISIBLE,0,0,220,23,hWnd,(HMENU)TAB_CONTROL,hInstance_Main,NULL);//240,245
   hWnd_CloseTab = CreateWindowEx(0,L"button",TEXT("X"),WS_VISIBLE|WS_CHILD,223,5,14,14,hWnd,(HMENU)BUTTON_CLOSE,hInstance_Main,NULL);
   hWnd_TapAndHold = CreateWindow(WC_SIPPREF, L"", WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance_Main, NULL);
   UpdateWindow(hWnd);
}

void destroy_chat_screen(HWND hWnd){
   DestroyWindow(hWnd_TapAndHold);
   DestroyWindow(hWnd_EditChat);
   DestroyWindow(hWnd_ButtonChat);
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
