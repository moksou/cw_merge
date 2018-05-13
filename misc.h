int name_exist(str8_t str, str8_t* arr, int arrsize);
list_t *list_init();
void list_add(list_t *list, void *item, size_t itemsize, int dup);
int myalloc(void** pointer, size_t size);
void list_cleanup(list_t* list);
