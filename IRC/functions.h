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

char *strstri(char *t, char *s){//lstrcmpi
   int i, j;
   for(i=0; t[i] != '\0'; i++){
      for(j=0; s[j] != '\0'; j++){
         if(toupper(s[j])==toupper(t[i+j])){
            continue; 
         }else{
            break;
         }
      }
      if (s[j] == '\0'){
         break;
      }
   }
   if (s[j] == '\0'){
      return (i+t);
   }else{
      return '\0';
   }
}

void set_led(int led_num, int state){
    NLED_SETTINGS_INFO settings;
    settings.LedNum=led_num;
    settings.OffOnBlink=state;
    NLedSetDevice(NLED_SETTINGS_INFO_ID,&settings);
}

VOID CALLBACK deactivate_led(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
    timeKillEvent(uTimerID);
    LEDtimer=NULL;
    set_led(config.lednumber,0);
}

void activate_led(){
    LEDtimer = timeSetEvent(config.ledinterval, 50, deactivate_led, 0, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
    if(LEDtimer!=NULL){
       set_led(config.lednumber,1);
    }
}

int irc_and_ircconfig_init(irc_t *irc, ircconfig_t *config, char *filepath){
   iniparser_t iniparser;
   if(iniparser_init(&iniparser)!=0){
      return -1;
   }
   if(iniparser_load(&iniparser,filepath)!=0){
      iniparser_destroy(&iniparser);
      return -1;
   }
   char host[IRC_BUFFER_SIZE_LITTLE];
   char port[IRC_BUFFER_SIZE_LITTLE];
   char user[IRC_BUFFER_SIZE_LITTLE];
   char name[IRC_BUFFER_SIZE_LITTLE];
   char nick[IRC_BUFFER_SIZE_LITTLE];
   char perform[IRC_BUFFER_SIZE_MEDIUM];
   char autojoin_channels[IRC_BUFFER_SIZE_MEDIUM];
   int autojoin_delay;
   int reconnect;
   int encoding;
   int sounds;
   int lednumber;
   int ledinterval;

   strncpy(host,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"),IRC_BUFFER_SIZE_LITTLE);
   strncpy(port,iniparser_getstring(&iniparser, "server", "port", "6667"),IRC_BUFFER_SIZE_LITTLE);
   strncpy(user,iniparser_getstring(&iniparser, "client", "user", "user"),IRC_BUFFER_SIZE_LITTLE);
   strncpy(name,iniparser_getstring(&iniparser, "client", "name", "Real Name"),IRC_BUFFER_SIZE_LITTLE);
   strncpy(nick,iniparser_getstring(&iniparser, "client", "nick", "nickname"),IRC_BUFFER_SIZE_LITTLE);
   strncpy(perform,iniparser_getstring(&iniparser, "client", "perform", ""),IRC_BUFFER_SIZE_MEDIUM);
   strncpy(autojoin_channels,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"),IRC_BUFFER_SIZE_MEDIUM);
   autojoin_delay = iniparser_getint(&iniparser, "autojoin", "delay", 5000);

   reconnect = iniparser_getint(&iniparser, "autoreconnect", "retries", 5);
   if(iniparser_getint(&iniparser, "options", "encoding", 1)){
      encoding = CP_UTF8;
   }else{
      encoding = CP_ACP;
   }
   sounds = iniparser_getint(&iniparser, "options", "sounds", 0);
   lednumber = iniparser_getint(&iniparser, "options", "lednumber", -1);
   ledinterval = iniparser_getint(&iniparser, "options", "ledinterval", 500);
   
   iniparser_destroy(&iniparser);
   if(irc_init(irc,host,port,user,name,nick,perform,autojoin_channels,autojoin_delay)!=0){
      return -1;
   }
   if(ircconfig_init(config,reconnect,encoding,sounds,lednumber,ledinterval)!=0){
      irc_destroy(irc);
      return -1;
   }
   return 0;
}

void settext_fromstr(HWND hDlg, int control, char *edittext){
   wchar_t wedittext[IRC_BUFFER_SIZE_LITTLE];
   MultiByteToWideChar(CP_UTF8,0,edittext,-1,wedittext,IRC_BUFFER_SIZE_LITTLE);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void settext_fromint(HWND hDlg, int control, int edittext){
   wchar_t wedittext[IRC_BUFFER_SIZE_LITTLE];
   wsprintf(wedittext,L"%d",edittext);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void gettext_tostr(HWND hDlg, int control, char *edittext, int size){
   wchar_t wedittext[IRC_BUFFER_SIZE_LITTLE];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRC_BUFFER_SIZE_LITTLE);
   WideCharToMultiByte(CP_UTF8,0,wedittext,-1,edittext,size,NULL,NULL);
}

int gettext_toint(HWND hDlg, int control){
   wchar_t wedittext[IRC_BUFFER_SIZE_LITTLE];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRC_BUFFER_SIZE_LITTLE);
   return wcstol(wedittext,L'\0',10);
}
