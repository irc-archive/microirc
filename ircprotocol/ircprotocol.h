/*
* ircprotocol.h
*
* This file contains an ircprotocol lib implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2009-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#ifndef IRCPROTOCOL_T
#define IRCPROTOCOL_T

#define RECV_PRIVMSG 50
#define RECV_NICK 51
#define RECV_JOIN 52
#define RECV_PART 53
#define RECV_QUIT 54
#define RECV_KICK 55
#define RECV_WELCOME 56
#define RECV_NICK_TAKEN 57
#define RECV_NICK_LIST 58
#define RECV_OTHER 59
#define RECV_CTCP 60
#define RECV_REGISTERED 61
#define RECV_NOTICE 62
#define RECV_TOPIC 63
#define RECV_TOPIC_CHANGED 64
#define RECV_CHANNEL_MODE 65
#define RECV_NICK_MODE 66

#define SEND_PRIVMSG 80
#define SEND_JOIN 81
#define SEND_PART 82
#define SEND_KICK 83
#define SEND_QUIT 84
#define SEND_PONG 85
#define SEND_NICK 86
#define SEND_USER 87
#define SEND_RAW 88
#define SEND_CTCP 89
#define SEND_SET_TOPIC 90
#define SEND_GET_TOPIC 91
#define SEND_CHANNEL_MODE 92
#define SEND_NICK_MODE 93
#define SEND_NAMES 94

#define CHAR_SPACE ' '
#define CHAR_TRAIL ':'
#define CHAR_MASKSEPARATOR1 '!'
#define CHAR_MASKSEPARATOR2 '@'
#define CHAR_LISTSEPARATOR ','
#define CHAR_ISUPPORTSEPARATOR '='
#define CHAR_PREFIXDELIMITER ')'
#define CHAR_CTCP 1
#define CHAR_TERMINATOR 0

#define IRCPROTOCOL_MAX_NICKS_PER_MESSAGE 256
#define IRCPROTOCOL_CONFIG_MAX_TOKENS 10
#define IRCPROTOCOL_RECV_MAX_TOKENS 15
#define IRCPROTOCOL_REGISTER_TIMEOUT 30000

#define IRCPROTOCOL_SIZE_SMALL 256
#define IRCPROTOCOL_SIZE_MEDIUM 1024
#define IRCPROTOCOL_SIZE_LARGE 16384

#define IRCPROTOCOL_SPLITER_DATA "\r\n"
#define IRCPROTOCOL_SPLITER_SIZE 2

typedef struct irc_t{
   char prefix_mode[IRCPROTOCOL_SIZE_SMALL];
   char prefix_char[IRCPROTOCOL_SIZE_SMALL];
   char chantypes[IRCPROTOCOL_SIZE_SMALL];
   int channellen;
   int nicklen;
   int maxnicklen;
   
   char host[IRCPROTOCOL_SIZE_SMALL];
   char port[IRCPROTOCOL_SIZE_SMALL];
   char user[IRCPROTOCOL_SIZE_SMALL];
   char name[IRCPROTOCOL_SIZE_SMALL];
   char nick[IRCPROTOCOL_SIZE_SMALL];
   char perform[IRCPROTOCOL_SIZE_MEDIUM];
   char autojoin_channels[IRCPROTOCOL_SIZE_MEDIUM];
   int autojoin_delay;
   
   network_t network;
   char recv_buffer[IRCPROTOCOL_SIZE_MEDIUM];
   buffer_t recv_buffer_stream;
   char send_buffer[IRCPROTOCOL_SIZE_MEDIUM];
   CRITICAL_SECTION send_buffer_critical_section;
   int connected;
}irc_t;

export int irc_init(irc_t*, char*, char*, char*, char*, char*, char*, char*, int);
export void irc_destroy(irc_t*);

export int irc_connect(irc_t*);
export void irc_disconnect(irc_t*, char*);
export int irc_recv_message(irc_t*, char**, int*);
export int irc_send_message(irc_t*, int, char**, int);
export int irc_validate_channel(irc_t*, char*);
export char *irc_get_nick(irc_t*, char*);
export void irc_tokenize_nicklist(irc_t*, char*, char**, int*);

#endif
