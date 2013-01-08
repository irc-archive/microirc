/*
* network.h
*
* This file contains an network lib implementation.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#ifndef NETWORK_T
#define NETWORK_T

typedef struct network_t{
   char *host;
   int port;
   int socket;
}network_t;

typedef struct data_t{
   unsigned int size;
	char *data;
}data_t;

#ifdef _WIN32
   export int WSAinit_tcp();
   export void WSAdestroy_tcp();
#endif

export int bind_tcp(network_t*, char*, char*);
export int accept_tcp(network_t*, network_t*);
export int connect_tcp(network_t*, char*, char*);
export void close_tcp(network_t*);

export int sendbytes_tcp(network_t*, data_t*, int);
export int getbytes_tcp(network_t*, data_t*, int);

export int gettext_tcp(network_t*, buffer_t*);
export int sendtext_tcp(network_t*, buffer_t*, char*, int);

#endif
