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
#include "dialogs_functions.h"
#include "gui_functions.h"
#include "client.h"
#include "manager.h"

//MessageBox(NULL,L"LOL",NULL,MB_ICONHAND|MB_APPLMODAL|MB_SETFOREGROUND);
int WINAPI WinMain2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    /* Load extra richedit library */
    if(LoadLibrary(L"Msftedit.dll") < 0){
        return 0;
    }
    
    /* Load global properties and set defaults */
    config.h_instance = hInstance;
    LoadString(hInstance, IDS_WNDCLASS_IRC, config.window_class, IRC_SIZE_SMALL);
    LoadString(hInstance, IDS_APP_TITLE, config.window_title, IRC_SIZE_SMALL);
    config.background_color = 0x00FFFFFF;
    config.text_color = 0x00000000;
    if(GetModuleFileName(NULL,config.module_path,IRC_SIZE_SMALL)<=0){
        return 0;
    }
#ifdef CLIENT_ONLY
    if(update_title(config.window_title,L"Client.ini")!=0){
#else
    if(update_title(config.window_title,lpCmdLine)!=0){
#endif
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
    
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRC));
    wc.hCursor = 0;
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName = 0;
    wc.lpszClassName = config.window_class;
#ifdef CLIENT_ONLY
    wc.lpfnWndProc = WindowProcClient;
#else
    if(wcslen(lpCmdLine)==0){
        wc.lpfnWndProc = WindowProcManager;
    }else{
        wc.lpfnWndProc = WindowProcClient;
    }
#endif
    if(RegisterClass(&wc)==0){
        return 0;
    }

    //HWND hwndEdit= CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Type here"), ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, x, y, width, height, hwndOwner, NULL, hinst, NULL);

#ifdef CLIENT_ONLY
    HWND hWndMain = CreateWindowEx(0, config.window_class, config.window_title, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, L"options.ini");
#else
    HWND hWndMain = CreateWindowEx(0, config.window_class, config.window_title, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,(HMENU)0, hInstance, lpCmdLine);
#endif
    if(hWndMain==NULL){
        return 0;
    }
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    MSG msg;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRC));
    while(GetMessage(&msg, NULL, 0, 0)){
        if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}
