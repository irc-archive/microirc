/*
* ircconfig.h
*
* This file contains some functions according to irconfig.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

int ircconfig_init(ircconfig_t *ircconfig, int connect_on_startup, int reconnect_retries, char *part, char *kick, char *quit, int encoding, int bubble, int sounds, int led_number, int led_interval){
   memset(ircconfig,0,sizeof(ircconfig_t));
   if(reconnect_retries<0 || led_interval<0){
      return -1;
   }
   ircconfig->connect_on_startup = connect_on_startup;
   ircconfig->reconnect_retries = reconnect_retries;
   strncpy0(ircconfig->part,part,IRC_SIZE_SMALL);
   strncpy0(ircconfig->kick,kick,IRC_SIZE_SMALL);
   strncpy0(ircconfig->quit,quit,IRC_SIZE_SMALL);
   ircconfig->encoding = encoding;
   ircconfig->bubble = bubble;
   ircconfig->sounds = sounds;
   ircconfig->led_number = led_number;
   ircconfig->led_interval = led_interval;
   return 0;
}

void ircconfig_destroy(ircconfig_t *ircconfig){
   memset(ircconfig,0,sizeof(ircconfig_t));
}

void irc_config_destroy(irc_t *irc, ircconfig_t *ircconfig){
   irc_destroy(irc);
   ircconfig_destroy(ircconfig);
}

int irc_config_init(irc_t *irc, ircconfig_t *ircconfig, wchar_t *filepath){
   iniparser_t iniparser;
   if(iniparser_init(&iniparser)!=0){
      return -1;
   }
   if(winiparser_load(&iniparser,filepath)!=0){
      iniparser_destroy(&iniparser);
      return -1;
   }
   char host[IRCPROTOCOL_SIZE_SMALL];
   char port[IRCPROTOCOL_SIZE_SMALL];
   char user[IRCPROTOCOL_SIZE_SMALL];
   char name[IRCPROTOCOL_SIZE_SMALL];
   char nick[IRCPROTOCOL_SIZE_SMALL];
   char perform[IRCPROTOCOL_SIZE_MEDIUM];
   char channels[IRCPROTOCOL_SIZE_MEDIUM];
   int delay;
   int connect_on_startup;
   int reconnect_retries;
   char part[IRC_SIZE_SMALL];
   char kick[IRC_SIZE_SMALL];
   char quit[IRC_SIZE_SMALL];
   int encoding;
   int bubble;
   int sounds;
   int led_number;
   int led_interval;

   strncpy0(host,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(port,iniparser_getstring(&iniparser, "server", "port", "6667"),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(user,iniparser_getstring(&iniparser, "client", "user", "user"),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(name,iniparser_getstring(&iniparser, "client", "name", "Real Name"),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(nick,iniparser_getstring(&iniparser, "client", "nick", "change_me"),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(perform,iniparser_getstring(&iniparser, "client", "perform", ""),IRCPROTOCOL_SIZE_MEDIUM);
   strncpy0(channels,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"),IRCPROTOCOL_SIZE_MEDIUM);
   delay = iniparser_getint(&iniparser, "autojoin", "delay", 5000);

   connect_on_startup = iniparser_getint(&iniparser, "connection", "connect_on_startup", 1);
   reconnect_retries = iniparser_getint(&iniparser, "connection", "reconnect_retries", 5);
   strncpy0(part,iniparser_getstring(&iniparser, "messages", "part", ""),IRC_SIZE_SMALL);
   strncpy0(kick,iniparser_getstring(&iniparser, "messages", "kick", ""),IRC_SIZE_SMALL);
   strncpy0(quit,iniparser_getstring(&iniparser, "messages", "quit", "http://code.google.com/p/microirc/"),IRC_SIZE_SMALL);
   if(iniparser_getint(&iniparser, "miscellaneous", "encoding", 1)){
      encoding = CP_UTF8;
   }else{
      encoding = CP_ACP;
   }
   bubble = iniparser_getint(&iniparser, "miscellaneous", "bubble", 0);
   sounds = iniparser_getint(&iniparser, "miscellaneous", "sounds", 0);
   led_number = iniparser_getint(&iniparser, "miscellaneous", "led_number", -1);
   led_interval = iniparser_getint(&iniparser, "miscellaneous", "led_interval", 500);
   
   iniparser_destroy(&iniparser);
   if(irc_init(irc,host,port,user,name,nick,perform,channels,delay)!=0){
      return -1;
   }
   if(ircconfig_init(ircconfig,connect_on_startup,reconnect_retries,part,kick,quit,encoding,bubble,sounds,led_number,led_interval)!=0){
      irc_destroy(irc);
      return -1;
   }
   return 0;
}

int irc_config_reload(irc_t *irc, ircconfig_t *ircconfig, wchar_t *filepath){
   return irc_config_init(irc,ircconfig,filepath);
}

void settext_fromstr(HWND hDlg, int control, char *edittext){
   wchar_t wedittext[IRCPROTOCOL_SIZE_MEDIUM];
   MultiByteToWideChar(IRC_CONFIG_FILE_ENCODING,0,edittext,-1,wedittext,IRCPROTOCOL_SIZE_MEDIUM);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void settext_fromwstr(HWND hDlg, int control, wchar_t *wedittext){
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void settext_fromint(HWND hDlg, int control, int edittext){
   wchar_t wedittext[IRCPROTOCOL_SIZE_SMALL];
   wsprintf(wedittext,L"%d",edittext);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void gettext_tostr(HWND hDlg, int control, char *edittext, int size){
   wchar_t wedittext[IRCPROTOCOL_SIZE_MEDIUM];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRCPROTOCOL_SIZE_MEDIUM);
   WideCharToMultiByte(IRC_CONFIG_FILE_ENCODING,0,wedittext,-1,edittext,size,NULL,NULL);
}

void gettext_towstr(HWND hDlg, int control, wchar_t *wedittext, int size){
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRCPROTOCOL_SIZE_MEDIUM);
}

int gettext_toint(HWND hDlg, int control){
   wchar_t wedittext[IRCPROTOCOL_SIZE_SMALL];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRCPROTOCOL_SIZE_SMALL);
   return wcstol(wedittext,L'\0',10);
}

void setcombo_fromint(HWND hDlg, int control, int combovalue){
   HWND combo = GetDlgItem(hDlg,control);
   ComboBox_AddString(combo,L"Local");
   ComboBox_AddString(combo,L"UTF-8");
   if(combovalue){
      ComboBox_SetCurSel(combo,1);
   }else{
      ComboBox_SetCurSel(combo,0);
   }
}

int getint_fromcombo(HWND hDlg, int control){
   HWND combo = GetDlgItem(hDlg,IDC_COMBO1);
   if(ComboBox_GetCurSel(combo) != 0){
      return 1;
   }else{
      return 0;
   }
}

void setcheck_fromint(HWND hDlg, int control, int checkvalue){
   HWND check = GetDlgItem(hDlg,control);
   if(checkvalue != 0){
      Button_SetCheck(check,BST_CHECKED);
   }else{
      Button_SetCheck(check,BST_UNCHECKED);
   }
}

int getint_fromcheck(HWND hDlg, int control){
   HWND check = GetDlgItem(hDlg,control);
   if(Button_GetCheck(check)==BST_CHECKED){
      return 1;
   }else{
      return 0;
   }
}
