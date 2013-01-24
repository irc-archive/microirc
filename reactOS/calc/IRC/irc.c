/*
* irc.cpp
*
* This file contains an irc client gui implementation with ircprotocol lib.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include "irc.h"

config_t config;
resize_t resize;
guiclient_t client;
guimanager_t manager;

#include "functions.h"
#include "ircconfig.h"
#include "checkbox_manager.h"
#include "tab_manager.h"
#include "proc_functions.h"
#include "client.h"
#include "manager.h"

//wchar_t aa[100];
//swprintf(aa, L"%d\n", aaa);
//swprintf(aa, L"OI\n", uMsg, wParam, lParam, mitab->text, a);
//write_text_current(client.tabcontrol_chatview_handle, aa, NULL, TSTRUE);
//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
//ChooseColor
int WINAPI WinMain2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    /* Load extra richedit library */
    if(LoadLibrary(L"Msftedit.dll") < 0){
        return 0;
    }
    
#ifdef CLIENT_ONLY
    lpCmdLine = IRC_CONST_DEFAULT;
#endif
    
    /* Load global properties and set defaults */
    config.h_instance = hInstance;
    LoadString(hInstance, IDS_WNDCLASS_IRC, config.window_class, IRC_SIZE_SMALL);
    LoadString(hInstance, IDS_APP_TITLE, config.window_title, IRC_SIZE_SMALL);
    config.background_color = 0x00FFFFFF;
    config.text_color = 0x00000000;
    if(GetModuleFileName(NULL,config.module_path,IRC_SIZE_SMALL)<=0){
        return 0;
    }
    
    if(update_title(config.window_title,lpCmdLine)!=0){
        return 0;
    }
    
    if(FindWindow(config.window_class, config.window_title)!=NULL){
        return 0;
    }
    
    INITCOMMONCONTROLSEX icex;
    memset(&icex, 0, sizeof(icex));
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_BAR_CLASSES|ICC_TAB_CLASSES;
    if (!InitCommonControlsEx(&icex)){
        return 0;
    }
    
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW|CS_VREDRAW;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRC));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = config.window_class;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRC));
    if(wcslen(lpCmdLine)==0){
        wcex.lpfnWndProc = WindowProcManager;
    }else{
        wcex.lpfnWndProc = WindowProcClient;
    }
    
    if(RegisterClassEx(&wcex)==0){
        return 0;
    }
    
    HWND hWndMain = CreateWindowEx(0, config.window_class, config.window_title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,NULL, hInstance, lpCmdLine);
    if(hWndMain==NULL){
        return 0;
    }
    
    ShowWindow(hWndMain, SW_SHOWMAXIMIZED);
    UpdateWindow(hWndMain);
    MSG msg;
	memset(&msg, 0, sizeof(msg));
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRC));
    while(GetMessage(&msg, NULL, 0, 0)){
        if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}
