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

int ircconfig_init(ircconfig_t *ircconfig, int reconnect, int encoding, int sounds, int lednumber, int ledinterval){
   memset(ircconfig,0,sizeof(ircconfig_t));
   if(reconnect<0 || ledinterval<0){
      return -1;
   }
   ircconfig->reconnect = reconnect;
   ircconfig->encoding = encoding;
   ircconfig->sounds = sounds;
   ircconfig->lednumber = lednumber;
   ircconfig->ledinterval = ledinterval;
   return 0;
}

void ircconfig_destroy(ircconfig_t *ircconfig){
   memset(ircconfig,0,sizeof(ircconfig_t));
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
   char host[IRCPROTOCOL_SIZE_SMALL];
   char port[IRCPROTOCOL_SIZE_SMALL];
   char user[IRCPROTOCOL_SIZE_SMALL];
   char name[IRCPROTOCOL_SIZE_SMALL];
   char nick[IRCPROTOCOL_SIZE_SMALL];
   char perform[IRCPROTOCOL_SIZE_MEDIUM];
   char autojoin_channels[IRCPROTOCOL_SIZE_MEDIUM];
   int autojoin_delay;
   int reconnect;
   int encoding;
   int sounds;
   int lednumber;
   int ledinterval;

   strncpy(host,iniparser_getstring(&iniparser, "server", "host", "chat.freenode.net"),IRCPROTOCOL_SIZE_SMALL);
   strncpy(port,iniparser_getstring(&iniparser, "server", "port", "6667"),IRCPROTOCOL_SIZE_SMALL);
   strncpy(user,iniparser_getstring(&iniparser, "client", "user", "user"),IRCPROTOCOL_SIZE_SMALL);
   strncpy(name,iniparser_getstring(&iniparser, "client", "name", "Real Name"),IRCPROTOCOL_SIZE_SMALL);
   strncpy(nick,iniparser_getstring(&iniparser, "client", "nick", "change_me"),IRCPROTOCOL_SIZE_SMALL);
   strncpy(perform,iniparser_getstring(&iniparser, "client", "perform", ""),IRCPROTOCOL_SIZE_MEDIUM);
   strncpy(autojoin_channels,iniparser_getstring(&iniparser, "autojoin", "channels", "#microirc"),IRCPROTOCOL_SIZE_MEDIUM);
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
