/*
* manager.h
*
* This file contains functions to use by manager.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void init_profile_screen(HWND hWnd){
   manager.static_label1_handle = CreateWindowEx(0,L"STATIC",L"Profiles",WS_CHILD|WS_VISIBLE,resize.STATIC_LEFT,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
   manager.static_label2_handle = CreateWindowEx(0,L"STATIC",L"Selected",WS_CHILD|WS_VISIBLE,resize.STATIC_LEFT_SECOND,resize.STATIC_TOP,resize.STATIC_WIDTH,resize.STATIC_HEIGHT,hWnd,(HMENU)NULL,config.h_instance,NULL);
}

void destroy_profile_screen(){
   DestroyWindow(manager.static_label1_handle);
   DestroyWindow(manager.static_label2_handle);
}

int guimanager_init(HWND hWnd){
	memset(&manager,0,sizeof(manager));
	wchar_t wfilespath[IRC_SIZE_SMALL];
	wfile_to_fullpath(L"*.ini",wfilespath);
	WIN32_FIND_DATA find;
	memset(&find,0,sizeof(find));
	HANDLE findhwnd = FindFirstFile(wfilespath,&find);
	if(findhwnd!=INVALID_HANDLE_VALUE){
		checkbox_create(find.cFileName,hWnd);
		while(FindNextFile(findhwnd,&find)){
			checkbox_create(find.cFileName,hWnd);
		}
		FindClose(findhwnd);
	}
	init_menu_bar(hWnd,IDR_MAIN_MENU_MANAGER);
	init_profile_screen(hWnd);
	return 0;
}

void guimanager_destroy(){
   int i;
   for(i=0;i<manager.connect_size;i++){
      DestroyWindow(manager.connect_handles[i]);
   }
   destroy_profile_screen();
   memset(&manager,0,sizeof(struct guimanager_t));
}
