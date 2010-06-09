/*
* network.c
*
* This file contains an network lib implementation. (Windows only)
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

#define _WIN32_WINNT 0x9999

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "buffer.h"
#include "network.h"

__declspec(dllexport) int bind_tcp(network_t *network, char *host, char *port){
   memset(network,0,sizeof(network_t));
   WSADATA wsa_data;
   int listen_socket;
   struct sockaddr_in service;

   if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
      printf("WSAStartup failed\n");
      return -1;
   }
   listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(listen_socket == INVALID_SOCKET) {
      printf("create socket failed\n");
      WSACleanup();
      return -1;
   }
   printf("create socket...done\n");
   service.sin_family = AF_INET;
   service.sin_addr.s_addr = inet_addr(host);
   service.sin_port = htons(atoi(port));
   if(bind(listen_socket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
      printf("bind failed\n");
      closesocket(listen_socket);
      WSACleanup();
      return -1;
   }
   printf("bind...done\n");
   if(listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
      printf("listen failed\n");
      closesocket(listen_socket);
      WSACleanup();
      return -1;
   }
   printf("listen...done\n");
   network->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(network->host,host,strlen(host)+sizeof(char));
   network->port = atoi(port);
   network->socket = listen_socket;
   return 0;
}

__declspec(dllexport) int accept_tcp(network_t *server, network_t *client){
   struct sockaddr_in address;
   int size_address = sizeof(address);
   client->socket = accept(server->socket, (struct sockaddr*)&address, &size_address);
   if(client->socket == INVALID_SOCKET){
      printf("accept failed: %d\n", WSAGetLastError());
      return -1;
   }
   char *host = inet_ntoa(address.sin_addr);
   client->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(client->host,host,strlen(host)+sizeof(char));
   client->port = address.sin_port;
   return 0;
}

__declspec(dllexport) int connect_tcp(network_t *network, char *host, char *port){
   memset(network,0,sizeof(network_t));
   WSADATA wsa_data;
   int client_socket;
   struct addrinfo *result = NULL;
   struct addrinfo *ptr = NULL;
   struct addrinfo hints;
   
   if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0){
      return -1;
   }
   memset(&hints,0,sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   if(getaddrinfo(host,port,&hints,&result)!=0 ) {
      printf("getting address info failed\n");
      WSACleanup();
      return -1;
   }
   printf("getting address info...done\n");
   for(ptr=result; ptr != NULL ;ptr=ptr->ai_next){
      client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if(client_socket == INVALID_SOCKET) {
         freeaddrinfo(result);
         WSACleanup();
         return -1;
      }
      if(connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR){
         closesocket(client_socket);
         client_socket = INVALID_SOCKET;
      }else{
         break;
      }
   }
   freeaddrinfo(result);
   if(client_socket == INVALID_SOCKET){
      printf("connect to all address failed\n");
      WSACleanup();
      return -1;
   }
   printf("connect...done\n");
   network->host = (char*)malloc((strlen(host)+sizeof(char))*sizeof(char));
   memcpy(network->host,host,strlen(host)+sizeof(char));
   network->port = atoi(port);
   network->socket = client_socket;
   return 0;
}

__declspec(dllexport) void close_tcp(network_t *network){
	closesocket(network->socket);
   free(network->host);
	WSACleanup();
   memset(network,0,sizeof(network_t));
}

__declspec(dllexport) int sendbytes_tcp(network_t *network, data_t *send_data, int b_free){
   unsigned int size = send_data->size;
   char *data = send_data->data;
   int result = send(network->socket, (char*)&size, sizeof(int), 0);
   if(result < 0){
      printf("send size failed: %d\n", WSAGetLastError());
      goto error1;
   }else if(result == 0){
      printf("connection already closed\n");
      goto error1;
   }
   printf("send size:%u\n",size);
   while(size > 0){
      result = send(network->socket, data, size, 0);
      if(result < 0){
         printf("send msg failed: %d\n", WSAGetLastError());
         goto error1;
      }else if(result == 0){
         printf("connection already closed\n");
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

__declspec(dllexport) int getbytes_tcp(network_t *network, data_t *recv_data, int b_malloc){
   unsigned int size;
   char *data;
   int result;
   if(b_malloc != 2){
      result = recv(network->socket, (char*)&size, sizeof(int), 0);
      if(result < 0){
         printf("recv size failed: %d\n", WSAGetLastError());
         goto error1;
      }else if(result == 0){
         printf("connection already closed\n");
         goto error1;
      }
      printf("recv size:%u\n",size);
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
         printf("recv msg failed: %d\n", WSAGetLastError());
         goto error1;
      }else if(result == 0){
         printf("connection already closed\n");
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

__declspec(dllexport) int new_data_t(data_t *data, void *bytes, int size){
   data->data = (char*)malloc(size);
   if(data->data == NULL){
      return -1;
   }
   if(bytes!=NULL){
      memcpy(data->data,bytes,size);
   }
   data->size = size;
   return 0;
}

__declspec(dllexport) void destroy_data_t(data_t *data){
   free(data->data);
   memset(data,0,sizeof(data_t));
}

__declspec(dllexport) void print_data_t(data_t *data){
   int i;
   printf("DATA_T size:%d ",data->size);
   for(i=0;i<data->size;i++){
      printf("%c",*(char*)(data->data+i));
   }
   printf(".\n");
}

__declspec(dllexport) int gettext_tcp(network_t *network, buffer_t *buffer){
   int retval=0;
   int result = buffer_read_avaiable_data_size(buffer);
   if(result>0){
      return result;
   }
   while(retval==0){
      result = recv(network->socket, buffer_read_avaiable_write_buffer(buffer), buffer_read_avaiable_write_size(buffer), 0);
      if(result < 0){
         printf("recv msg failed: %d\n", WSAGetLastError());
         return -1;
      }else if(result == 0){
         printf("connection already closed\n");
         return -1;
      }
      retval = buffer_write_size_after(buffer,result);
   }
   return buffer_read_avaiable_data_size(buffer);
}

__declspec(dllexport) int sendtext_tcp(network_t *network, buffer_t *buffer, char *data, int size){
   int result;
   memcpy(data+(size-buffer->s_split),buffer->d_split,buffer->s_split);
   while(size>0){
      result = send(network->socket, data, size, 0);
      if(result < 0){
         printf("send msg failed: %d\n", WSAGetLastError());
         return -1;
      }else if(result == 0){
         printf("connection already closed\n");
         return -1;
      }
      size = size - result;
      data = data + result;
   }
   return 0;
}
