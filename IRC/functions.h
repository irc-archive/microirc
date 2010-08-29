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

void wappend_fullpath(wchar_t *wconfigfile, wchar_t *result){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   wcscpy(wfullpath,module_path);
   wcscpy(wcsrchr(wfullpath,'\\')+1,wconfigfile);
   wcscpy(result,wfullpath);
}

void append_fullpath(char *configfile, char *result){
   char fullpath[IRC_SIZE_SMALL];
   WideCharToMultiByte(CP_ACP,0,module_path,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   strcpy(strrchr(fullpath,'\\')+1,configfile);
   strcpy(result,fullpath);
}

int winiparser_load(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wcscpy(wfullpath,module_path);
   wcscpy(wcsrchr(wfullpath,'\\')+1,wfilename);
   WideCharToMultiByte(CP_ACP,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_load(iniparser,fullpath);
}

int winiparser_store(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wcscpy(wfullpath,module_path);
   wcscpy(wcsrchr(wfullpath,'\\')+1,wfilename);
   WideCharToMultiByte(CP_ACP,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_store(iniparser,fullpath);
}
