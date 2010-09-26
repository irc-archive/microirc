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

void init_menu_bar(HWND hWnd, int barId){
   SHMENUBARINFO mbi;
   memset(&mbi, 0, sizeof(SHMENUBARINFO));
   mbi.cbSize = sizeof(SHMENUBARINFO);
   mbi.hwndParent = hWnd;
   mbi.nToolBarId = barId;
   mbi.hInstRes = app_instance;
   if(!SHCreateMenuBar(&mbi)){
      menu_bar_handle = NULL;
   }else{
      menu_bar_handle = mbi.hwndMB;
   }
   UpdateWindow(hWnd);
}

void destroy_menu_bar(HWND hWnd){
   CommandBar_Destroy(menu_bar_handle);
   UpdateWindow(hWnd);
}

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
   UpdateWindow(hWnd);
}

void destroy_loading_screen(HWND hWnd){
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
   edit_chatinput_handle = CreateWindowEx(0,L"edit", NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_NOHIDESEL,EDITCHAT_LEFT,EDITCHAT_TOP,EDITCHAT_WIDTH,EDITCHAT_HEIGHT,hWnd,(HMENU)NULL,app_instance,NULL);
   
   old_ChatSendProc = (WNDPROC)GetWindowLong(edit_chatinput_handle,GWL_WNDPROC);
   SetWindowLong(edit_chatinput_handle,GWL_WNDPROC,(LONG)ChatSendProc);
   
   Edit_LineLength(edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   Edit_LimitText(edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   button_chatsend_handle = CreateWindowEx(0,L"button",TEXT("Send"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|BS_CENTER|BS_VCENTER,BUTTONCHAT_LEFT,BUTTONCHAT_TOP,BUTTONCHAT_WIDTH,BUTTONCHAT_HEIGHT,hWnd,(HMENU)BUTTON_CHATSEND,app_instance,NULL);
   tabcontrol_chatview_handle = CreateWindowEx(0,WC_TABCONTROL,NULL,WS_CHILD|WS_VISIBLE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,TABCONTROLCHAT_LEFT,TABCONTROLCHAT_TOP,TABCONTROLCHAT_WIDTH,TABCONTROLCHAT_HEIGHT,hWnd,(HMENU)TABCONTROL_CHATVIEW,app_instance,NULL);
   button_closetab_handle = CreateWindowEx(0,L"button",TEXT("x"),WS_CHILD|WS_VISIBLE|BS_CENTER|BS_VCENTER,CLOSETAB_LEFT,CLOSETAB_TOP,CLOSETAB_WIDTH,CLOSETAB_HEIGHT,hWnd,(HMENU)BUTTON_CLOSETAB,app_instance,NULL);
   sippref_handle = CreateWindowEx(0,WC_SIPPREF,L"",WS_CHILD,0,0,0,0,hWnd,(HMENU)NULL,app_instance,NULL);
   UpdateWindow(hWnd);
}

void destroy_chat_screen(HWND hWnd){
   DestroyWindow(sippref_handle);
   DestroyWindow(edit_chatinput_handle);
   DestroyWindow(button_chatsend_handle);
   while(tab_delete_current(tabcontrol_chatview_handle)!=-1);
   DestroyWindow(tabcontrol_chatview_handle);
   DestroyWindow(button_closetab_handle);
   UpdateWindow(hWnd);
}

int open_input_box(HWND parent_window, wchar_t *title, wchar_t *text, wchar_t *result, unsigned int result_len){
   //result will be null string if DialogBoxParam fails or user provides no text
   wchar_t *titletext[2]={title,text};
   wchar_t *temp = (wchar_t*)DialogBoxParam(app_instance, (LPCTSTR)IDD_INPUTBOX, parent_window, InputBoxProc, (LPARAM)titletext);
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
