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
   /*SHMENUBARINFO mbi;
   memset(&mbi, 0, sizeof(SHMENUBARINFO));
   mbi.cbSize = sizeof(SHMENUBARINFO);
   mbi.hwndParent = hWnd;
   mbi.nToolBarId = barId;
   mbi.hInstRes = app_instance;
   if(!SHCreateMenuBar(&mbi)){
      menu_bar_handle = NULL;
   }else{
      menu_bar_handle = mbi.hwndMB;
   }*/
}

/*void destroy_menu_bar(){
   CommandBar_Destroy(menu_bar_handle);
}*/

void init_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_LOAD_CURSOR,0,0);
}

void destroy_loading_screen(HWND hWnd){
   SendMessage(hWnd,WM_UNLOAD_CURSOR,0,0);
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
   client.edit_chatinput_handle = CreateWindowEx(0,L"EDIT",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_NOHIDESEL,resize.EDITCHAT_LEFT,resize.EDITCHAT_TOP,resize.EDITCHAT_WIDTH,resize.EDITCHAT_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
   client.button_chatsend_handle = CreateWindowEx(0,L"BUTTON",TEXT("Send"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|BS_CENTER|BS_VCENTER,resize.BUTTONCHAT_LEFT,resize.BUTTONCHAT_TOP,resize.BUTTONCHAT_WIDTH,resize.BUTTONCHAT_HEIGHT,hWnd,(HMENU)BUTTON_CHATSEND,config.h_instance,NULL);
   client.tabcontrol_chatview_handle = CreateWindowEx(0,L"SYSTABCONTROL32",NULL,WS_CHILD|WS_VISIBLE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,resize.TABCONTROLCHAT_LEFT,resize.TABCONTROLCHAT_TOP,resize.TABCONTROLCHAT_WIDTH,resize.TABCONTROLCHAT_HEIGHT,hWnd,(HMENU)TABCONTROL_CHATVIEW,config.h_instance,NULL);
   client.button_closetab_handle = CreateWindowEx(0,L"BUTTON",TEXT("x"),WS_CHILD|WS_VISIBLE|BS_CENTER|BS_VCENTER,resize.CLOSETAB_LEFT,resize.CLOSETAB_TOP,resize.CLOSETAB_WIDTH,resize.CLOSETAB_HEIGHT,hWnd,(HMENU)BUTTON_CLOSETAB,config.h_instance,NULL);
   client.sippref_handle = CreateWindowEx(0,L"SIPPREF",NULL,WS_CHILD,0,0,0,0,hWnd,(HMENU)NULL,config.h_instance,NULL);

   old_ChatSendProc = (WNDPROC)GetWindowLong(client.edit_chatinput_handle,GWL_WNDPROC);
   SetWindowLong(client.edit_chatinput_handle,GWL_WNDPROC,(LONG)ChatSendProc);

   Edit_LineLength(client.edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   Edit_LimitText(client.edit_chatinput_handle,EDITCHATINPUT_LIMIT);
   TabCtrl_SetPadding(client.tabcontrol_chatview_handle,SCALEX(3),SCALEY(1));
}

void destroy_chat_screen(){
   DestroyWindow(client.sippref_handle);
   DestroyWindow(client.edit_chatinput_handle);
   DestroyWindow(client.button_chatsend_handle);
   while(tab_delete_current(client.tabcontrol_chatview_handle)!=-1);
   DestroyWindow(client.tabcontrol_chatview_handle);
   DestroyWindow(client.button_closetab_handle);
}

void init_profile_screen(HWND hWnd){
   manager.static_label1_handle = CreateWindowEx(0,L"STATIC",L"Profiles",WS_CHILD|WS_VISIBLE,resize.STATIC_LEFT,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
   manager.static_label2_handle = CreateWindowEx(0,L"STATIC",L"Selected",WS_CHILD|WS_VISIBLE,resize.STATIC_LEFT_SECOND,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
}

void destroy_profile_screen(){
   DestroyWindow(manager.static_label1_handle);
   DestroyWindow(manager.static_label2_handle);
}

int open_input_box(HWND parent_window, wchar_t *title, wchar_t *text, wchar_t *result, unsigned int result_len){
   //result will be null string if DialogBoxParam fails or user provides no text
   wchar_t *titletext[2]={title,text};
   wchar_t *temp = (wchar_t*)DialogBoxParam(config.h_instance, (LPCTSTR)IDD_INPUTBOX, parent_window, InputBoxProc, (LPARAM)titletext);
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
