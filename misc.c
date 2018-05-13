#include "globaldefs.h"

int name_exist(str8_t str, str8_t* arr, int arrsize)
{
    if (!str || !arrsize || arr == NULL)
        return 0;
    for (int i = 0; i < arrsize; i++) {
        if (strncmp(str, arr[i], 8) == 0)
            return 1;
    }
    return 0;
}

list_t *list_init()
{
    list_t *list;
    list = calloc(1, sizeof(list_t));
    //list->data = calloc(1, itemsize);
    list->next = NULL;
    if (list == NULL) {
        fprintf(stderr, "List initialization failed\n");
        exit(-1);
    }
    return list;
}

void list_add(list_t *list, void *item, size_t itemsize, int dup)
{
    list_t *current,
           *last;

    for (current = list; current != NULL; current = current->next) {
        if (current->data == NULL) {
            current->data = calloc(1, itemsize);
            memcpy(current->data, item, itemsize);
            return;
        }
        if (dup && memcmp(current->data, item, itemsize) == 0) {
            //fprintf(stderr, "Item already exists in list\n");
            return;
        }
        if (current->next == NULL) {
            last = list_init();
            last->data = calloc(1, itemsize);
            memcpy(last->data, item, itemsize);
            current->next = last;
            return;
        }
    }
    

    

    //printf("---%.8s\n", &list->data[8]);
}


void list_cleanup(list_t* list)
{
    list_t *current = list;

    while (current != NULL) {
        list_t *tmp = current;
        current = current->next;
        free(tmp->data);
        free(tmp);
    }
}
        

int myalloc(void** pointer, size_t size)
{
    *pointer = malloc(size);
    if (*pointer == NULL) {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    return 0;
}

//int myrealloc(void** pointer, size_t size)
//{
//    *pointer = realloc(*pointer, size);
//    if (*pointer == NULL) {
//        fprintf(stderr, "realloc error\n");
//        exit(-1);
//    }
//    return 0;
//}
