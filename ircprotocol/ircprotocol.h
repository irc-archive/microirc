/*
* ircprotocol.h
*
* This file contains an ircprotocol lib implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2009-2010, Jos� Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

//It's still unknown whether timeSetEvent() actually uses this value or not...
#define TIME_KILL_SYNCHRONOUS 64

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

#define CHAR_SPACE ' '
#define CHAR_COLON ':'
#define CHAR_EXCLAMATAIONMARK '!'
#define CHAR_ATSIGN '@'
#define CHAR_COMMA ','
#define CHAR_CTCP 1
#define CHAR_TERMINATOR 0

#define IRC_CONFIG_MAX_TOKENS 10
#define IRC_RECV_MAX_TOKENS 10
#define IRC_REGISTER_TIMEOUT 30000

#define IRC_BUFFER_SIZE_LITTLE 256
#define IRC_BUFFER_SIZE_MEDIUM 1024
#define IRC_BUFFER_SIZE_LARGE 16384

#define IRC_SPLITER_DATA "\r\n"
#define IRC_SPLITER_SIZE 2

typedef struct irc_t{
   char host[IRC_BUFFER_SIZE_LITTLE];
   char port[IRC_BUFFER_SIZE_LITTLE];
   char user[IRC_BUFFER_SIZE_LITTLE];
   char name[IRC_BUFFER_SIZE_LITTLE];
   char nick[IRC_BUFFER_SIZE_LITTLE];
   char perform[IRC_BUFFER_SIZE_MEDIUM];
   char autojoin_channels[IRC_BUFFER_SIZE_MEDIUM];
   int autojoin_delay;
   
   network_t network;
   char recv_buffer[IRC_BUFFER_SIZE_MEDIUM];
   buffer_t recv_buffer_stream;
   char send_buffer[IRC_BUFFER_SIZE_MEDIUM];
   CRITICAL_SECTION send_buffer_critical_section;
   int connected;
}irc_t;

__declspec(dllexport) int irc_init(irc_t*, char*, char*, char*, char*, char*, char*, char*, int);
__declspec(dllexport) void irc_destroy(irc_t*);

__declspec(dllexport) int irc_connect(irc_t*);
__declspec(dllexport) void irc_disconnect(irc_t*, char*);
__declspec(dllexport) int irc_recv_message(irc_t*, char**, int*);
__declspec(dllexport) int irc_send_message(irc_t*, int, char**, int);
