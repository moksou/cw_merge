int name_exist(str8_t str, str8_t* arr, int arrsize);
list_t *list_init();
void list_add(list_t *list, void *item, size_t itemsize, int dup);
void list_delete(list_t *list, list_t *item, size_t itemsize);
int myalloc(void** pointer, size_t size);
void list_cleanup(list_t* list);
array_t *array_init();
void array_add(array_t *arr, str8_t str, uint32_t index, int dup);
void array_cleanup(array_t *arr);
