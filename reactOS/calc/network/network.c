/*
* network.c
*
* This file contains an network lib implementation.
* stdout contains debug log
* stderr contains error log
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

//#define _WIN32_WINNT 0x9999

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
   #include <winsock2.h>
   #include <ws2tcpip.h>
#else
   #include <errno.h>
   #include <unistd.h>
   #include <sys/types.h>
   #include <arpa/inet.h>
   #include <netinet/in.h>
   #include <sys/socket.h>
   #include <netdb.h>
#endif
#include "../util/util.h"
#include "../buffer/buffer.h"
#include "network.h"

int get_last_error(){
   #ifdef _WIN32
      return WSAGetLastError();
   #else
      return errno;
   #endif
}

#ifdef _WIN32
   export int WSAinit_tcp(){
      WSADATA wsa_data;
      if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0){
         printf("WSAStartup failed\n");
         return -1;
      }
      return 0;
   }

   export void WSAdestroy_tcp(){
      WSACleanup();
   }
#endif

export int bind_tcp(network_t *network, char *host, char *port){
   memset(network,0,sizeof(network_t));
   int listen_socket;
   struct sockaddr_in service;
   
   listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(listen_socket < 0) {
      fprintf(stderr, "create socket failed\n");
      return -1;
   }
   printf("create socket...done\n");
   service.sin_family = AF_INET;
   service.sin_addr.s_addr = inet_addr(host);
   service.sin_port = htons(atoi(port));
   if(bind(listen_socket, (struct sockaddr*)&service, sizeof(service)) < 0) {
      fprintf(stderr, "bind socket failed\n");
      closesocket(listen_socket);
      return -1;
   }
   printf("bind socket...done\n");
   if(listen(listen_socket, SOMAXCONN) < 0) {
      fprintf(stderr, "listen failed\n");
      closesocket(listen_socket);
      return -1;
   }
   printf("listen...done\n");
   network->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(network->host,host,strlen(host)+sizeof(char));
   network->port = atoi(port);
   network->socket = listen_socket;
   return 0;
}

export int accept_tcp(network_t *server, network_t *client){
   struct sockaddr_in address;
   int size_address = sizeof(address);
   client->socket = accept(server->socket, (struct sockaddr*)&address, &size_address);
   if(client->socket < 0){
      fprintf(stderr, "accept failed: %d\n", get_last_error());
      return -1;
   }
   char *host = inet_ntoa(address.sin_addr);
   client->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(client->host,host,strlen(host)+sizeof(char));
   client->port = address.sin_port;
   return 0;
}

export int connect_tcp(network_t *network, char *host, char *port){
   memset(network,0,sizeof(network_t));
   int client_socket;
   struct addrinfo *result = NULL;
   struct addrinfo *ptr = NULL;
   struct addrinfo hints;
   
   memset(&hints,0,sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   if(getaddrinfo(host,port,&hints,&result)!=0 ) {
      fprintf(stderr, "getting address info failed\n");
      return -1;
   }
   printf("getting address info...done\n");
   for(ptr=result; ptr != NULL ;ptr=ptr->ai_next){
      client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if(client_socket < 0){
         freeaddrinfo(result);
         return -1;
      }
      if(connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen) < 0){
         closesocket(client_socket);
         client_socket = -1;
      }else{
         break;
      }
   }
   freeaddrinfo(result);
   if(client_socket < 0){
      fprintf(stderr, "connect to all address failed\n");
      return -1;
   }
   printf("connect...done\n");
   network->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(network->host,host,strlen(host)+sizeof(char));
   network->port = atoi(port);
   network->socket = client_socket;
   return 0;
}

export void close_tcp(network_t *network){
	closesocket(network->socket);
   free(network->host);
   memset(network,0,sizeof(network_t));
}

export int sendbytes_tcp(network_t *network, data_t *send_data, int b_free){
   unsigned int size = send_data->size;
   char *data = send_data->data;
   int result = send(network->socket, (char*)&size, sizeof(int), 0);
   if(result < 0){
      fprintf(stderr, "send size failed: %d\n", get_last_error());
      goto error1;
   }else if(result == 0){
      fprintf(stderr, "connection already closed\n");
      goto error1;
   }
   printf("send size: %u\n",size);
   while(size > 0){
      result = send(network->socket, data, size, 0);
      if(result < 0){
         fprintf(stderr, "send msg failed: %d\n", get_last_error());
         goto error1;
      }else if(result == 0){
         fprintf(stderr, "connection already closed\n");
         goto error1;
      }
      size = size - result;
      data = data + result;
   }
   if(b_free!=0){
      free(send_data->data);
   }
   return size;
   error1:
      if(b_free!=0){
         free(send_data->data);
      }
      return -1;
}

export int getbytes_tcp(network_t *network, data_t *recv_data, int b_malloc){
   unsigned int size;
   char *data;
   int result;
   if(b_malloc != 2){
      result = recv(network->socket, (char*)&size, sizeof(int), 0);
      if(result < 0){
         fprintf(stderr, "recv size failed: %d\n", get_last_error());
         goto error1;
      }else if(result == 0){
         fprintf(stderr, "connection already closed\n");
         goto error1;
      }
      printf("recv size: %u\n",size);
      recv_data->size = size;
      if(b_malloc == 1){
         data = (char*)malloc(size);
         if(data == NULL){
            goto error1;
         }
         recv_data->data = data;
      }else{
         if(recv_data->size < size){
            goto error1;
         }
         data = recv_data->data;
      }
   }else{
      size = recv_data->size;
      data = recv_data->data;
   }
   while(size > 0){
      result = recv(network->socket, data, size, 0);
      if(result < 0){
         fprintf(stderr, "recv msg failed: %d\n", get_last_error());
         goto error1;
      }else if(result == 0){
         fprintf(stderr, "connection already closed\n");
         goto error1;
      }
      size = size - result;
      data = data + result;
   }
   if(b_malloc == 1){
      free(recv_data->data);
   }
   return size;
   error1:
      if(b_malloc == 1){
         free(recv_data->data);
      }
      return -1;
}

export int gettext_tcp(network_t *network, buffer_t *buffer){
   int retval=0;
   int result = buffer_get_avaiable_data_size(buffer);
   if(result!=0){
      return result;
   }
   while(retval==0){
      result = recv(network->socket, buffer_get_avaiable_write_buffer(buffer), buffer_get_avaiable_write_size(buffer), 0);
      if(result < 0){
         fprintf(stderr, "recv msg failed: %d\n", get_last_error());
         return -1;
      }else if(result == 0){
         fprintf(stderr, "connection already closed\n");
         return -1;
      }
      retval = buffer_write_size_after(buffer,result);
   }
   return buffer_get_avaiable_data_size(buffer);
}

export int sendtext_tcp(network_t *network, buffer_t *buffer, char *data, int size){
   int result;
   memcpy(data+(size-buffer->s_split),buffer->d_split,buffer->s_split);
   while(size>0){
      result = send(network->socket, data, size, 0);
      if(result < 0){
         fprintf(stderr, "send msg failed: %d\n", get_last_error());
         return -1;
      }else if(result == 0){
         fprintf(stderr, "connection already closed\n");
         return -1;
      }
      size = size - result;
      data = data + result;
   }
   return 0;
}
