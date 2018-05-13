#include "globaldefs.h"

lump_t* lump_init(str8_t name)
{
    lump_t *lump;
    if (!name)
        return NULL;
    lump = calloc(1, sizeof(lump_t));
    lump->pos = 0;
    lump->size = 0;
    strncpy(lump->name, name, 8);
    return lump;
}

int lump_isempty(lump_t* lump)
{
    if (lump->pos == 0
          && lump->size == 0
          && strncmp(lump->name, "", 8) == 0)
        return 1;
    return 0;
}

void lump_add(list_t *table, lump_t *lump, int dup)
{
    list_add(table, lump, sizeof(lump_t), dup);
}

lump_t *lump_find(list_t* table, str8_t lumpname)
{
    for (list_t* current = table; current != NULL; current = current->next) {
        lump_t *needle = current->data;
        if (strncmp(needle->name, lumpname, 8) == 0)
            return needle;
    }
    return NULL;
}
