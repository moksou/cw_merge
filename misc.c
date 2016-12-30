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
