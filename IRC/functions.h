/*
* functions.h
*
* This file contains some generic functions.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void set_led(int led_num, int state){
   NLED_SETTINGS_INFO settings;
   settings.LedNum=led_num;
   settings.OffOnBlink=state;
   NLedSetDevice(NLED_SETTINGS_INFO_ID,&settings);
}

VOID CALLBACK deactivate_led(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
   timer_led=NULL;
   set_led(config.lednumber,0);
}

void activate_led(){
   if(timer_led!=NULL){
      return;
   }
   timer_led=timeSetEvent(config.ledinterval, 50, deactivate_led, 0, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
   if(timer_led!=NULL){
      set_led(config.lednumber,1);
   }
}

int title(wchar_t *window_title, wchar_t *cmd_line){
   if(wcslen(cmd_line)==0){
      wcscat(window_title,L" Profile Manager");
   }else{
      wchar_t *end = wcsstr(cmd_line,L".ini");
      if(end==NULL){
         return -1;
      }
      wchar_t *start = wcsrchr(cmd_line,'\\');
      if(start==NULL){
         start=cmd_line;
      }else{
         start++;
      }
      wcscat(window_title,L" ");
      wcsncat(window_title,start,end-start);
   }
   return 0;
}

int validate_name(wchar_t *str){
   while(*str!='\0'){
      if(*str<48 || *str>122){
         return -1;
      }
      if(*str>57 && *str<65){
         return -1;
      }
      if(*str>90 && *str<96){
         return -1;
      }
      str++;
   }
   return 0;
}

void wfile_to_fullpath(wchar_t *wfilename, wchar_t *wfullpath){
   wcsncpy(wfullpath,module_path,IRC_SIZE_SMALL);
   wcscpy(wcsrchr(wfullpath,'\\')+1,wfilename);
}

int winiparser_load(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wfile_to_fullpath(wfilename,wfullpath);
   WideCharToMultiByte(IRC_FILE_PATH_ENCODING,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_load(iniparser,fullpath);
}

int winiparser_store(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wfile_to_fullpath(wfilename,wfullpath);
   WideCharToMultiByte(IRC_FILE_PATH_ENCODING,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_store(iniparser,fullpath);
}

void refresh_client_sizes(int width, int height, int logicalx, int logicaly){
   BORDER = SCALEX(1,logicalx);
   CLOSETAB_WIDTH = SCALEX(20,logicalx);
   CLOSETAB_HEIGHT = SCALEY(20,logicaly);
   BUTTONCHAT_WIDTH = SCALEY(40,logicaly);
   BUTTONCHAT_HEIGHT = SCALEY(20,logicaly);
   TABCONTROLCHAT_WIDTH = width-(BORDER+BORDER+BORDER+CLOSETAB_WIDTH);
   TABCONTROLCHAT_HEIGHT = SCALEY(20,logicaly);
   EDITCHAT_WIDTH = width-(BORDER+BORDER+BORDER+BUTTONCHAT_WIDTH);
   EDITCHAT_HEIGHT = SCALEY(20,logicaly);
   TABALL_HEIGHT = height-(BORDER+BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+EDITCHAT_HEIGHT);
   TABTALK_STATUS_WIDTH = width-(BORDER+BORDER);
   TABNICK_CHAT_WIDTH = SCALEX(70,logicalx);
   TABTALK_CHAT_WIDTH = width-(BORDER+BORDER+BORDER+TABNICK_CHAT_WIDTH);

   CLOSETAB_TOP = BORDER;
   CLOSETAB_LEFT = BORDER+BORDER+TABCONTROLCHAT_WIDTH;
   BUTTONCHAT_TOP = BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+TABALL_HEIGHT;
   BUTTONCHAT_LEFT = BORDER+BORDER+EDITCHAT_WIDTH;
   TABCONTROLCHAT_TOP = BORDER;
   TABCONTROLCHAT_LEFT = BORDER;
   EDITCHAT_TOP = BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+TABALL_HEIGHT;
   EDITCHAT_LEFT = BORDER;
   TABTALK_TOP = BORDER+BORDER+TABCONTROLCHAT_HEIGHT;
   TABTALK_LEFT = BORDER;
   TABNICK_TOP = BORDER+BORDER+TABCONTROLCHAT_HEIGHT;
   TABNICK_LEFT = BORDER+BORDER+TABTALK_CHAT_WIDTH;
}

void refresh_manager_sizes(int logicalx, int logicaly){
   MANAGER_RADIO_LEFT = SCALEX(5,logicalx);
   MANAGER_RADIO_TOP = SCALEY(20,logicaly);
   MANAGER_RADIO_WIDTH = SCALEX(180,logicalx);
   MANAGER_RADIO_HEIGHT = SCALEY(20,logicaly);
   MANAGER_RADIO_TOP_DISTANCE = SCALEY(25,logicaly);
}
