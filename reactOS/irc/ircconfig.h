/*
* ircconfig.h
*
* This file contains some functions to manage irc configuration.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

/* Configuration */
#define IRC_CONF_SERVER "server"
#define IRC_CONF_CLIENT "client"
#define IRC_CONF_AUTOJOIN "autojoin"
#define IRC_CONF_CONNECTION "connection"
#define IRC_CONF_MESSAGES "messages"
#define IRC_CONF_MISCELLANEOUS "miscellaneous"
#define IRC_CONF_COLORS "colors"

/* Server */
#define IRC_CONF_HOST "host"
#define IRC_CONF_HOST_VAL "chat.freenode.net"
#define IRC_CONF_PORT "port"
#define IRC_CONF_PORT_VAL "6667"
#define IRC_CONF_PASS "pass"
#define IRC_CONF_PASS_VAL ""

/* Client */
#define IRC_CONF_USER "user"
#define IRC_CONF_USER_VAL "user"
#define IRC_CONF_NAME "name"
#define IRC_CONF_NAME_VAL "Real Name"
#define IRC_CONF_NICK "nick"
#define IRC_CONF_NICK_VAL "change_me"
#define IRC_CONF_PERFORM "perform"
#define IRC_CONF_PERFORM_VAL ""

/* Autojoin */
#define IRC_CONF_CHANNELS "channels"
#define IRC_CONF_CHANNELS_VAL "#microrc"
#define IRC_CONF_DELAY "delay"
#define IRC_CONF_DELAY_VAL 5000

/* Connection */
#define IRC_CONF_STARTUP "connect_on_startup"
#define IRC_CONF_STARTUP_VAL 1
#define IRC_CONF_RETRIES "reconnect_retries"
#define IRC_CONF_RETRIES_VAL 5

/* Messages */
#define IRC_CONF_PART "part"
#define IRC_CONF_PART_VAL ""
#define IRC_CONF_KICK "kick"
#define IRC_CONF_KICK_VAL ""
#define IRC_CONF_QUIT "quit"
#define IRC_CONF_QUIT_VAL "http://code.google.com/p/microirc/"

/* Miscellaneous */
#define IRC_CONF_ENCODING "encoding"
#define IRC_CONF_ENCODING_VAL 1
#define IRC_CONF_BUBBLE "bubble"
#define IRC_CONF_BUBBLE_VAL 0
#define IRC_CONF_SOUNDS "sounds"
#define IRC_CONF_SOUNDS_VAL 0
#define IRC_CONF_LEDNUMBER "led_number"
#define IRC_CONF_LEDNUMBER_VAL -1
#define IRC_CONF_LEDINTERVAL "led_interval"
#define IRC_CONF_LEDINTERVAL_VAL 500

/* Colors */
#define IRC_CONF_BACKGROUND_COLOR "background"
#define IRC_CONF_BACKGROUND_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_FONT_COLOR "font"
#define IRC_CONF_FONT_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_JOIN_COLOR "join"
#define IRC_CONF_JOIN_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_NICKCHANGE_COLOR "nickchange"
#define IRC_CONF_NICKCHANGE_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_TOPICCHANGE_COLOR "topicchange"
#define IRC_CONF_TOPICCHANGE_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_MODE_COLOR "mode"
#define IRC_CONF_MODE_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_PART_COLOR "part"
#define IRC_CONF_PART_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_KICK_COLOR "kick"
#define IRC_CONF_KICK_COLOR_VAL RGB(255,255,255)
#define IRC_CONF_QUIT_COLOR "quit"
#define IRC_CONF_QUIT_COLOR_VAL RGB(255,255,255)

int ircconfig_init(ircconfig_t *ircconfig, int connect_on_startup, int reconnect_retries, char *part, char *kick, char *quit, int encoding, int bubble, int sounds, int led_number, int led_interval, int background_color, int topicchange_color, int font_color, int join_color, int nickchange_color, int mode_color, int part_color, int kick_color, int quit_color){
   memset(ircconfig,0,sizeof(ircconfig_t));
   if(reconnect_retries<0 || bubble<0 || led_interval<0){
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
   ircconfig->background_color = background_color;
   ircconfig->font_color = font_color;
   ircconfig->join_color = join_color;
   ircconfig->nickchange_color = nickchange_color;
   ircconfig->topicchange_color = topicchange_color;
   ircconfig->mode_color = mode_color;
   ircconfig->part_color = part_color;
   ircconfig->kick_color = kick_color;
   ircconfig->quit_color = quit_color;
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
   char pass[IRCPROTOCOL_SIZE_SMALL];
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
    int background_color;
    int font_color;
    int join_color;
    int nickchange_color;
    int topicchange_color;
    int mode_color;
    int part_color;
    int kick_color;
    int quit_color;

   strncpy0(host,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_HOST, IRC_CONF_HOST_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(port,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PORT, IRC_CONF_PORT_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(pass,iniparser_getstring(&iniparser, IRC_CONF_SERVER, IRC_CONF_PASS, IRC_CONF_PASS_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(user,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_USER, IRC_CONF_USER_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(name,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NAME, IRC_CONF_NAME_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(nick,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_NICK, IRC_CONF_NICK_VAL),IRCPROTOCOL_SIZE_SMALL);
   strncpy0(perform,iniparser_getstring(&iniparser, IRC_CONF_CLIENT, IRC_CONF_PERFORM, IRC_CONF_PERFORM_VAL),IRCPROTOCOL_SIZE_MEDIUM);
   strncpy0(channels,iniparser_getstring(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_CHANNELS, IRC_CONF_CHANNELS_VAL),IRCPROTOCOL_SIZE_MEDIUM);
   delay = iniparser_getint(&iniparser, IRC_CONF_AUTOJOIN, IRC_CONF_DELAY, IRC_CONF_DELAY_VAL);

   connect_on_startup = iniparser_getint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_STARTUP, IRC_CONF_STARTUP_VAL);
   reconnect_retries = iniparser_getint(&iniparser, IRC_CONF_CONNECTION, IRC_CONF_RETRIES, IRC_CONF_RETRIES_VAL);
   strncpy0(part,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_PART, IRC_CONF_PART_VAL),IRC_SIZE_SMALL);
   strncpy0(kick,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_KICK, IRC_CONF_KICK_VAL),IRC_SIZE_SMALL);
   strncpy0(quit,iniparser_getstring(&iniparser, IRC_CONF_MESSAGES, IRC_CONF_QUIT, IRC_CONF_QUIT_VAL),IRC_SIZE_SMALL);
   if(iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_ENCODING, IRC_CONF_ENCODING_VAL)){
      encoding = CP_UTF8;
   }else{
      encoding = CP_ACP;
   }
   bubble = iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_BUBBLE, IRC_CONF_BUBBLE_VAL);
   sounds = iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_SOUNDS, IRC_CONF_SOUNDS_VAL);
   led_number = iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDNUMBER, IRC_CONF_LEDNUMBER_VAL);
   led_interval = iniparser_getint(&iniparser, IRC_CONF_MISCELLANEOUS, IRC_CONF_LEDINTERVAL, IRC_CONF_LEDINTERVAL_VAL);
    background_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_BACKGROUND_COLOR, IRC_CONF_BACKGROUND_COLOR_VAL);
    font_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_FONT_COLOR, IRC_CONF_FONT_COLOR_VAL);
    join_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_JOIN_COLOR, IRC_CONF_JOIN_COLOR_VAL);
    nickchange_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_NICKCHANGE_COLOR, IRC_CONF_NICKCHANGE_COLOR_VAL);
    topicchange_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_TOPICCHANGE_COLOR, IRC_CONF_TOPICCHANGE_COLOR_VAL);
    mode_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_MODE_COLOR, IRC_CONF_MODE_COLOR_VAL);
    part_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_PART_COLOR, IRC_CONF_PART_COLOR_VAL);
    kick_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_KICK_COLOR, IRC_CONF_KICK_COLOR_VAL);
    quit_color = iniparser_getint(&iniparser, IRC_CONF_COLORS, IRC_CONF_QUIT_COLOR, IRC_CONF_QUIT_COLOR_VAL);

   iniparser_destroy(&iniparser);
   if(irc_init(irc,host,port,pass,user,name,nick,perform,channels,delay)!=0){
      return -1;
   }
   if(ircconfig_init(ircconfig,connect_on_startup,reconnect_retries,part,kick,quit,encoding,bubble,sounds,led_number,led_interval,background_color,font_color,join_color,nickchange_color,topicchange_color,mode_color,part_color,kick_color,quit_color)!=0){
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
   return wcstol(wedittext,'\0',10);
}

void setcombo_fromint(HWND hDlg, int control, int combovalue){
   HWND combo = GetDlgItem(hDlg,control);
   ComboBox_AddString(combo,IRC_CONST_ANSI);
   ComboBox_AddString(combo,IRC_CONST_UNICODE);
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

void setcolor_fromrich(HWND hDlg, int control, COLORREF color){
    HWND rich = GetDlgItem(hDlg,control);
    SendMessage(rich,EM_SETBKGNDCOLOR,0,color);
}

COLORREF getcolor_fromrich(HWND hDlg, int control){
    HWND rich = GetDlgItem(hDlg,control);
    return SendMessage(rich,EM_SETBKGNDCOLOR,1,0);
}
