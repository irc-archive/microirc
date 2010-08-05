/*
* gui_functions.h
*
* This file contains some gui functions for irc client.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, Jos� Pedroso <josedpedroso@gmail.com>
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
   mbi.hInstRes = app_instance;
   if (!SHCreateMenuBar(&mbi)){
      menu_bar_handle = NULL;
   }else{
      menu_bar_handle = mbi.hwndMB;
   }
   memset(&s_sai, 0, sizeof (s_sai));
   s_sai.cbSize = sizeof (s_sai);
   UpdateWindow(hWnd);
}

void destroy_menu_bar(HWND hWnd){
   CommandBar_Destroy(menu_bar_handle);
   UpdateWindow(hWnd);
}

void init_login_menu(HWND hWnd){
   button_connect_handle = CreateWindowEx(0,L"button",TEXT("Connect"),WS_CHILD|WS_VISIBLE|WS_BORDER,BUTTONCONNECT_LEFT*window_width,BUTTONCONNECT_TOP*window_height,BUTTONCONNECT_WIDTH*window_width,BUTTONCONNECT_HEIGHT*window_height,hWnd,(HMENU)BUTTON_CONNECT,app_instance,NULL);
   UpdateWindow(hWnd);
}

void destroy_login_menu(HWND hWnd){
   DestroyWindow(button_connect_handle);
   UpdateWindow(hWnd);
}

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
   static_connecting_handle = CreateWindowEx(0,L"static",TEXT("CONNECTING...."),WS_CHILD|WS_VISIBLE|SS_CENTER,STATICCONNECTING_LEFT*window_width,STATICCONNECTING_TOP*window_height,STATICCONNECTING_WIDTH*window_width,STATICCONNECTING_HEIGHT*window_height,hWnd,(HMENU)BUTTON_CONNECT,app_instance,NULL);
   UpdateWindow(hWnd);
}

void destroy_loading_screen(HWND hWnd){
   DestroyWindow(static_connecting_handle);
   SendMessage(hWnd,WM_UNLOAD_CURSOR,0,0);
   UpdateWindow(hWnd);
}

WNDPROC old_ChatSendProc;
LRESULT CALLBACK ChatSendProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
   switch (uMsg){
      case WM_CHAR:{
         if(wParam == VK_RETURN){
            SendMessage(GetParent(hWnd),WM_COMMAND,BUTTON_CHATSEND,0);
            return 0;
         }
         break;
      }
   }
   return CallWindowProc(old_ChatSendProc, hWnd, uMsg, wParam, lParam);
}

void init_chat_screen(HWND hWnd){
   INITCOMMONCONTROLSEX icex;
   edit_chatinput_handle = CreateWindowEx(0,L"edit", NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_NOHIDESEL,EDITCHAT_LEFT*window_width,EDITCHAT_TOP*window_height,EDITCHAT_WIDTH*window_width,EDITCHAT_HEIGHT*window_height,hWnd,(HMENU)NULL,app_instance,NULL);
   
   old_ChatSendProc = (WNDPROC)GetWindowLong(edit_chatinput_handle,GWL_WNDPROC);
   SetWindowLong(edit_chatinput_handle,GWL_WNDPROC,(LONG)ChatSendProc);
   
   Edit_LineLength(edit_chatinput_handle,200);
   Edit_LimitText(edit_chatinput_handle,200);
   button_chatsend_handle = CreateWindowEx(0,L"button",TEXT("Send"),WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_CENTER|BS_VCENTER,BUTTONCHAT_LEFT*window_width,BUTTONCHAT_TOP*window_height,BUTTONCHAT_WIDTH*window_width,BUTTONCHAT_HEIGHT*window_height,hWnd,(HMENU)BUTTON_CHATSEND,app_instance,NULL);
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_TAB_CLASSES;
   InitCommonControlsEx(&icex);
   tabcontrol_chatview_handle = CreateWindowEx(0,WC_TABCONTROL,NULL,WS_CHILD|WS_VISIBLE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,TABCONTROLCHAT_LEFT*window_width,TABCONTROLCHAT_TOP*window_height,TABCONTROLCHAT_WIDTH*window_width,TABCONTROLCHAT_HEIGHT*window_height,hWnd,(HMENU)TABCONTROL_CHATVIEW,app_instance,NULL);
   button_closetab_handle = CreateWindowEx(0,L"button",TEXT("x"),WS_VISIBLE|WS_CHILD|BS_CENTER|BS_VCENTER,CLOSETAB_LEFT*window_width,CLOSETAB_TOP*window_height,CLOSETAB_WIDTH*window_width,CLOSETAB_HEIGHT*window_height,hWnd,(HMENU)BUTTON_CLOSETAB,app_instance,NULL);
   loadcursor_handle = CreateWindowEx(0,WC_SIPPREF,L"",WS_CHILD,0,0,0,0,hWnd,(HMENU)NULL,app_instance,NULL);
   UpdateWindow(hWnd);
}

void destroy_chat_screen(HWND hWnd){
   DestroyWindow(loadcursor_handle);
   DestroyWindow(edit_chatinput_handle);
   DestroyWindow(button_chatsend_handle);
   while(tab_delete_current(tabcontrol_chatview_handle)!=-1);
   DestroyWindow(tabcontrol_chatview_handle);
   DestroyWindow(button_closetab_handle);
   UpdateWindow(hWnd);
}

int open_input_box(HWND parent_window, wchar_t *title, wchar_t *result, unsigned result_len){
   //result will be null string if DialogBoxParam fails or user provides no text
   wchar_t *temp = (wchar_t*)DialogBoxParam(app_instance, (LPCTSTR)IDD_INPUTBOX, parent_window, InputBoxProc, (LPARAM)title);
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
