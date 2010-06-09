#ifndef LIST_T
#define LIST_T

#define LIST_MAX_SIZE 268435456
#define LIST_MIN_SIZE 1024

typedef struct list_t{
   int size;
   int data_size;
   int current_size;
   char *list;
}list_t;

__declspec(dllexport) int list_init(list_t*, int);
__declspec(dllexport) void list_destroy(list_t*);

__declspec(dllexport) void *list_add(list_t*, void*);
__declspec(dllexport) void *list_add_index(list_t*, int, void*);
__declspec(dllexport) int list_get(list_t*, void*);
__declspec(dllexport) void *list_get_index(list_t*, int, void*, int);
__declspec(dllexport) int list_remove(list_t*, void*);
__declspec(dllexport) int list_remove_index(list_t*, int, void*, int);
__declspec(dllexport) int list_size(list_t*);

__declspec(dllexport) int list_array_data(list_t*,void**);
__declspec(dllexport) int list_array_references(list_t*,void**);
__declspec(dllexport) void list_array_destroy(void*);

#endif
