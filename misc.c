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
    
}

void list_delete(list_t *list, list_t *item, size_t itemsize)
{
    list_t *current;

    if (memcmp(list->data, item->data, itemsize) == 0) {
        printf("test\n");
        list = list->next;
    }
    for (current = list; current->next != NULL; current = current->next) {
        list_t *next = current->next;
        //printf("test\n");
        if (memcmp(next->data, item->data, itemsize) == 0) {
            current->next = next->next;
            //free(next->data);
            //free(next);
        }
    }
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

array_t *array_init()
{
    array_t *arr = calloc(1, sizeof(array_t));
    return arr;
}

void array_add(array_t *arr, str8_t str, uint32_t index, int dup)
{
    if (index > arr->num)
        return;
    
    if (dup) {
        for (size_t i = 0; i < arr->num; i++) {
            if (strncmp(arr->string[i], str, sizeof(str8_t)) == 0)
                return;
        }
    }

    arr->string = realloc(arr->string, sizeof(str8_t) * (arr->num + 1));
    memmove(&arr->string[index + 1], &arr->string[index], sizeof(str8_t) * (arr->num - index));

    strncpy(arr->string[index], str, 8);
    arr->num++;
}


void array_cleanup(array_t *arr)
{
    free(arr->string);
    free(arr);
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
