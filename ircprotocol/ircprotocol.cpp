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
   d_tokens[*s_tokens] = strnullatchar(str,CHAR_TRAIL);
   if(d_tokens[*s_tokens]==NULL){
      return;
   }
   (*s_tokens)++;
   d_tokens[*s_tokens] = strnullatchar(d_tokens[(*s_tokens)-1],CHAR_MASKSEPARATOR1);
   if(d_tokens[*s_tokens]==NULL){
      return;
   }
   (*s_tokens)++;
   if(d_tokens[(*s_tokens)-1]!=NULL){
      d_tokens[*s_tokens] = strnullatchar(d_tokens[(*s_tokens)-1],CHAR_MASKSEPARATOR2);
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
   strncpy(irc->prefix_mode,"ov",IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->prefix_char,"@+",IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->chantypes,"#",IRCPROTOCOL_SIZE_SMALL);
   irc->channellen = 200;
   irc->nicklen = 16;
   irc->maxnicklen = 16;
   strncpy(irc->host,host,IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->port,port,IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->user,user,IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->name,name,IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->nick,nick,IRCPROTOCOL_SIZE_SMALL);
   strncpy(irc->perform,perform,IRCPROTOCOL_SIZE_MEDIUM);
   strncpy(irc->autojoin_channels,channels,IRCPROTOCOL_SIZE_MEDIUM);
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
   if(buffer_init(&irc->recv_buffer_stream,IRCPROTOCOL_SPLITER_DATA,IRCPROTOCOL_SPLITER_SIZE,IRCPROTOCOL_SIZE_LARGE)!=0){
      return -1;
   }
   if(connect_tcp(&irc->network,irc->host,irc->port)!=0){
      buffer_destroy(&irc->recv_buffer_stream);
      return -1;
   }
   irc->connected=1;
   InitializeCriticalSection(&irc->send_buffer_critical_section);
   char *sendrecv[IRCPROTOCOL_RECV_MAX_TOKENS];
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
   timer = timeSetEvent(IRCPROTOCOL_REGISTER_TIMEOUT,1000,timer_procedure,(DWORD)irc,TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
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
            strncpy(irc->nick,sendrecv[1],IRCPROTOCOL_SIZE_SMALL);
            char *d_tokens[IRCPROTOCOL_CONFIG_MAX_TOKENS];
            int s_tokens=0;
            int i;
            tokens_required(irc->perform,CHAR_LISTSEPARATOR,IRCPROTOCOL_CONFIG_MAX_TOKENS,d_tokens,&s_tokens);
            for(i=0;i<s_tokens;i++){
               if(irc_send_message(irc,SEND_RAW,&d_tokens[i],1)<0){
                  goto fullerror;
               }
            }
            s_tokens=0;
            Sleep(irc->autojoin_delay);
            tokens_required(irc->autojoin_channels,CHAR_LISTSEPARATOR,IRCPROTOCOL_CONFIG_MAX_TOKENS,d_tokens,&s_tokens);
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
   char *sendrecv[1];
   sendrecv[0] = message;
   if(message==NULL){
      irc_send_message(irc,SEND_QUIT,NULL,0);
   }else{
      irc_send_message(irc,SEND_QUIT,sendrecv,1);
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
      buffer_read_get_data(&irc->recv_buffer_stream,irc->recv_buffer,IRCPROTOCOL_SIZE_MEDIUM);
      int recvsize = buffer_line_len(&irc->recv_buffer_stream,irc->recv_buffer,IRCPROTOCOL_SIZE_MEDIUM);
      irc->recv_buffer[recvsize]=CHAR_TERMINATOR;
      if(strlen(irc->recv_buffer)!=recvsize){
         return -1;
      }
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
         if(sresult<2){
            return -1;
         }
         get_nick_user_host(dresult[0],d_result,s_result);
         if(!memcmp(dresult[1],"JOIN",4) && next!=NULL){//nick user host channel
            next = strignorechar(next,CHAR_TRAIL);
            d_result[*s_result] = next;
            (*s_result)++;
            return RECV_JOIN;
         }else if(!memcmp(dresult[1],"KICK",4) && next!=NULL){//nick user host channel victim message
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_TRAIL);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_KICK;
         }else if(!memcmp(dresult[1],"MODE",4) && next!=NULL){//nick user host channel modes nicklist
            //char *nicklist = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            //d_result[*s_result] = nicklist;
            //(*s_result)++;
            //return RECV_NICK_LIST;
            return RECV_OTHER;
         }else if(!memcmp(dresult[1],"NICK",4) && next!=NULL){//nick user host newnick
            next = strignorechar(next,CHAR_TRAIL);
            d_result[*s_result] = next;
            (*s_result)++;
            if(strcmp(d_result[0],irc->nick)==0){
               strncpy(irc->nick,d_result[3],IRCPROTOCOL_SIZE_SMALL);
            }
            return RECV_NICK;
         }else if(!memcmp(dresult[1],"NOTICE",6) && next!=NULL){//nick user host destination message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_TRAIL);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_NOTICE;
         }else if(!memcmp(dresult[1],"PART",4) && next!=NULL){//nick user host channel message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_TRAIL);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_PART;
         }else if(!memcmp(dresult[1],"PRIVMSG",7) && next!=NULL){//nick user host destination message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_TRAIL);
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
               next = strignorechar(next,CHAR_TRAIL);
               d_result[*s_result] = next;
               (*s_result)++;
            }
            return RECV_QUIT;
         }else if(!memcmp(dresult[1],"TOPIC",5) && next!=NULL){//nick user host channel topic
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            message = strignorechar(message,CHAR_TRAIL);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_TOPIC_CHANGED;
         }else if(!memcmp(dresult[1],"001",3) && next!=NULL){//host nick message
            char *message = tokens_required(next,CHAR_SPACE,1,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_TRAIL);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_WELCOME;
         }else if(!memcmp(dresult[1],"005",3) && next!=NULL){//host nick options
            char *d_tokens[IRCPROTOCOL_RECV_MAX_TOKENS];
            int s_tokens=0;
            int i;
            char *separator;
            tokens_required(next,CHAR_SPACE,IRCPROTOCOL_RECV_MAX_TOKENS,d_tokens,&s_tokens);
            for(i=1;i<s_tokens;i++){
               separator = strchr(d_tokens[i],CHAR_ISUPPORTSEPARATOR);
               if(separator!=NULL){
                  *separator='\0';
                  *separator++;
                  if(memcmp("CHANTYPES",d_tokens[i],9)==0){
                     strncpy(irc->chantypes,separator,IRCPROTOCOL_SIZE_SMALL);
                  }else if(memcmp("PREFIX",d_tokens[i],6)==0){
                     *separator++;
                     char *newseparator = strchr(separator,CHAR_PREFIXDELIMITER);
                     if(newseparator!=NULL){
                        *newseparator='\0';
                        *newseparator++;
                        if(strlen(separator)==strlen(newseparator)){
                           strncpy(irc->prefix_mode,separator,IRCPROTOCOL_SIZE_SMALL);
                           strncpy(irc->prefix_char,newseparator,IRCPROTOCOL_SIZE_SMALL);
                        }
                     }
                  }else if(memcmp("CHANNELLEN",d_tokens[i],10)==0){
                     irc->channellen = atoi(separator);
                  }else if(memcmp("NICKLEN",d_tokens[i],7)==0){
                     irc->nicklen = atoi(separator);
                  }else if(memcmp("MAXNICKLEN",d_tokens[i],10)==0){
                     irc->maxnicklen = atoi(separator);
                  }
               }
            }
         }else if(!memcmp(dresult[1],"332",3) && next!=NULL){//host nick channel topic
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            message = strignorechar(message,CHAR_TRAIL);
            d_result[*s_result] = message;
            (*s_result)++;
            return RECV_TOPIC;
         }else if(!memcmp(dresult[1],"353",3) && next!=NULL){//host channel nicklist
            char *nicks = tokens_required(next,CHAR_SPACE,3,d_result,s_result);
            d_result[(*s_result)-3] = d_result[(*s_result)-1];
            (*s_result)-=2;
            nicks = strignorechar(nicks,CHAR_TRAIL);
            d_result[*s_result] = nicks;
            (*s_result)++;
            return RECV_NICK_LIST;
         }else if(!memcmp(dresult[1],"433",3) && next!=NULL){//host actualnick failednick message
            char *message = tokens_required(next,CHAR_SPACE,2,d_result,s_result);
            if(message!=NULL){
               message = strignorechar(message,CHAR_TRAIL);
               d_result[*s_result] = message;
               (*s_result)++;
            }
            return RECV_NICK_TAKEN;
         }else if(next!=NULL){//host message
            next = strignorechar(next,CHAR_TRAIL);
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
            sprintf(irc->send_buffer, "NICK microirc%d", rand()%1000);
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

__declspec(dllexport) int irc_validate_channel(irc_t *irc, char *channel){
   if(strlen(channel)>irc->channellen || strlen(channel)<2){
      return -1;
   }
   if(strchr(irc->chantypes,channel[0])==NULL){
      return -1;
   }
   return 0;
}

__declspec(dllexport) int irc_validate_nick(irc_t *irc, char *nick){
   if(strlen(nick)>irc->nicklen || strlen(nick)>irc->maxnicklen || strlen(nick)<1){
      return -1;
   }
   char *temp = strchr(irc->prefix_char,nick[0]);
   if(temp==NULL){
      return -1;
   }
   return temp - irc->prefix_char;
}

__declspec(dllexport) void irc_tokenize_nicklist(irc_t *irc, char *nicklist, char **d_result, int *s_result){
   tokens_required(nicklist,CHAR_SPACE,IRCPROTOCOL_MAX_NICKS_PER_MESSAGE,d_result,s_result);
}
