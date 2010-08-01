/*
* ircprotocol.c
*
* This file contains an ircprotocol lib implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2009-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "buffer.h"
#include "network.h"
#include "list.h"
#include "ircprotocol.h"

char *tokens_required(char *str, char character, int n_tokens, char **d_tokens, int *s_tokens){
   int in = 0;
   while(*str!=CHAR_TERMINATOR){
      while(*str==character){
         str++;
      }
      while(*str!=character && *str!=CHAR_TERMINATOR){
         if(in==0){
            d_tokens[*s_tokens] = str;
            (*s_tokens)++;
            in = 1;
         }
         str++;
      }
      if(in){
         if(*str!=CHAR_TERMINATOR){
            *str=CHAR_TERMINATOR;
            str++;
         }else{
            return NULL;
         }
         n_tokens--;
         if(n_tokens<=0){
            return str;
         }
         in = 0;
      }
   }
   return NULL;
}

char *strnullatchar(char *str, char character){
   char *result = strchr(str,character);
   if(result==NULL){
      return NULL;
   }
   *result=CHAR_TERMINATOR;
   return result+1;
}

char *strignorechar(char *str, char character){
   if(str[0]==character){
      return str+1;
   }
   return str;
}

void get_nick_user_host(char *str, char **d_tokens, int *s_tokens){
   d_tokens[*s_tokens] = strnullatchar(str,CHAR_COLON);
   if(d_tokens[*s_tokens]==NULL){
      return;
   }
   (*s_tokens)++;
   d_tokens[*s_tokens] = strnullatchar(d_tokens[(*s_tokens)-1],CHAR_EXCLAMATAIONMARK);
   if(d_tokens[*s_tokens]==NULL){
      return;
   }
   (*s_tokens)++;
   if(d_tokens[(*s_tokens)-1]!=NULL){
      d_tokens[*s_tokens] = strnullatchar(d_tokens[(*s_tokens)-1],CHAR_ATSIGN);
      if(d_tokens[*s_tokens]==NULL){
         return;
      }
      (*s_tokens)++;
   }else{
      (*s_tokens)--;
   }
}

int verify_buffer(char **messages, int size){
   for(size--;size>=0;size--){
      if(messages[size]==NULL){
         return -1;
      }
   }
   return 0;
}

__declspec(dllexport) int irc_init(irc_t *irc, char *host, char *port, char *user, char *name, char *nick, char *perform, char *channels, int delay){
   memset(irc,0,sizeof(irc_t));
   if(strlen(host)==0 || strlen(port)==0 || strlen(user)==0 || strlen(name)==0 || strlen(nick)==0 || delay<0){
      return -1;
   }
   strncpy(irc->host,host,IRC_BUFFER_SIZE_LITTLE);
   strncpy(irc->port,port,IRC_BUFFER_SIZE_LITTLE);
   strncpy(irc->user,user,IRC_BUFFER_SIZE_LITTLE);
   strncpy(irc->name,name,IRC_BUFFER_SIZE_LITTLE);
   strncpy(irc->nick,nick,IRC_BUFFER_SIZE_LITTLE);
   strncpy(irc->perform,perform,IRC_BUFFER_SIZE_MEDIUM);
   strncpy(irc->autojoin_channels,channels,IRC_BUFFER_SIZE_MEDIUM);
   irc->autojoin_delay = delay;
   return 0;
}

__declspec(dllexport) void irc_destroy(irc_t *irc){
   memset(irc,0,sizeof(irc_t));
}

VOID CALLBACK timer_procedure(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
   irc_t *irc = (irc_t*)dwUser;
   irc->connected = 0;
   close_tcp(&irc->network);
}

__declspec(dllexport) int irc_connect(irc_t *irc){
   if(buffer_init(&irc->recv_buffer_stream,IRC_SPLITER_DATA,IRC_SPLITER_SIZE,IRC_BUFFER_SIZE_LARGE)!=0){
      return -1;
   }
   if(connect_tcp(&irc->network,irc->host,irc->port)!=0){
      buffer_destroy(&irc->recv_buffer_stream);
      return -1;
   }
   irc->connected=1;
   InitializeCriticalSection(&irc->send_buffer_critical_section);
   char *sendrecv[IRC_RECV_MAX_TOKENS];
   sendrecv[0] = irc->nick;
   if(irc_send_message(irc,SEND_NICK,sendrecv,1)<0){
      goto fullerror;
   }
   sendrecv[0] = irc->user;
   sendrecv[1] = irc->name;
   if(irc_send_message(irc,SEND_USER,sendrecv,2)<0){
      goto fullerror;
   }
   MMRESULT timer;
   timer = timeSetEvent(IRC_REGISTER_TIMEOUT,1000,timer_procedure,(DWORD)irc,TIME_ONESHOT);
   if(timer==0){
      goto fullerror;
   }
   int size;
   int result = 1;
   while(result>0 && result!=RECV_WELCOME){
      result = irc_recv_message(irc,sendrecv,&size);
      switch(result){
         case RECV_WELCOME:{
            timeKillEvent(timer);
            char *d_tokens[IRC_CONFIG_MAX_TOKENS];
            int s_tokens=0;
            int i;
            tokens_required(irc->perform,CHAR_COMMA,IRC_CONFIG_MAX_TOKENS,d_tokens,&s_tokens);
            for(i=0;i<s_tokens;i++){
               if(irc_send_message(irc,SEND_RAW,&d_tokens[i],1)<0){
                  goto fullerror;
               }
            }
            s_tokens=0;
            Sleep(irc->autojoin_delay);
            tokens_required(irc->autojoin_channels,CHAR_COMMA,IRC_CONFIG_MAX_TOKENS,d_tokens,&s_tokens);
            for(i=0;i<s_tokens;i++){
               if(irc_send_message(irc,SEND_JOIN,&d_tokens[i],1)<0){
                  goto fullerror;
               }
            }
            break;
         }
         case RECV_NICK_TAKEN:{
            if(irc_send_message(irc,SEND_NICK,NULL,0)<0){
               goto fullerror1;
            }
            break;
         }
      }
   }
   if(result<=0){
      goto fullerror;
   }
   return 0;
   fullerror1:
      timeKillEvent(timer);
   fullerror:
      close_tcp(&irc->network);
      buffer_destroy(&irc->recv_buffer_stream);
      DeleteCriticalSection(&irc->send_buffer_critical_section);
      return -1;
}

__declspec(dllexport) void irc_disconnect(irc_t *irc, char *message){
   irc->connected=0;
   char *msgs[1];
   msgs[0] = message;
   if(message==NULL){
      irc_send_message(irc,SEND_QUIT,NULL,0);
   }else{
      irc_send_message(irc,SEND_QUIT,msgs,1);
   }
   close_tcp(&irc->network);
   buffer_destroy(&irc->recv_buffer_stream);
   DeleteCriticalSection(&irc->send_buffer_critical_section);
}

__declspec(dllexport) int irc_recv_message(irc_t *irc, char **d_result, int *s_result){
   while(irc->connected){
      if(gettext_tcp(&irc->network,&irc->recv_buffer_stream)<=0){
         return -1;
      }
      buffer_read_get_data(&irc->recv_buffer_stream,irc->recv_buffer,IRC_BUFFER_SIZE_MEDIUM);
      irc->recv_buffer[buffer_line_len(&irc->recv_buffer_stream,irc->recv_buffer,IRC_BUFFER_SIZE_MEDIUM)]=CHAR_TERMINATOR;
      if(!memcmp(irc->recv_buffer,"PING",4)){
         char *pong[1] = {strnullatchar(irc->recv_buffer,CHAR_SPACE)};
         if(irc_send_message(irc,SEND_PONG,pong,1)<0){
            return -1;
         }
      }else if(!memcmp(irc->recv_buffer,":",1)){
         *s_result=0;
         char *dresult[2];
         int sresult=0;
         char *next = tokens_required(irc->recv_buffer,CHAR_SPACE,2,dresult,&sresult);
         get_nick_user_host(dresult[0],d_result,s_result);
         if(!memcmp(dresult[1],"JOIN",4) && next!=NULL){//nick user host channel
            next = strignorechar(next,CHAR_COLON);
            d_result[*s_result] = next;
            (*s_result)++;
            return RECV_JOIN;
         }else if(!memcmp(dresult[1],"KICK",4) && next!=NULL){//nick user host channel victim message
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_COLON);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_KICK;
         }else if(!memcmp(dresult[1],"NICK",4) && next!=NULL){//nick user host newnick
            next = strignorechar(next,CHAR_COLON);
            d_result[*s_result] = next;
            (*s_result)++;
            return RECV_NICK;
         }else if(!memcmp(dresult[1],"NOTICE",6) && next!=NULL){//nick user host destination message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_COLON);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_NOTICE;
         }else if(!memcmp(dresult[1],"PART",4) && next!=NULL){//nick user host channel message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_COLON);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_PART;
         }else if(!memcmp(dresult[1],"PRIVMSG",7) && next!=NULL){//nick user host destination message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_COLON);
            if(message[0]==CHAR_CTCP){
               message = strignorechar(message,CHAR_CTCP);
               message[strlen(message)-1] = CHAR_TERMINATOR;
               d_result[*s_result] = message;
               (*s_result)++;
               return RECV_CTCP;
            }else{
               d_result[*s_result] = message;
               (*s_result)++;
               return RECV_PRIVMSG;
            }
         }else if(!memcmp(dresult[1],"QUIT",4)){//nick user host message
            if(next!=NULL){
               next = strignorechar(next,CHAR_COLON);
               d_result[*s_result] = next;
               (*s_result)++;
            }
            return RECV_QUIT;
         }else if(!memcmp(dresult[1],"TOPIC",5) && next!=NULL){//nick user host channel topic
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_COLON);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_TOPIC_CHANGED;
         }else if(!memcmp(dresult[1],"001",3) && next!=NULL){//host nick message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_COLON);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_WELCOME;
         }else if(!memcmp(dresult[1],"332",3) && next!=NULL){//host nick channel topic
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            message = strignorechar(message,CHAR_COLON);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_TOPIC;
         }else if(!memcmp(dresult[1],"353",3) && next!=NULL){//host channel nicklist
            char *nicks = tokens_required(next,CHAR_SPACE,3,d_result,s_result);
            d_result[(*s_result)-3] = d_result[(*s_result)-1];
            (*s_result)-=2;
            nicks = strignorechar(nicks,CHAR_COLON);
            d_result[*s_result] = nicks;
            (*s_result)++;
            return RECV_NICK_LIST;
         }else if(!memcmp(dresult[1],"433",3) && next!=NULL){//host actualnick failednick message
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_COLON);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_NICK_TAKEN;
         }else if(next!=NULL){//host message
            next = strignorechar(next,CHAR_COLON);
            d_result[*s_result] = next;
            (*s_result)++;
            return RECV_OTHER;
         }
      }
   }
   return -1;
}

__declspec(dllexport) int irc_send_message(irc_t *irc, int opcode, char **messages, int size){
   switch(opcode){
      case SEND_CTCP:{
         if(size!=2 || messages[0]==NULL || messages[1]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"PRIVMSG %s :%s",messages[0],messages[1]);
         break;
      }
      case SEND_GET_TOPIC:{//channel
         if(size!=1 || messages[0]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"TOPIC %s",messages[0]);
         break;
      }
      case SEND_JOIN:{//channel
         if(size!=1 || messages[0]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"JOIN :%s",messages[0]);
         break;
      }
      case SEND_KICK:{//channel user message
         if(size!=3 || messages[0]==NULL || messages[1]==NULL || messages[2]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"KICK %s %s :%s",messages[0],messages[1],messages[2]);
         break;
      }
      case SEND_NICK:{//nick or null
         if((size<0 || size>1) && verify_buffer(messages,size)!=0){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         if(size==0){
            srand(GetTickCount());
            sprintf(irc->send_buffer, "NICK nick_em_uso%d", rand()%1000);
         }else{
            sprintf(irc->send_buffer, "NICK %s", messages[0]);
         }
         break;
      }
      case SEND_PART:{//channel
         if((size<1 || size>2) && verify_buffer(messages,size)!=0){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         if(size==1){
            sprintf(irc->send_buffer,"PART %s",messages[0]);
         }else{
            sprintf(irc->send_buffer,"PART %s :s",messages[0],messages[1]);
         }
         break;
      }
      case SEND_PONG:{//ping
         if(size!=1 || messages[0]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"PONG %s",messages[0]);
         break;
      }
      case SEND_PRIVMSG:{//destination message
         if(size!=2 || messages[0]==NULL || messages[1]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"PRIVMSG %s :%s",messages[0],messages[1]);
         break;
      }
      case SEND_QUIT:{//message
         if((size<0 || size>1) && verify_buffer(messages,size)!=0){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         if(size==0){
            sprintf(irc->send_buffer,"QUIT :+q");
         }else{
            sprintf(irc->send_buffer,"QUIT :%s",messages[0]);
         }
         break;
      }
      case SEND_RAW:{
         if(size!=1 || messages[0]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"%s",messages[0]);
         break;
      }
      case SEND_SET_TOPIC:{//channel topic
         if(size!=2 || messages[0]==NULL || messages[1]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"TOPIC %s :%s",messages[0],messages[1]);
         break;
      }
      case SEND_USER:{//user and user
         if(size!=2 || messages[0]==NULL || messages[1]==NULL){
            return -1;
         }
         EnterCriticalSection(&irc->send_buffer_critical_section);
         sprintf(irc->send_buffer,"USER %s 8 * :%s",messages[0],messages[1]);
         break;
      }
      default:{
         return -1;
      }
   }
   int send_result = sendtext_tcp(&irc->network,&irc->recv_buffer_stream,irc->send_buffer,strlen(irc->send_buffer)+irc->recv_buffer_stream.s_split);
   LeaveCriticalSection(&irc->send_buffer_critical_section);
   return send_result;
}
