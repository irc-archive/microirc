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

__declspec(dllexport) int bind_tcp(network_t*, char*, char*);
__declspec(dllexport) int accept_tcp(network_t*, network_t*);
__declspec(dllexport) int connect_tcp(network_t*, char*, char*);
__declspec(dllexport) void close_tcp(network_t*);

__declspec(dllexport) int new_data_t(data_t*, char*, int);
__declspec(dllexport) void destroy_data_t(data_t*);
__declspec(dllexport) void print_data_t(data_t*);
__declspec(dllexport) int sendbytes_tcp(network_t*, data_t*, int);
__declspec(dllexport) int getbytes_tcp(network_t*, data_t*, int);

__declspec(dllexport) int gettext_tcp(network_t*, buffer_t*);
__declspec(dllexport) int sendtext_tcp(network_t*, buffer_t*, char*, int);

#endif
